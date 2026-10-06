#include "PheromoneManager.h"
#include "DepositFactory.h"
#include "PheromoneSettings.h"
#include "Engine/World.h"

bool UPheromoneManager::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer))
    {
        return false;
    }
    const UWorld* World = Cast<UWorld>(Outer);
    return World && World->IsGameWorld();
}

void UPheromoneManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Settings = GetDefault<UPheromoneSettings>();
    DepositFactory = NewObject<UDepositFactory>(this);
    ensure(Settings);
    ensure(DepositFactory);
}

void UPheromoneManager::Tick(float DeltaTime)
{
    for (FPheromoneDeposit& Entry : Deposits)
    {
        Entry.Strength -= Entry.DecayRate * DeltaTime;
    }
    Deposits.RemoveAllSwap([](const FPheromoneDeposit& Entry) { return Entry.Strength <= 0.f; });
}

TStatId UPheromoneManager::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UPheromoneManager, STATGROUP_Tickables);
}

void UPheromoneManager::Deposit(const FVector& Position, EPheromoneDepositType Type, EPheromoneDepositOrigin Origin)
{
    if (!ensure(DepositFactory))
    {
        return;
    }

    const FPheromoneDeposit NewDeposit = DepositFactory->Create(Position, Type, Origin);
    if (!TryMergeIntoExisting(NewDeposit))
    {
        Deposits.Add(NewDeposit);
    }
}

float UPheromoneManager::GetRadius(const FPheromoneDeposit& Entry) const
{
    if (Entry.MaxStrength <= 0.f)
    {
        return Settings->MinDepositRadius;
    }

    const float Alpha = FMath::Clamp(Entry.Strength / Entry.MaxStrength, 0.f, 1.f);
    return FMath::Lerp(Settings->MinDepositRadius, Settings->MaxDepositRadius, Alpha);
}

void UPheromoneManager::ForEachNearby(const FVector& Position, float Radius, EPheromoneDepositType Type,
                                      TFunctionRef<void(const FPheromoneDeposit&)> Visitor) const
{
    for (const FPheromoneDeposit& Entry : Deposits)
    {
        if (Entry.Type != Type)
        {
            continue;
        }

        const float Reach = Radius + GetRadius(Entry);
        if (FVector::DistSquared(Entry.Position, Position) <= FMath::Square(Reach))
        {
            Visitor(Entry);
        }
    }
}

bool UPheromoneManager::IsValidRedirectionAnchor(const FVector& Position) const
{
    for (const FPheromoneDeposit& Entry : Deposits)
    {
        if (Entry.Type != EPheromoneDepositType::FORAGE
            || Entry.Origin != EPheromoneDepositOrigin::ANT
            || Entry.Strength < Settings->MinAnchorStrength) // CHANGED: reads from settings
        {
            continue;
        }

        const float Reach = GetRadius(Entry);
        if (FVector::DistSquared(Entry.Position, Position) <= FMath::Square(Reach))
        {
            return true;
        }
    }
    return false;
}

bool UPheromoneManager::TryPlaceRedirection(const TArray<FVector>& Path)
{
    if (!ensure(DepositFactory) || Path.Num() < 2)
    {
        return false;
    }

    if (!IsValidRedirectionAnchor(Path[0]) || !IsValidRedirectionAnchor(Path.Last()))
    {
        return false;
    }

    const FPheromoneDeposit Sample = DepositFactory->Create(FVector::ZeroVector,
        EPheromoneDepositType::REDIRECTION, EPheromoneDepositOrigin::PLAYER);

    const double Spacing = GetRadius(Sample) * 1.1;

    if (!ensure(Spacing > KINDA_SMALL_NUMBER))
    {
        return false;
    }

    TArray<FVector> Points;
    Points.Add(Path[0]);

    double Carry = 0.0;

    for (int32 i = 1; i < Path.Num(); ++i)
    {
        FVector Cursor = Path[i - 1];
        const double SegmentLength = FVector::Dist(Cursor, Path[i]);
        if (SegmentLength <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const FVector Direction = (Path[i] - Cursor) / SegmentLength;
        double Remaining = SegmentLength;
        double Needed = Spacing - Carry;

        while (Remaining >= Needed)
        {
            Cursor += Direction * Needed;
            Remaining -= Needed;
            Points.Add(Cursor);
            Carry = 0.0;
            Needed = Spacing;
        }

        Carry += Remaining;
    }
    Points.Add(Path.Last());

    for (const FVector& Point : Points)
    {
        Deposit(Point, EPheromoneDepositType::REDIRECTION, EPheromoneDepositOrigin::PLAYER);
    }
    return true;
}

bool UPheromoneManager::TryMergeIntoExisting(const FPheromoneDeposit& NewDeposit)
{
    const float NewRadius = GetRadius(NewDeposit);

    FPheromoneDeposit* ClosestDeposit = nullptr;
    double ClosestDistanceSquared = 0.0;

    for (FPheromoneDeposit& Entry : Deposits)
    {
        if (Entry.Type != NewDeposit.Type)
        {
            continue;
        }

        const double DistanceSquared = FVector::DistSquared(Entry.Position, NewDeposit.Position);

        const float Range = FMath::Max(GetRadius(Entry), NewRadius);

        if (DistanceSquared <= FMath::Square(Range))
        {
            if (!ClosestDeposit || DistanceSquared < ClosestDistanceSquared)
            {
                ClosestDeposit = &Entry;
                ClosestDistanceSquared = DistanceSquared;
            }
        }
    }

    if (!ClosestDeposit)
    {
        return false;
    }

    const float TotalStrength = ClosestDeposit->Strength + NewDeposit.Strength;
    const float DriftWeight = TotalStrength > KINDA_SMALL_NUMBER ? NewDeposit.Strength / TotalStrength : 0.5f;
    ClosestDeposit->Position += (NewDeposit.Position - ClosestDeposit->Position) * DriftWeight;

    ClosestDeposit->Strength = FMath::Min(ClosestDeposit->Strength + NewDeposit.Strength, ClosestDeposit->MaxStrength);

    if (NewDeposit.Origin == EPheromoneDepositOrigin::ANT)
    {
        ClosestDeposit->Origin = EPheromoneDepositOrigin::ANT;
    }

    return true;
}

void UPheromoneManager::ForEach(TFunctionRef<void(const FPheromoneDeposit&)> Visitor) const
{
    for (const FPheromoneDeposit& Entry : Deposits)
    {
        Visitor(Entry);
    }
}

#if !UE_BUILD_SHIPPING
void UPheromoneManager::Clear()
{
    Deposits.Reset();
}
#endif

int32 UPheromoneManager::Count() const
{
    return Deposits.Num();
}
