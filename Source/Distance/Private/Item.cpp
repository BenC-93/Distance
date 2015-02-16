// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"
#include <cmath>

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	droppable = true;
	isInUse = false;
	amount = 100.0f;
	maxAmount = 100.0f;
	regenRate = 1.0f;
	regenAmount = 1.0f;
	name = "Default";


	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -60.f);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite> (TEXT("/Game/Sprites/Lantern_Sprite")));

	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("TriggerBox"));
	TriggerBox->Mobility = EComponentMobility::Movable;
	TriggerBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f), true);
	TriggerBox->AttachTo(RootComponent);
}

void AItem::StartUse()
{
	isInUse = true; //this should be the first thing that happens
}

void AItem::EndUse()
{
	isInUse = false; //this should be the last thing that happens
}

void AItem::OnEquip()
{
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().SetTimer(this, &AItem::Regenerate, regenRate, true);
}

void AItem::OnUnequip()
{
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().ClearTimer(this, &AItem::Regenerate);
}

void AItem::Pickup()
{
	/*if (SpriteComponent)
	{
		SpriteComponent->DestroyComponent();
	}*/
	Destroy();
}

void AItem::Drop()
{
	// Change the input to this function to take a world location
	// Move (teleport) this object to that location
	// Enable the visual component
	isInUse = false;
}


float AItem::GetAmount()
{
	return amount;
}

float AItem::GetMaxAmount()
{
	return maxAmount;
}

void AItem::ChangeAmount(float value)
{
	float tempAmount = GetAmount() + value;
	tempAmount = fmax(0.0f, fmin(100.0f, tempAmount));
	amount = tempAmount;
}

void AItem::Regenerate()
{
	ChangeAmount(regenAmount);
}

void AItem::Update(class InventoryItem* invItem)
{
	name = invItem->name;
	amount = invItem->currentValue;
	maxAmount = invItem->maxValue;
}