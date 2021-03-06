// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/Components/ChildActorComponent.h"
#include "Item.h"
#include "InventoryItem.h"
#include "DistanceCharacter.generated.h"

// make ConvergenceState enum blueprintable
UENUM(BlueprintType)
enum class ConvergenceState : uint8
{
	CONVERGENCE	UMETA(DisplayName = "Convergence"),
	DIVERGENCE1	UMETA(DisplayName = "Divergence1"),
	DIVERGENCE2 UMETA(DisplayName = "Divergence2")
};

UCLASS(Blueprintable)
class ADistanceCharacter : public ACharacter
{
	GENERATED_BODY()

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

	/** Player's current health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;
	
	/** Player's maximum allowed health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float MaxHealth;

	/** Player's base damage level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	float BaseDamage;
	
	/** Player's convergence state */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Convergence)
	ConvergenceState PlayerConvergenceState;
	
	/* Currently held item */
	AItem* GetItem();

	/* Inventory array */
	UPROPERTY(Replicated)
	TArray<AInventoryItem*> Inventory;//was tarray of class aitem*

	/* Array index of currently equipped item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 EquippedSlot;

	UFUNCTION(BlueprintImplementableEvent, Category = Camera)
	void PlayConvergenceAnim();

	/* Pick up nearby item object in the world */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void PickupItem(AItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickupItem(AItem* Item);
	
	UFUNCTION(NetMulticast, Reliable)
	void ClientPickupItem(AItem* Item);

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void ItemPickedUp();

	/* Drop currently equipped item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AItem* DropItem(int32 InvSlot);

	/* Equip a different item that is already in the inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void EquipItem(int32 InvSlot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void EquipItemComponent(int32 itemIndex);

	/* Use the current equipped item (calls Item's Use() function) */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UseItem();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UTexture2D*> GetSpriteInventory();

	/* Returns the inventory array */
	TArray<class AInventoryItem*> GetInventory();

	/* Toggle visibility of inventory GUI */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ToggleInventory();

	/* Grabs the Name of the Item for comparison for Bosses*/
	UFUNCTION(BlueprintCallable, Category = Light)
	FString GetItemName();

	UFUNCTION(BlueprintCallable, Category = Light)
	bool GetIsItemDroppable();
	
	UFUNCTION(Unreliable, NetMulticast)
	void FlipForMovement_RPC(const FVector DestLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = Convergence)
	void BPTransitionToNewConvergenceState();

	UFUNCTION(BlueprintCallable, Category = Item)
	void ChangeItemAmount(float lightAmount);

	UFUNCTION(BlueprintCallable, Category = Item)
	float GetItemAmount();

	UFUNCTION(BlueprintCallable, Category = Item)
	float GetMaxItemAmount();

	UFUNCTION(BlueprintCallable, Category = Item)
	bool GetItemEnabled();

	UFUNCTION(BlueprintCallable, Category = Item)
	bool GetItemHasOwner();

	UFUNCTION(BlueprintCallable, Category = Speed)
	void ChangeSpeed(float speedAmount);

	/** Changes player's health based on an attack or item effect */
	UFUNCTION(BlueprintCallable, Category = Health)
	void ChangeHealth(float healthAmount);


	/* Regeneration functions */
	virtual void BeginPlay();
	
	void RegenerateHealth();
	void StartRegeneration();

	/** Changes player's target's health based on player attack */
	UFUNCTION(BlueprintCallable, Category = Damage)
	float Attack(float extra);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

