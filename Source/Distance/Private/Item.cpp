// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"
#include <cmath>

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	droppable = true;
	canUse = true;
	isInUse = false;
	amount = 100.0f;
	maxAmount = 100.0f;
	regenRate = 1.0f;
	regenAmount = 1.0f;
	name = TEXT("Default");
	OwningPawn = NULL;


	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);

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
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AItem::Pickup(ADistanceCharacter* PickerUpper)
{
	OwningPawn = PickerUpper;
	Destroy();
}

void AItem::Drop()
{
	if (droppable)
	{
		OwningPawn = NULL;
		isInUse = false;
	}
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
	tempAmount = fmax(0.0f, fmin(maxAmount, tempAmount));
	amount = tempAmount;
}

void AItem::Regenerate()
{
	ChangeAmount(regenAmount);
}

void AItem::Update(class UInventoryItem* invItem)
{
	name = invItem->GetItemName();
	amount = invItem->currentValue;
	maxAmount = invItem->maxValue;
}

FString AItem::GetItemName()
{
	return name;
}

void AItem::SetItemName(FString n)
{
	name = n;
}

UTexture2D* AItem::GetTheSprite() const
{
	return SpriteComponent->GetSprite()->GetBakedTexture();
}

ADistanceCharacter* AItem::GetOwningPawn()
{
	return OwningPawn;
}