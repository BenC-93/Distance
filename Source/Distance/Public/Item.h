// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "InventoryItem.h"
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

	/* Item's name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString name;

	/* Whether item can be dropped once in inventory. 
	   Used primarily for the 'Lantern' item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Item)
	bool droppable;

	/* Whether the item is currently being used by the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Use)
	bool isInUse;

	/* An abstract value used for various values, depending on item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Amount)
	float amount;

	/* Item defined maximum value for arbitrary 'amount' value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Amount)
	float maxAmount;

	/* Time in seconds that passes before regenerating. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Regen)
	float regenRate;

	/* Amount to regenerate by. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Regen)
	float regenAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers)
	class UBoxComponent *TriggerBox;
	
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

	/* What happens when a player picks up this item. */
	UFUNCTION(BlueprintCallable, Category = Item)
	void Pickup();

	/* What happens when the player drops this item. */
	UFUNCTION(BlueprintCallable, Category = Item)
	void Drop();

	/* Returns the current value of arbitrary 'amount' variable. */
	UFUNCTION(BlueprintCallable, Category = Amount)
	float GetAmount();

	/* Returns the value of 'maxAmount' variable. */
	UFUNCTION(BlueprintCallable, Category = Amount)
	float GetMaxAmount();

	/* Changes 'amount' variable by value, within the bounds [0.0, maxAmount]. */
	UFUNCTION(BlueprintCallable, Category = Amount)
	virtual void ChangeAmount(float value);

	/* Adds to 'amount' at rate 'rate'. */
	UFUNCTION(BlueprintCallable, Category = Regen)
	void Regenerate();

	FString GetItemName();
	void SetItemName(FString n);

	void Update(class AInventoryItem* invItem);

	UTexture2D* GetTheSprite() const;
};
