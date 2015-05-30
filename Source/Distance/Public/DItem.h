// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DItem.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API ADItem : public AActor
{
	GENERATED_BODY()

public:
	ADItem(const FObjectInitializer& ObjectInitializer);
	
	/* Item's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString ItemName;
	
	/* Reference to player holding this item */
	UPROPERTY(EditAnywhere, Category = Item)
	ADistanceCharacter* OwningPawn;
	
	UPROPERTY(Category = Animation, EditAnywhere)
	UAnimationAsset* UseAnimation;
	
	/* Whether item can be dropped once in inventory.
	 Used primarily for the 'Lantern' item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	bool bIsDroppable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Use)
	bool bCanUse;
	
	/* Whether the item is currently being used by the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Use)
	bool bIsInUse;
	
	/* An abstract value used for various values, depending on item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Amount)
	float ItemAmount;
	
	/* Item defined maximum value for arbitrary 'amount' value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Amount)
	float MaxItemAmount;
	
	/* Time in seconds that passes before regenerating. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Regen)
	float RegenRate;
	
	/* Amount to regenerate by. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Regen)
	float RegenAmount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;
	
	FString GetItemName() const;
	
	class UTexture2D* GetItemSprite() const;
	
	void SetOwningPawn(ADistanceCharacter* NewOwner);
	
	void OnEnterInventory(ADistanceCharacter* NewOwner);
	
	void OnLeaveInventory();
	
	/* Item's primary action for when it is use by the player. */
	UFUNCTION(BlueprintCallable, Category = Use)
	virtual void StartUse();
	
	UFUNCTION(BlueprintCallable, Category = Use)
	virtual void EndUse();
	
	/* What happens when the player equips the item. */
	UFUNCTION(BlueprintCallable, Category = Item)
	void OnEquip();
	
	/* What happens when the player unequips the item. */
	UFUNCTION(BlueprintCallable, Category = Item)
	void OnUnequip();
	
	void AttachToPawn();
	
	void DetachFromPawn();
	
	/* Changes 'ItemAmount' variable by value, within the bounds [0.0, MaxItemAmount]. */
	UFUNCTION(BlueprintCallable, Category = Amount)
	virtual void ChangeAmount(float value);
	
	void Regenerate();
	
	class ADistanceCharacter* GetOwningPawn();
	
	virtual void FlipForDirection(float xFlip = 1.0f);
};
