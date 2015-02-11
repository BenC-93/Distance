// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	isInUse = false;
	amount = 100.0f;
	maxAmount = 100.0f;


	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -60.f);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite> (TEXT("/Game/Sprites/Lantern_Sprite")));
}

void AItem::Use()
{
	isInUse = !isInUse;
	if (isInUse)//may or may not need to check this if im just toggling
	{
		UE_LOG(LogTemp, Warning, TEXT("Item is: Enabled"));
		SpriteComponent->SetVisibility(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item is: Disabled"));
		SpriteComponent->SetVisibility(false);
	}
}

void AItem::OnEquip()
{
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
}

void AItem::OnUnequip()
{
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
}

void AItem::Pickup()
{
	if (SpriteComponent)
	{
		SpriteComponent->DestroyComponent();
	}
}

void AItem::Drop()
{
	// Change the input to this function to take a world location
	// Move (teleport) this object to that location
	// Enable the visual component
}
