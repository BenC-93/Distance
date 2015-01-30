// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DISTANCE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:

	AItem(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	bool bIsEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	float amount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* SpriteComponent;
};