// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Distance.h"
#include "Item.h"
#include "Containers/UnrealString.h"
#include "InventoryItem.h"
/**
 * 
 */
class DISTANCE_API InventoryItem
{
public:
	TSubclassOf<class AItem> ItemClass;
	FString name;
	float maxValue;
	float currentValue;

	InventoryItem();
	InventoryItem(class AItem* item);
	void Update(class AItem* item);
	~InventoryItem();
};
