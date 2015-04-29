// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
//#include "Item.h"
#include "Containers/UnrealString.h"
#include "InventoryItem.generated.h"
/**
 * 
 */
UCLASS()
class DISTANCE_API AInventoryItem : public AActor
{
	GENERATED_BODY()
public:
	AInventoryItem(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Replicated)
	TSubclassOf<class AItem> ItemClass;
	UPROPERTY(Replicated)
	FString name;
	UPROPERTY(Replicated)
	float maxValue;
	UPROPERTY(Replicated)
	float currentValue;
	UPROPERTY(Replicated)
	UTexture2D* sprite;

	void CreateFromItem(AItem* item);
	UTexture2D* GetItemSprite();
	FString GetItemName();
	void SetItemName(FString n);
	void Update(class AItem* item);
};
