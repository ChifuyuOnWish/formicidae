#include "PheromoneSettings.h"

UPheromoneSettings::UPheromoneSettings()
{
    CategoryName = TEXT("Game");
    SectionName = TEXT("Pheromone");

    Forage      = FDepositDefaults{ 50.f, 1.f, 100.f };
    Redirection = FDepositDefaults{ 50.f, 2.f, 100.f };
    Alarm       = FDepositDefaults{ 75.f, 1.f, 100.f };
}