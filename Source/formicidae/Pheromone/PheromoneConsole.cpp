#include "CoreMinimal.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "Misc/DefaultValueHelper.h"
#include "PheromoneManager.h"
#include "PheromoneSettings.h"
#include "Ant/AntAgent.h"
// #include "Ant/FoodSource.h"

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

static void AntSpawn(const TArray<FString>&, UWorld* World)
{
    if (!World)
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("No world available for this command."));
        return;
    }

    FVector SpawnLocation = FVector::ZeroVector;

    const APlayerController* PlayerController = World->GetFirstPlayerController();
    const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
    if (Pawn)
    {
        FVector Forward = PlayerController->GetControlRotation().Vector();
        Forward.Z = 0.0;
        Forward = Forward.GetSafeNormal();
        if (Forward.IsNearlyZero())
        {
            Forward = FVector::ForwardVector;
        }

        SpawnLocation = Pawn->GetActorLocation() + Forward * 400.0;
        SpawnLocation.Z = 0.0;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AAntAgent* NewAnt = World->SpawnActor<AAntAgent>(AAntAgent::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (NewAnt)
    {
        UE_LOG(LogPheromoneDebug, Log, TEXT("Spawned ant agent at %s."), *SpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("Failed to spawn ant agent."));
    }
}

static void AntClear(const TArray<FString>&, UWorld* World)
{
    if (!World)
    {
        UE_LOG(LogPheromoneDebug, Warning, TEXT("No world available for this command."));
        return;
    }

    int32 ClearedCount = 0;

    for (TActorIterator<AAntAgent> It(World); It; ++It)
    {
        AAntAgent* Ant = *It;
        if (Ant)
        {
            Ant->Destroy();
            ++ClearedCount;
        }
    }

    UE_LOG(LogPheromoneDebug, Log, TEXT("Cleared %d ant agents."), ClearedCount);
}

struct FTestCounters
{
    int32 Passed = 0;
    int32 Failed = 0;
};

static void Expect(FTestCounters& Counters, const TCHAR* Description, bool bCondition)
{
    if (bCondition)
    {
        ++Counters.Passed;
        UE_LOG(LogPheromoneDebug, Log, TEXT("PASS: %s"), Description);
    }
    else
    {
        ++Counters.Failed;
        UE_LOG(LogPheromoneDebug, Error, TEXT("FAIL: %s"), Description);
    }
}

static TArray<FPheromoneDeposit> Snapshot(const UPheromoneManager& Manager)
{
    TArray<FPheromoneDeposit> Out;
    Manager.ForEach([&Out](const FPheromoneDeposit& Entry) { Out.Add(Entry); });
    return Out;
}

static void RunTests(const TArray<FString>&, UWorld* World)
{
    UPheromoneManager* Manager = GetManager(World);
    if (!Manager)
    {
        return;
    }

    TActorIterator<AAntAgent> AntIt(World);
    if (AntIt)
    {
        UE_LOG(LogPheromoneDebug, Error, TEXT("Ants are alive. Run ant.clear first."));
        return;
    }

    const UPheromoneSettings* Settings = GetDefault<UPheromoneSettings>();
    const FDepositDefaults& Forage = Settings->Forage;
    const FDepositDefaults& Redirection = Settings->Redirection;

    if (Forage.MaxStrength <= 0.f || Forage.Strength <= 0.f)
    {
        UE_LOG(LogPheromoneDebug, Error, TEXT("Forage defaults must be above zero to run the tests."));
        return;
    }

    const double R = Settings->MinDepositRadius
        + (Settings->MaxDepositRadius - Settings->MinDepositRadius) * (Forage.Strength / Forage.MaxStrength);
    const double Far = 4.0 * Settings->MaxDepositRadius;

    FTestCounters C;
    UE_LOG(LogPheromoneDebug, Log, TEXT("Running pheromone tests (this clears all deposits)."));

    Manager->Clear();
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("1 create: one deposit"), E.Num() == 1);
        if (E.Num() == 1)
        {
            Expect(C, TEXT("1 create: strength is the forage default"),
                FMath::IsNearlyEqual(E[0].Strength, Forage.Strength, 0.01f));
            Expect(C, TEXT("1 create: type is FORAGE"), E[0].Type == EPheromoneDepositType::FORAGE);
            Expect(C, TEXT("1 create: origin is ANT"), E[0].Origin == EPheromoneDepositOrigin::ANT);
        }
    }

    const double MergeX = 0.5 * R;
    Manager->Deposit(FVector(MergeX, 0.0, 0.0), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("2 merge: still one deposit"), E.Num() == 1);
        if (E.Num() == 1)
        {
            Expect(C, TEXT("2 merge: strengths added, capped at max"),
                FMath::IsNearlyEqual(E[0].Strength, FMath::Min(2.f * Forage.Strength, Forage.MaxStrength), 0.01f));
            Expect(C, TEXT("2 merge: position drifted halfway"),
                FMath::IsNearlyEqual(E[0].Position.X, MergeX * 0.5, 0.01));
        }
    }

    const FVector MergedPos(MergeX * 0.5, 0.0, 0.0);
    const int32 Repeats = FMath::CeilToInt(Forage.MaxStrength / Forage.Strength) + 2;
    for (int32 i = 0; i < Repeats; ++i)
    {
        Manager->Deposit(MergedPos, EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    }
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("3 cap: strength stops at max"),
            E.Num() == 1 && FMath::IsNearlyEqual(E[0].Strength, Forage.MaxStrength, 0.01f));
    }

    Manager->Deposit(FVector(Far, 0.0, 0.0), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    Expect(C, TEXT("4 far: two deposits"), Manager->Count() == 2);

    Manager->Tick(10.f);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("5 decay: strength dropped by decay * 10"),
            E.Num() >= 1 && FMath::IsNearlyEqual(E[0].Strength, Forage.MaxStrength - Forage.DecayRate * 10.f, 0.01f));
    }

    Manager->Clear();
    const FVector OriginPos(0.0, 500.0, 0.0);
    Manager->Deposit(OriginPos, EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::PLAYER);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("6 origin: starts as PLAYER"), E.Num() == 1 && E[0].Origin == EPheromoneDepositOrigin::PLAYER);
    }
    Manager->Deposit(OriginPos + FVector(10.0, 0.0, 0.0), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("6 origin: becomes ANT after an ANT merge"), E.Num() == 1 && E[0].Origin == EPheromoneDepositOrigin::ANT);
    }
    Manager->Deposit(OriginPos + FVector(5.0, 0.0, 0.0), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::PLAYER);
    {
        const TArray<FPheromoneDeposit> E = Snapshot(*Manager);
        Expect(C, TEXT("6 origin: stays ANT after a PLAYER merge"), E.Num() == 1 && E[0].Origin == EPheromoneDepositOrigin::ANT);
    }

    Manager->Clear();
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::REDIRECTION, EPheromoneDepositOrigin::PLAYER);
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::ALARM, EPheromoneDepositOrigin::ANT);
    Expect(C, TEXT("7 types: three separate deposits"), Manager->Count() == 3);

    Manager->Clear();
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::REDIRECTION, EPheromoneDepositOrigin::PLAYER);
    Manager->Tick(Redirection.Strength / FMath::Max(Redirection.DecayRate, 0.01f) + 1.f);
    Expect(C, TEXT("8 removal: dead deposit removed"), Manager->Count() == 0);

    Manager->Clear();
    Manager->Deposit(FVector::ZeroVector, EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    Manager->Deposit(FVector(Far, 0.0, 0.0), EPheromoneDepositType::FORAGE, EPheromoneDepositOrigin::ANT);
    const int32 CountBefore = Manager->Count();
    const TArray<FVector> GoodPath = { FVector(0.0, 0.0, 0.0), FVector(Far, 0.0, 0.0) };
    Expect(C, TEXT("9 redirection: placed"), Manager->TryPlaceRedirection(GoodPath));
    Expect(C, TEXT("9 redirection: chain of deposits created"), Manager->Count() > CountBefore);

    const int32 CountAfterGood = Manager->Count();
    const TArray<FVector> BadPath = { FVector(0.0, 0.0, 0.0), FVector(Far, 500.0, 0.0) };
    Expect(C, TEXT("10 redirection: refused"), !Manager->TryPlaceRedirection(BadPath));
    Expect(C, TEXT("10 redirection: nothing created"), Manager->Count() == CountAfterGood);

    Manager->Clear();
    UE_LOG(LogPheromoneDebug, Log, TEXT("Pheromone tests: %d passed, %d failed"), C.Passed, C.Failed);
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

static FAutoConsoleCommandWithWorldAndArgs AntSpawnCommand(
    TEXT("ant.spawn"),
    TEXT("Spawns a test ant agent at the player's location."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&AntSpawn));

static FAutoConsoleCommandWithWorldAndArgs AntClearCommand(
    TEXT("ant.clear"),
    TEXT("Clears all ant agents."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&AntClear));

static FAutoConsoleCommandWithWorldAndArgs TestCommand(
    TEXT("pheromone.test"),
    TEXT("Runs the pheromone scenarios and prints PASS or FAIL. Clears all deposits."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&RunTests));

#endif
