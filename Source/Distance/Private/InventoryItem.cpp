// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "InventoryItem.h"

UInventoryItem::UInventoryItem()
{
	ItemClass = NULL;
	name = TEXT("Default");
	maxValue = 100.0f;
	currentValue = 100.0f;
	sprite = NULL;
}

UInventoryItem::UInventoryItem(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetItemName();
	maxValue = item->maxAmount;
	currentValue = item->amount;
	sprite = item->GetTheSprite();
}

UTexture2D* UInventoryItem::GetItemSprite()
{
	return sprite;
}

FString UInventoryItem::GetItemName()
{
	return name;
}

void UInventoryItem::SetItemName(FString n)
{
	name = n;
}

void UInventoryItem::Update(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->GetItemName();
	maxValue = item->maxAmount;
	currentValue = item->amount;
}

UInventoryItem::~UInventoryItem()
{
}
