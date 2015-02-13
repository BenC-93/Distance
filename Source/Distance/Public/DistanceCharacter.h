// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/Components/ChildActorComponent.h"
#include "Item.h"
#include "DistanceCharacter.generated.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float Health;
	
	/** Player's maximum allowed health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	/** Player's base damage level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	float BaseDamage;
	
	/* Temporary function for implementing the light(lantern) item */
	AItem* GetLight();

	/* Inventory array */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TArray<class AItem*> Inventory;

	/* Array index of currently equipped item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 EquippedSlot;

	/* Pick up nearby item object in the world */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void PickupItem(AItem* Item);

	/* Drop currently equipped item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void DropItem(int32 InvSlot);

	/* Equip a different item that is already in the inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void EquipItem(int32 InvSlot);

	/* Use the current equipped item (calls Item's Use() function) */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UseItem();

	/* Returns the inventory array */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class AItem*> GetInventory();

	/* Toggle visibility of inventory GUI */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ToggleInventory();

	/* The following functions are depreciated and will be removed soon:
	   ChangeLight, getLightAmount, getMaxLightAmount, getLightEnabled,
	   RegenerateLight */
	UFUNCTION(BlueprintCallable, Category = Light)
	void ChangeLight(float lightAmount);

	UFUNCTION(BlueprintCallable, Category = Light)
	float getLightAmount();

	UFUNCTION(BlueprintCallable, Category = Light)
	float getMaxLightAmount();

	UFUNCTION(BlueprintCallable, Category = Light)//TODO: need to be able to enable this by some input
	bool getLightEnabled();

	UFUNCTION(BlueprintCallable, Category = Speed)
	void ChangeSpeed(float speedAmount);

	/** Changes player's health based on an attack or item effect */
	UFUNCTION(BlueprintCallable, Category = Health)
	void ChangeHealth(float healthAmount);


	/* Regeneration functions */
	void RegenerateHealth();
	void RegenerateLight();
	void StartRegeneration();

	/** Changes player's target's health based on player attack */
	UFUNCTION(BlueprintCallable, Category = Damage)
	float Attack();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

