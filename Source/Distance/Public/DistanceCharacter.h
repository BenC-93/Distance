// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/Components/ChildActorComponent.h"
#include "Item.h"
//#include "InventoryItem.h"
//#include "NewItem.h"
#include "DistanceCharacter.generated.h"

enum ConvergenceState : uint32 { CONVERGENCE, DIVERGENCE1, DIVERGENCE2 };

UCLASS(Abstract, Blueprintable)
class ADistanceCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Set initial variables and create inventory */
	virtual void PostInitializeComponents() override;

	/* Update character variables */
	virtual void Tick(float DeltaSeconds) override;

	/* Cleanup inventory */
	virtual void Destroyed() override;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* ItemComponent;

public:
	ADistanceCharacter(const FObjectInitializer& ObjectInitializer);
	
	/** Player's maximum allowed health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	/** Player's base damage level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	float BaseDamage;
	
	/** Player's convergence state */
	ConvergenceState PlayerConvergenceState;

	TArray<class UTexture2D*> spriteInventory;

	/* Array index of currently equipped item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 EquippedSlot;

	/* Pick up nearby item object in the world */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void PickupItem(AItem* Item);

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void ItemPickedUp();

	/* Add an item to your inventory from the Item class */
	void AddItemOfClassToInventory(class TSubclassOf<class AItem> ItemClass);

	/* Drop currently equipped item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AItem* DropItem(int32 InvSlot);

	/* Equip a different item that is already in the inventory */
//	UFUNCTION(BlueprintCallable, Category = Inventory)
//	void EquipItem(int32 InvSlot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void EquipItemComponent(int32 itemIndex);

	/* Use the current equipped item (calls Item's Use() function) */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UseItem();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UTexture2D*> GetSpriteInventory();

	/* Returns the inventory array */
	TArray<class AItem*> GetInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Convergence)
	void BPTransitionToConvergenceState();

	UFUNCTION(BlueprintImplementableEvent, Category = Convergence)
	void BPTransitionToDivergenceState();


	/* The following functions are depreciated and will be removed soon:
	   ChangeLight, getLightAmount, getMaxLightAmount, getLightEnabled,
	   RegenerateLight */

	UFUNCTION(BlueprintCallable, Category = Item)
	void ChangeItemAmount(float itemAmount);

	UFUNCTION(BlueprintCallable, Category = Item)
	float GetMaxItemAmount();

	UFUNCTION(BlueprintCallable, Category = Item)//TODO: need to be able to enable this by some input
	bool GetItemEnabled();

	UFUNCTION(BlueprintCallable, Category = Speed)
	void ChangeSpeed(float speedAmount);

	/* Regeneration functions */
	virtual void BeginPlay();

	/** Changes player's target's health based on player attack */
	UFUNCTION(BlueprintCallable, Category = Damage)
	float Attack(float extra);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }



	/////////////////////*** INPUT HANDLING ***/////////////////////

	/* Player pressed next item button */
	void OnNextItem();

	/* Player pressed previous item button */
	void OnPrevItem();


	/////////////////////*** HEALTH and DAMAGE ***/////////////////////

	/* Health of character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;

	/* Health regeneration amount */
	UPROPERTY(EditDefaultsOnly, Category = Health)
	float HealthRegenAmount;

	/* Get max health */
	int32 GetMaxHealth() const;

	/* Changes player health */
	void ChangeHealth(float Amount);


	/////////////////////*** INVENTORY ***/////////////////////

//	/* Default inventory array */
//	UPROPERTY(BlueprintReadWrite, Category = Inventory)
//	TArray<TSubclassOf<class AItem>> DefaultInventoryClass;

	/* [server] Creates default inventory */
	void CreateDefaultInventory();

	/* Items in inventory */
	UPROPERTY(Transient, Replicated)
	TArray<class AItem*> Inventory;

	/* [server] Removes and destroys all items in inventory */
	void DestroyInventory();

	/* Currently equipped item*/
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentItem)
	class AItem* CurrentItem;

	/* Sets current item */
	void SetCurrentItem(class AItem* NewItem, class AItem* LastItem = NULL);

	/* Replication for setting current item */
	UFUNCTION()
	void OnRep_CurrentItem(class AItem* LastItem);

	/* [server] Add item to inventory */
	void AddItem(class AItem* Item);

	/* [server] Revome item from inventory */
	void RemoveItem(class AItem* Item);

	/* Find item in inventory */
	class AItem* FindItem(TSubclassOf<class AItem> ItemClass);

	/* [server + local] Equips a item from inventory */
	UFUNCTION(BlueprintCallable, Category = Item)
	void EquipItem(class AItem* Item);

	/* Equip item */
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipItem(class AItem* Item);

	/* Get number of inventory items */
	int32 GetInventoryCount() const;

	/* Get item from inventory at index */
	class AItem* GetInventoryItem(int32 index) const;


	/////////////////////*** ITEM USAGE ***/////////////////////

	/* [local] Starts item usage */
	void StartItemUse();

	/* [local] Stops items usage */
	void StopItemUse();

	/* Get using state */
	bool IsUsing() const;

	/* Current using state */
	uint8 bWantsToUse : 1;

	
	/////////////////////*** ITEM DATA ***/////////////////////

	/* Get equipped item */
	UFUNCTION(BlueprintCallable, Category = Item)
	class AItem* GetItem() const;

	/* Socket name for attaching item */
	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	FName ItemSocketPoint;

	/* Get item socket point */
	FName GetItemSocket() const;

};

