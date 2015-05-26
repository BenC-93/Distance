// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DItemPickup.h"
#include "DistanceCharacter.h"
#include "DItem.h"

ADItemPickup::ADItemPickup(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ItemAmount = 100.0f;
	ItemName = TEXT("Default");
	
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);
	
	bReplicates = true;
	bReplicateMovement = true;
}

// This is going to be doing some spawning and destruction, the method should only be called from authority
void ADItemPickup::OnUsed(APawn *InstigatorPawn)
{
	ensure(Role == ROLE_Authority);
	ADistanceCharacter* MyCharacter = Cast<ADistanceCharacter>(InstigatorPawn);
	if (MyCharacter)
	{
		// Call some code on the character to pick it up
		if (MyCharacter->HasInventorySpace())
		{
			// Create the actual item for the inventory
			ADItem* NewItem = GetWorld()->SpawnActor<ADItem>(ItemClass);
			MyCharacter->AddItem(NewItem);
			// TODO: Items that you drop need to save their value in the pickup class, make sure this gets transferred back
			
			Destroy();
		}
		else
		{
			// TODO: Potentially add actual UI feedback to this thing
			UE_LOG(LogDistance, Warning, TEXT("Inventory full when picking up item"));
		}
	}
}


