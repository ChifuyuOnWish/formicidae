#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AntHome.generated.h"

UCLASS()
class FORMICIDAE_API AAntHome : public AActor
{
    GENERATED_BODY()

public:
    AAntHome();
    virtual void Tick(float DeltaTime) override;

    void Store(int32 Amount);
    int32 GetStored() const { return Stored; }

    UPROPERTY(EditAnywhere, Category = "Home")
    float ReceiveRadius = 60.f;

private:
    int32 Stored = 0;
};