// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Object.h"
//#include "Item.h"
#include "Containers/UnrealString.h"
#include "InventoryItem.generated.h"
/**
 * 
 */
UCLASS()
class DISTANCE_API UInventoryItem : public UObject
{
	GENERATED_BODY()
public:
	TSubclassOf<class AItem> ItemClass;
	FString name;
	float maxValue;
	float currentValue;
	UTexture2D* sprite;

	UInventoryItem();
	UInventoryItem(class AItem* item);
	UTexture2D* GetItemSprite();
	FString GetItemName();
	void SetItemName(FString n);
	void Update(class AItem* item);
	~UInventoryItem();
};
