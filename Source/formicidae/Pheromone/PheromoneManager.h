#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PheromoneDeposit.h"
#include "DepositFactory.h"
#include "PheromoneManager.generated.h"

class UPheromoneSettings;

UCLASS()
class FORMICIDAE_API UPheromoneManager : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    void Deposit(const FVector& Position, EPheromoneDepositType Type, EPheromoneDepositOrigin Origin);
    void ForEachNearby(const FVector& Position, float Radius, EPheromoneDepositType Type,
                       TFunctionRef<void(const FPheromoneDeposit&)> Visitor) const;
    bool IsValidRedirectionAnchor(const FVector& Position) const;
    bool TryPlaceRedirection(const TArray<FVector>& Path);

    void ForEach(TFunctionRef<void(const FPheromoneDeposit&)> Visitor) const;
    int32 Count() const;

#if !UE_BUILD_SHIPPING
    void Clear();
#endif

private:
    UPROPERTY()
    TObjectPtr<UDepositFactory> DepositFactory;
    TArray<FPheromoneDeposit> Deposits;
    const UPheromoneSettings* Settings = nullptr;
    bool TryMergeIntoExisting(const FPheromoneDeposit& NewDeposit);
    float GetRadius(const FPheromoneDeposit& Entry) const;

#if !UE_BUILD_SHIPPING
    void DrawDebug() const;
#endif
};
