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

    UPROPERTY(EditAnywhere, Category = "Home")
    int32 AntReserve = 0;

    UPROPERTY(EditAnywhere, Category = "Home")
    float SpawnInterval = 1.f;

private:
    void SpawnAnt();

    int32 Stored = 0;
    float SpawnTimer = 0.f;
};