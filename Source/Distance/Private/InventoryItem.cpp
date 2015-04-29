// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "InventoryItem.h"
#include "UnrealNetwork.h"

AInventoryItem::AInventoryItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	// This section intentionally left blank?
	name = TEXT("Default");
}

void AInventoryItem::CreateFromItem(AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetItemName();
	maxValue = item->maxAmount;
	currentValue = item->amount;
	sprite = item->GetTheSprite();
}

UTexture2D* AInventoryItem::GetItemSprite()
{
	return sprite;
}

FString AInventoryItem::GetItemName()
{
	return name;
}

void AInventoryItem::SetItemName(FString n)
{
	name = n;
}

void AInventoryItem::Update(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetItemName();
	maxValue = item->maxAmount;
	currentValue = item->amount;
}

void AInventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AInventoryItem, ItemClass);
	DOREPLIFETIME(AInventoryItem, name);
	DOREPLIFETIME(AInventoryItem, maxValue);
	DOREPLIFETIME(AInventoryItem, currentValue);
	DOREPLIFETIME(AInventoryItem, sprite);
}
