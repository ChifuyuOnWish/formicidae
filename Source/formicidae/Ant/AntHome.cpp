#include "AntHome.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"

AAntHome::AAntHome()
{
    PrimaryActorTick.bCanEverTick = true;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void AAntHome::Store(int32 Amount)
{
    Stored += FMath::Max(Amount, 0);
}

void AAntHome::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        const FVector Center = GetActorLocation() + FVector(0.0, 0.0, 8.0);
        DrawDebugSphere(World, Center, ReceiveRadius, 16, FColor::Green, false, -1.f, 0, 2.f);
        DrawDebugString(World, Center + FVector(0.0, 0.0, 60.0),
            FString::Printf(TEXT("Stored: %d"), Stored), nullptr, FColor::White, 0.f);
    }
#endif
}