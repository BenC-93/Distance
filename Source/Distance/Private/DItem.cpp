// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DItem.h"
#include "DistanceCharacter.h"

ADItem::ADItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsDroppable = true;
	bCanUse = true;
	bIsInUse = false;
	ItemAmount = 100.0f;
	MaxItemAmount = 100.0f;
	RegenRate = 1.0f;
	RegenAmount = 1.0f;
	ItemName = TEXT("Default");
	OwningPawn = NULL;
	
	
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
}

FString ADItem::GetItemName() const
{
	return ItemName;
}

UTexture2D* ADItem::GetItemSprite() const
{
	return SpriteComponent->GetSprite()->GetBakedTexture();
}

void ADItem::SetOwningPawn(ADistanceCharacter* NewOwner)
{
	if (OwningPawn != NewOwner)
	{
		Instigator = NewOwner;
		OwningPawn = NewOwner;
		// Net owner for RPC calls.
		SetOwner(NewOwner);
	}
}

void ADItem::OnEnterInventory(ADistanceCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}
