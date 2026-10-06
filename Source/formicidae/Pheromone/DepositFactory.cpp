#include "DepositFactory.h"
#include "PheromoneSettings.h"

FPheromoneDeposit UDepositFactory::Create(const FVector& Position, EPheromoneDepositType Type, EPheromoneDepositOrigin Origin) const
{
    const UPheromoneSettings* Settings = GetDefault<UPheromoneSettings>();
    const FDepositDefaults* Defaults = nullptr;
    switch (Type)
    {
    case EPheromoneDepositType::FORAGE:      Defaults = &Settings->Forage;      break;
    case EPheromoneDepositType::REDIRECTION: Defaults = &Settings->Redirection; break;
    case EPheromoneDepositType::ALARM:       Defaults = &Settings->Alarm;       break;
    }

    if (!ensureMsgf(Defaults, TEXT("UDepositFactory::Create: no defaults for this deposit type")))
    {
        return FPheromoneDeposit();
    }

    FPheromoneDeposit Result;
    Result.Position = Position;
    Result.Strength = Defaults->Strength;
    Result.DecayRate = Defaults->DecayRate;
    Result.MaxStrength = Defaults->MaxStrength;
    Result.Origin = Origin;
    Result.Type = Type;
    return Result;
}
