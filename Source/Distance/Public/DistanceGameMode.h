// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ItemDataList.h"
#include "DistanceGameMode.generated.h"

UCLASS(minimalapi)
class ADistanceGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(Category=Items, EditAnywhere)
	TSubclassOf<class UItemDataList> ItemList;
	
	UItemDataList* ActualItemList;
public:
	ADistanceGameMode(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Items)
	void SpawnItem();
};
