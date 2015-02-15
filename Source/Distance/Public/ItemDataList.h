// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "Item.h"
#include "ItemDataList.generated.h"

/**
 * 
 */
UCLASS()
class UItemDataList : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Category=Items, EditAnywhere)
	TArray<TSubclassOf<AItem>> ItemTypes;
	
	UFUNCTION(BlueprintCallable, Category = Item List)
	class TSubclassOf<AItem> ItemAtIndex(uint32 index);
};
