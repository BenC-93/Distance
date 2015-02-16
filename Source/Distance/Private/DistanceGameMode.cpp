// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceGameMode.h"
#include "DistancePlayerController.h"
#include "DistanceCharacter.h"

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
}

class AItem* ADistanceGameMode::SpawnItem()
{
	printScreen(FColor::Red, "Spawning The thing");
	TSubclassOf<class AItem> ItemClass;
	ItemClass = ItemTypes[0];
	// Spawn item at player's location
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	return Cast<AItem>(GetWorld()->SpawnActor<AItem>(ItemClass, FVector(player->GetActorLocation()), FRotator(player->GetActorRotation())));
}

void ADistanceGameMode::SpawnRandomItemAtLocation(FVector location)
{
	printScreen(FColor::Red, "Spawning Random Item");
	TSubclassOf<class AItem> ItemClass;
	uint32 ItemIndex = FMath::RandRange(1, ItemTypes.Num()-1);
	ItemClass = ItemFromIndex(ItemIndex);
	GetWorld()->SpawnActor<AItem>(ItemClass, location, FRotator(0.0f, 0.0f, 0.0f));
}

class TSubclassOf<class AItem> ADistanceGameMode::ItemFromIndex(uint32 ItemIndex)
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