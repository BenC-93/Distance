// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DItemPickup.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DISTANCE_API ADItemPickup : public AActor
{
	GENERATED_BODY()
	
public:
	
	ADItemPickup(const FObjectInitializer& ObjectInitializer);
	
	/* Item's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString name;
	
	/* An abstract value used for various values, depending on item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Amount)
	float amount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> ItemClass;
	
//	
//	FUNCTIONS
//
	// Pickup the thing
	virtual void OnUsed(APawn* InstigatorPawn);
};
