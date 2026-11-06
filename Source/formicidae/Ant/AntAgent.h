#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodSource.h"
#include "AntHome.h"
#include "AntAgent.generated.h"

class UPheromoneManager;

enum class EAntState : uint8
{
    Exploring,
    Returning
};

UCLASS()
class FORMICIDAE_API AAntAgent : public AActor
{
    GENERATED_BODY()

public:
    AAntAgent();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void SetHeading(const FVector& NewHeading);

    UPROPERTY(EditAnywhere, Category = "Ant|Movement")
    float MoveSpeed = 150.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Movement")
    float MaxTurnRateDeg = 180.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Sensing")
    float SenseRadius = 60.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Sensing")
    float ConeHalfAngleDeg = 70.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Sensing")
    float DeadZoneRadius = 15.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Sensing")
    float DecisionInterval = 0.2f;

    UPROPERTY(EditAnywhere, Category = "Ant|Exploration")
    float ExplorationJitterDeg = 25.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Deposit")
    float DepositDistance = 40.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Deposit")
    bool bDepositWhileExploring = false;

    UPROPERTY(EditAnywhere, Category = "Ant|Food")
    float FoodSenseRadius = 250.f;

    UPROPERTY(EditAnywhere, Category = "Ant|Carry")
    int32 CarryCapacity = 1;

private:
    void UpdateState();
    FVector ChooseDirection();
    FVector ComputeDesiredDirection() const;
    void UpdateHeading(float DeltaTime);
    float MoveForward(float DeltaTime);
    void LayPheromone(float DistanceMoved);
    void FindFood();
    void FindHome();

    UPROPERTY()
    TObjectPtr<UPheromoneManager> PheromoneManager;

    TWeakObjectPtr<AFoodSource> Food;
    TWeakObjectPtr<AAntHome> Home;
    EAntState State = EAntState::Exploring;

    FVector Heading = FVector::ForwardVector;
    FVector DesiredHeading = FVector::ForwardVector;
    float DecisionTimer = 0.f;
    float DistanceSinceDeposit = 0.f;
    int32 Carried = 0;
    int32 TripCount = 0;
};