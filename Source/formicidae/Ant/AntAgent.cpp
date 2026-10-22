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

void AAntAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PheromoneManager)
        return;

    LayPheromone(MoveForward(DeltaTime));

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        DrawDebugSphere(World, GetActorLocation() + FVector(0.0, 0.0, 8.0), 8.f, 12, FColor::Yellow, false, -1.f, 0, 1.5f);
    }
#endif
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