#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PheromoneDeposit.h"
#include "DepositFactory.generated.h"

UCLASS()
class FORMICIDAE_API UDepositFactory : public UObject
{
    GENERATED_BODY()

public:
    FPheromoneDeposit Create(const FVector& Position, EPheromoneDepositType Type, EPheromoneDepositOrigin Origin) const;
};
