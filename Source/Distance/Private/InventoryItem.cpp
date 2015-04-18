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
	name = item->name;
	maxValue = item->maxAmount;
	currentValue = item->amount;
	sprite = item->GetTheSprite();
}

UTexture2D* UInventoryItem::GetItemSprite()
{
	return sprite;
}

void UInventoryItem::Update(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->name;
	maxValue = item->maxAmount;
	currentValue = item->amount;
}

UInventoryItem::~UInventoryItem()
{
}
