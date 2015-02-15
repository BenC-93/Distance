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

void ADistanceGameMode::SpawnItem()
{
	TSubclassOf<class AItem> ItemClass;
	UItemDataList* data = Cast<UItemDataList>(ItemList);
	ItemClass = data->ItemAtIndex(0);
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GetWorld()->SpawnActor<ItemClass>(FVector(player->GetActorLocation()), FRotator(player->GetActorRotation()));
}