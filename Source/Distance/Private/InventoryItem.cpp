// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "InventoryItem.h"

UInventoryItem::UInventoryItem()
{
	ItemClass = NULL;
	name = "Default";
	maxValue = 100;
	currentValue = 100;
	sprite = NULL;
}

UInventoryItem::UInventoryItem(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetName();
	maxValue = item->GetMaxEnergy();
	currentValue = item->GetEnergy();
	sprite = item->GetItemSprite()->GetSprite()->GetSourceTexture();
}

UTexture2D* UInventoryItem::GetItemSprite()
{
	return sprite;
}

void UInventoryItem::Update(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetName();
	maxValue = item->GetMaxEnergy();
	currentValue = item->GetEnergy();
}

UInventoryItem::~UInventoryItem()
{
}
