// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "InventoryItem.h"

InventoryItem::InventoryItem()
{
	ItemClass = NULL;
	name = "Default";
	maxValue = 100;
	currentValue = 100;
}

InventoryItem::InventoryItem(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->name;
	maxValue = item->maxAmount;
	currentValue = item->amount;
}

void InventoryItem::Update(class AItem* item)
{
	ItemClass = item->GetClass();
	name = item->name;
	maxValue = item->maxAmount;
	currentValue = item->amount;
}

InventoryItem::~InventoryItem()
{
}
