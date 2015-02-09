// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "Item.h"
#include "ItemDataList.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API UItemDataList : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(Category=Items, EditAnywhere)
	TArray<TSubclassOf<AItem>> ItemTypes;
	
	
};
