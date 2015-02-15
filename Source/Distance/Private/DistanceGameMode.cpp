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
	if (ItemList) {
		UE_LOG(LogDistance, Verbose, TEXT("Creating Item List"));
		ActualItemList = ConstructObject<UItemDataList>(ItemList);
	}
}

void ADistanceGameMode::SpawnItem()
{
	printScreen(FColor::Red, "Spawning The thing");
	TSubclassOf<class AItem> ItemClass;
	// Data wont cast, it's just the class?
	//UItemDataList* data = Cast<UItemDataList>(ItemList);
	ItemClass = ActualItemList->ItemAtIndex(0);
	// Spawn item at player's location
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GetWorld()->SpawnActor<AItem>(ItemClass, FVector(player->GetActorLocation()), FRotator(player->GetActorRotation()));
}