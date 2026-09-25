#pragma once

#include "CoreMinimal.h"

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
    FVector Position;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float Strength;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float DecayRate;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    float MaxStrength;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    EPheromoneDepositOrigin Origin;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pheromone Deposit")
    EPheromoneDepositType Type;

    FPheromoneDeposit()
        : Position(FVector::ZeroVector)
        , Strength(0.0f)
        , DecayRate(0.0f)
        , MaxStrength(0.0f)
        , Origin(EPheromoneDepositOrigin::ANT)
        , Type(EPheromoneDepositType::FORAGE)
    {
    }

    FPheromoneDeposit(FVector InPosition, float InStrength, float InDecayRate, float InMaxStrength, EPheromoneDepositOrigin InOrigin, EPheromoneDepositType InType)
        : Position(InPosition)
        , Strength(InStrength)
        , DecayRate(InDecayRate)
        , MaxStrength(InMaxStrength)
        , Origin(InOrigin)
        , Type(InType)
    {
    }
};
