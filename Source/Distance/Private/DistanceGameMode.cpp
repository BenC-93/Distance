// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceGameMode.h"
#include "DistancePlayerController.h"
#include "DistanceCharacter.h"
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
	GetWorld()->SpawnActor<AAIBoss>(indexClass, location, FRotator(0.0f, 0.0f, 0.0f));
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

void ADistanceGameMode::SpawnRandomItemAtLocation(FVector location)
{
	//printScreen(FColor::Red, TEXT("Spawning Random Item"));
	UE_LOG(LogDistance, Warning, TEXT("Spawning Random Item"));
	TSubclassOf<class ADItemPickup> ItemClass;
	uint32 ItemIndex = FMath::RandRange(1, ItemTypes.Num()-1);
	ItemClass = ItemFromIndex(ItemIndex);
	GetWorld()->SpawnActor<ADItemPickup>(ItemClass, location, FRotator(0.0f, 0.0f, 0.0f));
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