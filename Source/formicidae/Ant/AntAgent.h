#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntAgent.generated.h"

class UPheromoneManager;

UCLASS()
class FORMICIDAE_API AAntAgent : public AActor
{
    GENERATED_BODY()

public:
    AAntAgent();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

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

private:
    FVector ComputeDesiredDirection() const;
    void UpdateHeading(float DeltaTime);
    float MoveForward(float DeltaTime);
    void LayPheromone(float DistanceMoved);

    UPROPERTY()
    TObjectPtr<UPheromoneManager> PheromoneManager;
    FVector Heading = FVector::ForwardVector;
    FVector DesiredHeading = FVector::ForwardVector;
    float DecisionTimer = 0.f;
    float DistanceSinceDeposit = 0.f;
};
