// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceGameMode.h"
#include "DistancePlayerController.h"
#include "DistanceCharacter.h"
#include "AIBoss_Doubt.h"
#include "Shrine.h"
#include "ConvergenceCrystal.h"
#include "ConvergenceManager.h"

ADistanceGameMode::ADistanceGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// use our custom PlayerController class
	PlayerControllerClass = ADistancePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/2DSpriteCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	if (GEngine) {
		UE_LOG(LogDistance, Verbose, TEXT("Turning on on-screen debugging"));
		GEngine->bEnableOnScreenDebugMessages = true;
	}
	
	// Start on first boss
	BossIndex = 0;
	// Skip spawning the lantern from item drops
	ItemIndexDivergence1 = 1;
	ItemIndexDivergence2 = 1;
}

void ADistanceGameMode::StartPlay()
{
	Super::StartPlay();
	//ADistanceCharacter *p1 = Cast<ADistanceCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//ADistanceCharacter *p2 = Cast<ADistanceCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 1));
	//ConvergenceManager::InitializeWithPlayers(p1, p2);
}

void ADistanceGameMode::InitializeConvergence(class ADistanceCharacter* p1, class ADistanceCharacter* p2)
{
	ConvergenceManager::InitializeWithPlayers(p1, p2);
}

void ADistanceGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	printLog(TEXT("Player post login"));
	// This is probably going to be incredibly messy... Try to find another way.
	// Grab the remote player from login and the server player from gameplay statics, blech
	ADistancePlayerController *remoteController = Cast<ADistancePlayerController>(NewPlayer);
	ADistanceCharacter *p1 = Cast<ADistanceCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ADistanceCharacter *p2 = Cast<ADistanceCharacter>(remoteController->GetDistanceCharacter());
	ConvergenceManager::InitializeWithPlayers(p1, p2);
}

class ADItemPickup* ADistanceGameMode::SpawnLantern(ACharacter* player)
{
	//printScreen(FColor::Red, TEXT("Spawning The thing"));
	TSubclassOf<class ADItemPickup> ItemClass;
	ItemClass = ItemTypes[0];
	// Spawn item at player's location
	FVector Location = player->GetActorLocation();
	FRotator Rotation = player->GetActorRotation();
	return GetWorld()->SpawnActor<ADItemPickup>(ItemClass, player->GetActorLocation(), player->GetActorRotation());
}

class ADItemPickup* ADistanceGameMode::SpawnItemAtLocation(TSubclassOf<class ADItemPickup> indexClass, FVector location)
{
	//printScreen(FColor::Red, TEXT("Spawning Item"));
	UE_LOG(LogDistance, Warning, TEXT("Spawning Item"));
	return GetWorld()->SpawnActor<ADItemPickup>(indexClass, location, FRotator(0.0f, 0.0f, 0.0f));
}

class ATentacle* ADistanceGameMode::SpawnTentacleAtLocation(TSubclassOf<class ATentacle> indexClass, FVector location)//TODO: check if we need this anymore
{
	//printScreen(FColor::Red, TEXT("Spawning Tentacle"));
	UE_LOG(LogDistance, Error, TEXT("Spawning Tentacle"));
	return GetWorld()->SpawnActor<ATentacle>(indexClass, location, FRotator(0.0f, 0.0f, 0.0f));
}

void ADistanceGameMode::SpawnBossAtLocation(TSubclassOf<class AAIBoss> indexClass, FVector location)
{
	//printScreen(FColor::Red, TEXT("Spawning Boss"));
	UE_LOG(LogDistance, Warning, TEXT("Spawning Boss"));
	AAIBoss* boss = GetWorld()->SpawnActor<AAIBoss>(indexClass, location, FRotator(0.0f, 0.0f, 0.0f));
	if (boss->IsA(AAIBoss_Doubt::StaticClass()))
	{
		boss->SetActorRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		FVector bossLoc = boss->GetActorLocation();
		boss->SetActorLocation(FVector(bossLoc.X + 300.0f, bossLoc.Y, 200.0f));
	}
}

void ADistanceGameMode::SpawnBossForConvergence()
{
	TSubclassOf<AAIBoss> BossClass = BossClasses[BossIndex];
	SpawnBossAtLocation(BossClass, BossSpawnTransform.GetLocation());
}

void ADistanceGameMode::AdvanceToNextBoss()
{
	BossIndex++;
	if (BossIndex == BossClasses.Num())
	{
		// Beat the last boss
		// TODO: You won
		BossIndex = 0;
		UE_LOG(LogDistance, Error, TEXT("YOU WON"));
	}
}

void ADistanceGameMode::OnCrystalUsed(bool TriggeredConvergence)
{
	CrystalsUsed++;
	if (!TriggeredConvergence && CrystalsUsed >= TotalAvailableCrystals)
	{
		// Did not trigger convergence, but used up the last crystal
		// If bosses haven't been defeated players are in permanent divergence
		for (TActorIterator<AShrine> ShrineItr(GetWorld()); ShrineItr; ++ShrineItr)
		{
			ShrineItr->TriggerDecay();
		}
	}
	// Else check when convergence ends
}

void ADistanceGameMode::OnEndConvergence(bool DefeatedBoss)
{
	bool ConvergenceConditionAchieved = false;
	if (DefeatedBoss)
	{
		int32 NextBoss = BossIndex + 1;
		if (NextBoss >= BossClasses.Num())
		{
			// Players have successfully defeated all bosses, don't end convergence
			ConvergenceConditionAchieved = true;
		}
		AdvanceToNextBoss();
	}
	
	if (!ConvergenceConditionAchieved)
	{
		// Return players to divergence
		for (TActorIterator<AConvergenceCrystal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->Destroy();
		}
		ConvergenceManager::EndConvergence();
		
		// Check if all crystals are used up
		if (CrystalsUsed >= TotalAvailableCrystals)
		{
			// If bosses haven't been defeated players are in permanent divergence
			for (TActorIterator<AShrine> ShrineItr(GetWorld()); ShrineItr; ++ShrineItr)
			{
				ShrineItr->TriggerDecay();
			}
		}
	}
}

void ADistanceGameMode::SpawnRandomItemAtLocation(FVector location)
{
	ConvergenceState ItemSpawnState = ConvergenceManager::StateFromLocation(location);
	TSubclassOf<class ADItemPickup> ItemClass;
	uint32 ItemIndex;
	uint32 LastIndexToSpawn = ItemTypes.Num() - 1;
	// Spawn random item in convergence (unused), but go in order according to each divergence state
	switch (ItemSpawnState) {
		case ConvergenceState::CONVERGENCE:
			UE_LOG(LogDistance, Warning, TEXT("Spawning Random Item"));
			ItemIndex = FMath::RandRange(1, LastIndexToSpawn);
			break;
		case ConvergenceState::DIVERGENCE1:
			ItemIndex = ItemIndexDivergence1;
			ItemIndexDivergence1++;
			break;
		case ConvergenceState::DIVERGENCE2:
			ItemIndex = ItemIndexDivergence2;
			ItemIndexDivergence2++;
			break;
	}
	if (ItemIndex <= LastIndexToSpawn)
	{
		ItemClass = ItemFromIndex(ItemIndex);
		GetWorld()->SpawnActor<ADItemPickup>(ItemClass, location, FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		// One of the divergence states has spawned all valid items
		UE_LOG(LogDistance, Warning, TEXT("Out of items to spawn for state: %d"), ItemSpawnState);
	}
}

TSubclassOf<class ADItemPickup> ADistanceGameMode::ItemFromIndex(uint32 ItemIndex)
{
	if (ItemTypes.IsValidIndex(ItemIndex))
	{
		return ItemTypes[ItemIndex];
	}
	else 
	{
		UE_LOG(LogDistance, Error, TEXT("Invalid item index in GameMode: %d"), ItemIndex);
		return NULL;
	}
}