// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ItemDataList.h"
#include "Item.h"
#include "DistanceGameMode.generated.h"

UCLASS(minimalapi)
class ADistanceGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ADistanceGameMode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category=Items, EditAnywhere)
	TArray<TSubclassOf<AItem>> ItemTypes;//maybe convert to TMap, inport from Engine/source/runtime/core/public/containers/Map.h
	
	UFUNCTION(BlueprintCallable, Category = Items)
	class AItem* SpawnLantern();
	
	UFUNCTION(BlueprintCallable, Category = Items)
	class AItem* SpawnItemAtLocation(TSubclassOf<class AItem> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Items)
	void SpawnRandomItemAtLocation(FVector location);

	class TSubclassOf<class AItem> ItemFromIndex(uint32 ItemIndex);
};
