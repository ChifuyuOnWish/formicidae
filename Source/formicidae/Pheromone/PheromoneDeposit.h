#pragma once

#include "CoreMinimal.h"
#include "PheromoneDeposit.generated.h"

UENUM(BlueprintType)
enum class EPheromoneDepositOrigin : uint8
{
    ANT,
    PLAYER
};

UENUM(BlueprintType)
enum class EPheromoneDepositType : uint8
{
    FORAGE,
    REDIRECTION,
    ALARM
};

USTRUCT(BlueprintType)
struct FPheromoneDeposit
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float Strength = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float DecayRate = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float MaxStrength = 0.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    EPheromoneDepositOrigin Origin = EPheromoneDepositOrigin::ANT;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    EPheromoneDepositType Type = EPheromoneDepositType::FORAGE;
};
