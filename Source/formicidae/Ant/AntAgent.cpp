#include "AntAgent.h"
#include "Components/SceneComponent.h"
#include "Pheromone/PheromoneManager.h"
#include "DrawDebugHelpers.h"

AAntAgent::AAntAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void AAntAgent::BeginPlay()
{
    Super::BeginPlay();
    PheromoneManager = GetWorld() ? GetWorld()->GetSubsystem<UPheromoneManager>() : nullptr;
    ensure(PheromoneManager);

    float RandomAngle = FMath::FRandRange(0.f, 360.f);
    FVector RandomDirection = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.f);
    Heading = RandomDirection.GetSafeNormal();
    DesiredHeading = Heading;
    DecisionTimer = FMath::FRandRange(0.f, DecisionInterval);
}

float AAntAgent::MoveForward(float DeltaTime)
{
    float Distance = MoveSpeed * DeltaTime;
    AddActorWorldOffset(Heading * Distance);
    return Distance;
}

void AAntAgent::LayPheromone(float DistanceMoved)
{
    if (!PheromoneManager)
        return;

    DistanceSinceDeposit += DistanceMoved;
    if (DistanceSinceDeposit >= DepositDistance)
    {
        PheromoneManager->Deposit(GetActorLocation(), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
        DistanceSinceDeposit -= DepositDistance;
    }
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

void AAntAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PheromoneManager)
    {
        return;
    }

    DecisionTimer -= DeltaTime;
    if (DecisionTimer <= 0.f)
    {
        DesiredHeading = ComputeDesiredDirection();
        DecisionTimer = FMath::Max(DecisionTimer + DecisionInterval, 0.f);
    }

    UpdateHeading(DeltaTime);
    LayPheromone(MoveForward(DeltaTime));

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        DrawDebugSphere(World,
            GetActorLocation() + FVector(0.0, 0.0, 8.0),
            8.f, 12, FColor::Yellow, false, -1.f, 0, 1.5f);
    }
#endif
}
