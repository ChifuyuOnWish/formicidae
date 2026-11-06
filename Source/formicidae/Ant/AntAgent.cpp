#include "AntAgent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Pheromone/PheromoneManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogAnt, Log, All);

AAntAgent::AAntAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void AAntAgent::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    PheromoneManager = World ? World->GetSubsystem<UPheromoneManager>() : nullptr;
    ensure(PheromoneManager);

    const double Angle = FMath::FRandRange(0.0, 360.0);
    Heading = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)),
                      FMath::Sin(FMath::DegreesToRadians(Angle)), 0.0);
    DesiredHeading = Heading;

    DecisionTimer = FMath::FRandRange(0.f, DecisionInterval);

    FindHome();
    FindFood();
}

void AAntAgent::SetHeading(const FVector& NewHeading)
{
    FVector Flat = NewHeading;
    Flat.Z = 0.0;
    if (Flat.IsNearlyZero())
    {
        return;
    }
    Heading = Flat.GetSafeNormal();
    DesiredHeading = Heading;
}

void AAntAgent::SetFood(AFoodSource* NewFood)
{
    Food = NewFood;
}

void AAntAgent::FindHome()
{
    Home = nullptr;
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TActorIterator<AAntHome> It(World);
    if (It)
    {
        Home = *It;
    }
}

void AAntAgent::FindFood()
{
    Food = nullptr;
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const FVector Here = GetActorLocation();
    double BestDistance = FoodSenseRadius;
    for (TActorIterator<AFoodSource> It(World); It; ++It)
    {
        if (It->GetAmount() <= 0)
        {
            continue;
        }
        const double Distance = FVector::Dist2D(Here, It->GetActorLocation());
        if (Distance <= BestDistance)
        {
            BestDistance = Distance;
            Food = *It;
        }
    }
}

void AAntAgent::UpdateState()
{
    const FVector Here = GetActorLocation();

    if (State == EAntState::Exploring)
    {
        AFoodSource* F = Food.Get();
        if (F && FVector::Dist2D(Here, F->GetActorLocation()) <= F->PickupRadius)
        {
            const int32 Taken = F->TryTake(CarryCapacity);
            if (Taken > 0)
            {
                Carried = Taken;
                State = EAntState::Returning;
                DistanceSinceDeposit = 0.f;
                DecisionTimer = 0.f;

                if (const AAntHome* H = Home.Get())
                {
                    FVector ToHome = H->GetActorLocation() - Here;
                    ToHome.Z = 0.0;
                    if (!ToHome.IsNearlyZero())
                    {
                        Heading = ToHome.GetSafeNormal();
                        DesiredHeading = Heading;
                    }
                }
                UE_LOG(LogAnt, Log, TEXT("%s took %d food, heading home."), *GetName(), Taken);
            }
            else
            {
                Food = nullptr;
            }
        }
    }
    else
    {
        AAntHome* H = Home.Get();
        if (H && FVector::Dist2D(Here, H->GetActorLocation()) <= H->ReceiveRadius)
        {
            H->Store(Carried);
            Carried = 0;
            ++TripCount;
            State = EAntState::Exploring;
            DistanceSinceDeposit = 0.f;
            DecisionTimer = 0.f;
            Food = nullptr;

            Heading = -Heading;
            DesiredHeading = Heading;
            UE_LOG(LogAnt, Log, TEXT("%s delivered. Trips completed: %d"), *GetName(), TripCount);
        }
    }
}

FVector AAntAgent::ChooseDirection()
{
    const FVector Here = GetActorLocation();

    if (State == EAntState::Returning)
    {
        if (!Home.IsValid())
        {
            FindHome();
        }
        if (const AAntHome* H = Home.Get())
        {
            FVector ToHome = H->GetActorLocation() - Here;
            ToHome.Z = 0.0;
            if (!ToHome.IsNearlyZero())
            {
                return ToHome.GetSafeNormal();
            }
        }
        return Heading;
    }

    const AFoodSource* Current = Food.Get();
    if (!Current || Current->GetAmount() <= 0)
    {
        FindFood();
    }
    if (const AFoodSource* F = Food.Get())
    {
        FVector ToFood = F->GetActorLocation() - Here;
        ToFood.Z = 0.0;
        if (!ToFood.IsNearlyZero())
        {
            return ToFood.GetSafeNormal();
        }
    }
    return ComputeDesiredDirection();
}

FVector AAntAgent::ComputeDesiredDirection() const
{
    if (!PheromoneManager)
    {
        return Heading;
    }

    const double ConeThreshold = FMath::Cos(FMath::DegreesToRadians(ConeHalfAngleDeg));
    const FVector Origin = GetActorLocation();
    FVector Sum = FVector::ZeroVector;

    PheromoneManager->ForEachNearby(Origin, SenseRadius, EPheromoneDepositType::FORAGE,
        [&](const FPheromoneDeposit& Entry)
        {
            FVector ToDeposit = Entry.Position - Origin;
            ToDeposit.Z = 0.0;

            const double Distance = ToDeposit.Size();
            if (Distance < DeadZoneRadius || Distance < KINDA_SMALL_NUMBER)
            {
                return;
            }

            const FVector Direction = ToDeposit / Distance;
            if (FVector::DotProduct(Heading, Direction) < ConeThreshold)
            {
                return;
            }

            Sum += Direction * (Entry.Strength / Distance);
        });

    if (Sum.IsNearlyZero())
    {
        const double Jitter = FMath::FRandRange(-ExplorationJitterDeg, ExplorationJitterDeg);
        return Heading.RotateAngleAxis(Jitter, FVector::UpVector);
    }

    return Sum.GetSafeNormal();
}

void AAntAgent::UpdateHeading(float DeltaTime)
{
    const double Cross = FVector::CrossProduct(Heading, DesiredHeading).Z;
    const double Dot = FVector::DotProduct(Heading, DesiredHeading);
    double Angle = FMath::RadiansToDegrees(FMath::Atan2(Cross, Dot));

    const double MaxStep = MaxTurnRateDeg * DeltaTime;
    Angle = FMath::Clamp(Angle, -MaxStep, MaxStep);

    Heading = Heading.RotateAngleAxis(Angle, FVector::UpVector);
    Heading.Z = 0.0;
    Heading = Heading.GetSafeNormal();
}

float AAntAgent::MoveForward(float DeltaTime)
{
    const float Distance = MoveSpeed * DeltaTime;
    AddActorWorldOffset(Heading * Distance);
    return Distance;
}

void AAntAgent::LayPheromone(float DistanceMoved)
{
    if (!PheromoneManager)
    {
        return;
    }

    DistanceSinceDeposit += DistanceMoved;
    if (DistanceSinceDeposit >= DepositDistance)
    {
        PheromoneManager->Deposit(GetActorLocation(),
            EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
        DistanceSinceDeposit -= DepositDistance;
    }
}

void AAntAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PheromoneManager)
    {
        return;
    }

    UpdateState();

    DecisionTimer -= DeltaTime;
    if (DecisionTimer <= 0.f)
    {
        DesiredHeading = ChooseDirection();
        DecisionTimer = FMath::Max(DecisionTimer + DecisionInterval, 0.f);
    }

    UpdateHeading(DeltaTime);
    const float Moved = MoveForward(DeltaTime);

    if (State == EAntState::Returning || bDepositWhileExploring)
    {
        LayPheromone(Moved);
    }

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        const FColor Color = (State == EAntState::Returning) ? FColor::Orange : FColor::Yellow;
        DrawDebugSphere(World, GetActorLocation() + FVector(0.0, 0.0, 8.0),
            8.f, 12, Color, false, -1.f, 0, 1.5f);
    }
#endif
}