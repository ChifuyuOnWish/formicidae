#include "FoodSource.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"

AFoodSource::AFoodSource()
{
    PrimaryActorTick.bCanEverTick = true;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void AFoodSource::BeginPlay()
{
    Super::BeginPlay();
    Amount = MaxAmount;
}

int32 AFoodSource::TryTake(int32 Requested)
{
    const int32 Taken = FMath::Clamp(Requested, 0, Amount);
    Amount -= Taken;
    return Taken;
}

void AFoodSource::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Amount < MaxAmount)
    {
        RegenAccumulator += RegenPerSecond * DeltaTime;
        const int32 Whole = FMath::FloorToInt(RegenAccumulator);
        if (Whole > 0)
        {
            Amount = FMath::Min(Amount + Whole, MaxAmount);
            RegenAccumulator -= Whole;
        }
    }
    else
    {
        RegenAccumulator = 0.f;
    }

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        const FVector Center = GetActorLocation() + FVector(0.0, 0.0, 8.0);
        DrawDebugSphere(World, Center, PickupRadius, 16, FColor::Red, false, -1.f, 0, 2.f);
        DrawDebugString(World, Center + FVector(0.0, 0.0, 60.0),
            FString::Printf(TEXT("%d / %d"), Amount, MaxAmount), nullptr, FColor::White, 0.f);
    }
#endif
}