// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsEnabled = false;
	amount = 100.0f;
	maxAmount = 100.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -60.f);

	
}

void AItem::Use()
{

}

void AItem::OnEquip()
{

}

void AItem::OnUnequip()
{

}