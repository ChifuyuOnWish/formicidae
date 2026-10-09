#include "CoreMinimal.h"
#include "Engine/World.h"
#include "PheromoneManager.h"
#include "Misc/DefaultValueHelper.h"

#if !UE_BUILD_SHIPPING

DEFINE_LOG_CATEGORY_STATIC(LogPheromoneDebug, Log, All);

static UPheromoneManager* GetManager(UWorld* World)
{
    if (!World)
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("No world available for this command."));
        return nullptr;
    }

    UPheromoneManager* Manager = World->GetSubsystem<UPheromoneManager>();

    if (!Manager)
    {
        UE_LOG(LogPheromoneDebug, Warning,
            TEXT("No PheromoneManager in world '%s'. Is the game running?"),
            *World->GetName());
    }

    return Manager;
}

static bool ParsePheromoneType(const FString& TypeString, EPheromoneDepositType& OutType)
{
    if (TypeString.Equals(TEXT("forage"), ESearchCase::IgnoreCase))
    {
        OutType = EPheromoneDepositType::FORAGE;
    }
    else if (TypeString.Equals(TEXT("redirection"), ESearchCase::IgnoreCase))
    {
        OutType = EPheromoneDepositType::REDIRECTION;
    }
    else if (TypeString.Equals(TEXT("alarm"), ESearchCase::IgnoreCase))
    {
        OutType = EPheromoneDepositType::ALARM;
    }
    else
    {
        return false;
    }
    return true;
}

static bool ParsePheromoneOrigin(const FString& OriginString, EPheromoneDepositOrigin& OutOrigin)
{
    if (OriginString.Equals(TEXT("ant"), ESearchCase::IgnoreCase))
    {
        OutOrigin = EPheromoneDepositOrigin::ANT;
    }
    else if (OriginString.Equals(TEXT("player"), ESearchCase::IgnoreCase))
    {
        OutOrigin = EPheromoneDepositOrigin::PLAYER;
    }
    else
    {
        return false;
    }
    return true;
}

static void DumpDeposits(const TArray<FString>&, UWorld* World)
{
    UPheromoneManager* Manager = GetManager(World);
    if (!Manager)
    {
        return;
    }

    UE_LOG(LogPheromoneDebug, Log, TEXT("Deposits alive: %d"), Manager->Count());

    int32 Index = 0;

    Manager->ForEach([&Index](const FPheromoneDeposit& Entry)
    {
        UE_LOG(LogPheromoneDebug, Log,
            TEXT("[%d] Type=%s Origin=%s Position=%s Strength=%.2f Decay=%.2f"),
            Index,
            *UEnum::GetValueAsString(Entry.Type),
            *UEnum::GetValueAsString(Entry.Origin),
            *Entry.Position.ToString(),
            Entry.Strength,
            Entry.DecayRate);
        ++Index;
    });
}

static void DepositPheromone(const TArray<FString>& Args, UWorld* World)
{
    if (Args.Num() != 5)
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("Usage: pheromone.deposit <type> <origin> <x> <y> <z>"));
        return;
    }

    UPheromoneManager* Manager = GetManager(World);
    if (!Manager)
    {
        return;
    }

    EPheromoneDepositType Type = EPheromoneDepositType::FORAGE;
    if (!ParsePheromoneType(Args[0], Type))
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("Invalid pheromone type '%s'"), *Args[0]);
        return;
    }

    EPheromoneDepositOrigin Origin = EPheromoneDepositOrigin::ANT;
    if (!ParsePheromoneOrigin(Args[1], Origin))
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("Invalid pheromone origin '%s'"), *Args[1]);
        return;
    }

    FVector Position = FVector::ZeroVector;
    for (int32 Axis = 0; Axis < 3; ++Axis)
    {
        const FString& Text = Args[Axis + 2];
        float Value = 0.f;
        if (!FDefaultValueHelper::ParseFloat(Text, Value))
        {
            UE_LOG(LogPheromoneDebug, Warning, TEXT("Invalid position component '%s'"), *Text);
            return;
        }
        Position[Axis] = Value;
    }

    Manager->Deposit(Position, Type, Origin);
    UE_LOG(LogPheromoneDebug, Log, TEXT("Deposited pheromone at %s with type %s and origin %s."), *Position.ToString(), *UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Origin));
}

static void ClearDeposits(const TArray<FString>&, UWorld* World)
{
    UPheromoneManager* Manager = GetManager(World);
    if (!Manager)
    {
        return;
    }

    Manager->Clear();
    UE_LOG(LogPheromoneDebug, Log, TEXT("Cleared all pheromone deposits."));
}

static FAutoConsoleCommandWithWorldAndArgs DumpCommand(
    TEXT("pheromone.dump"),
    TEXT("Logs every live pheromone deposit."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&DumpDeposits));

static FAutoConsoleCommandWithWorldAndArgs DepositCommand(
    TEXT("pheromone.deposit"),
    TEXT("Deposits a pheromone at the specified position with the given type and origin."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&DepositPheromone));

static FAutoConsoleCommandWithWorldAndArgs ClearCommand(
    TEXT("pheromone.clear"),
    TEXT("Clears all pheromone deposits."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&ClearDeposits));

#endif
