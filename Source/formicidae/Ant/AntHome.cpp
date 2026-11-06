#include "AntHome.h"
#include "AntAgent.h"
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

void AAntHome::SpawnAnt()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AAntAgent* Ant = World->SpawnActor<AAntAgent>(AAntAgent::StaticClass(),
        GetActorLocation(), FRotator::ZeroRotator, Params);
    if (Ant)
    {
        --AntReserve;
    }
}

void AAntHome::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (AntReserve > 0)
    {
        SpawnTimer += DeltaTime;
        if (SpawnTimer >= FMath::Max(SpawnInterval, 0.01f))
        {
            SpawnTimer = 0.f;
            SpawnAnt();
        }
    }

#if !UE_BUILD_SHIPPING
    if (const UWorld* World = GetWorld())
    {
        const FVector Center = GetActorLocation() + FVector(0.0, 0.0, 8.0);
        DrawDebugSphere(World, Center, ReceiveRadius, 16, FColor::Green, false, -1.f, 0, 2.f);
        DrawDebugString(World, Center + FVector(0.0, 0.0, 60.0),
            FString::Printf(TEXT("Stored: %d  Ants left: %d"), Stored, AntReserve),
            nullptr, FColor::White, 0.f);
    }
#endif
}