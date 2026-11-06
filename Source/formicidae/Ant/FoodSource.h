#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodSource.generated.h"

UCLASS()
class FORMICIDAE_API AFoodSource : public AActor
{
    GENERATED_BODY()

public:
    AFoodSource();
    virtual void Tick(float DeltaTime) override;

    int32 TryTake(int32 Requested);

    int32 GetAmount() const { return Amount; }

    UPROPERTY(EditAnywhere, Category = "Food")
    float PickupRadius = 40.f;

    UPROPERTY(EditAnywhere, Category = "Food")
    int32 MaxAmount = 20;

    UPROPERTY(EditAnywhere, Category = "Food")
    float RegenPerSecond = 0.5f;

protected:
    virtual void BeginPlay() override;

private:
    int32 Amount = 0;
    float RegenAccumulator = 0.f;
};
