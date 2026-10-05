#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PheromoneSettings.generated.h"

USTRUCT()
struct FDepositDefaults
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Pheromone")
    float Strength = 0.f;

    UPROPERTY(EditAnywhere, Category = "Pheromone")
    float DecayRate = 0.f;

    UPROPERTY(EditAnywhere, Category = "Pheromone")
    float MaxStrength = 0.f;
};

UCLASS(config = Game, defaultconfig)
class FORMICIDAE_API UPheromoneSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPheromoneSettings();

    UPROPERTY(config, EditAnywhere, Category = "Anchors")
    float MinAnchorStrength = 15.f;

    UPROPERTY(config, EditAnywhere, Category = "Radius")
    float MinDepositRadius = 10.f;

    UPROPERTY(config, EditAnywhere, Category = "Radius")
    float MaxDepositRadius = 50.f;
    UPROPERTY(config, EditAnywhere, Category = "Deposit Defaults")
    FDepositDefaults Forage;
    UPROPERTY(config, EditAnywhere, Category = "Deposit Defaults")
    FDepositDefaults Redirection;
    UPROPERTY(config, EditAnywhere, Category = "Deposit Defaults")
    FDepositDefaults Alarm;
};