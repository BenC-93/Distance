// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
//#include "DistanceCharacter.h"
#include "Item.generated.h"

USTRUCT()
struct FItemData
{
	GENERATED_USTRUCT_BODY()

	/* Name */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString Name;

	/* Whether item can be dropped from inventory */
	UPROPERTY(BlueprintReadWrite, Category = ItemStat)
	bool bIsDroppable;

	/* Maximum energy */
	UPROPERTY(BlueprintReadWrite, Category = Energy)
	float MaxEnergy;

	/* Amount of energy spent upon use */
	UPROPERTY(BlueprintReadWrite, Category = Energy)
	float EnergyUseAmount;

	/* Energy regeneration amount (per DeltaSecond) */
	UPROPERTY(BlueprintReadWrite, Category = Energy)
	float EnergyRegenAmount;

	/* Time that must pass between item uses */
	UPROPERTY(BlueprintReadWrite, Category = ItemStat)
	float TimeBetweenUses;

	/* Defaults */
	FItemData()
	{
		Name = TEXT("Unknown");
		bIsDroppable = true;
		MaxEnergy = 100.f;
		EnergyUseAmount = -10.f;
		EnergyRegenAmount = 0.1f;
		TimeBetweenUses = 0.1f;
	}

};

class ADistanceCharacter;
class UPaperSpriteComponent;

UCLASS(Abstract, Blueprintable)
class DISTANCE_API AItem : public AActor
{
	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;


	/////////////////////*** ENERGY ***/////////////////////

	/* Change energy */
	virtual void ChangeEnergy(float amount);


	/////////////////////*** INVENTORY ***/////////////////////

	/* Item is being equipped */
	virtual void OnEquip();

	/* Item is being unequipped */
	virtual void OnUnequip();

	/* [server] Item added to player's inventory */
	virtual void OnEnterInventory(ADistanceCharacter* NewOwner);

	/* [server] Item removed from player's inventory */
	virtual void OnLeaveInventory();

	/* Check if equipped */
	bool IsEquipped() const;


	/////////////////////*** INPUT ***/////////////////////

	/* [local + server] Starts use */
	virtual void StartUse();

	/* [local + server] Stops use */
	virtual void StopUse();

	/* Start regen */
	virtual void StartRegen(bool bFromRep = false);

	/* Stop regen */
	virtual void StopRegen();


	/////////////////////*** CONTROL ***/////////////////////

	/* Check if item can be used */
	bool CanBeUsed() const;

	/* Check if item can regen */
	bool CanRegen() const;


	/////////////////////*** DATA ***/////////////////////

	/* Get item name */
	FString GetName() const;

	/* Get whether item is droppable */
	bool GetIsDroppable() const;

	/* Get current energy amount */
	UFUNCTION(BlueprintCallable, Category = Item)
	float GetEnergy() const;

	/* Get maximum energy amount */
	float GetMaxEnergy() const;

	/* Get energy regen amount */
	float GetEnergyRegenAmount() const;

	/* Get usage amount */
	float GetUseAmount() const;

	/* Get item sprite */
	UPaperSpriteComponent* GetItemSprite() const;

	/* Get owner pawn */
	class ADistanceCharacter* GetCharacterOwner() const;

	/* Set owning pawn */
	void SetOwningPawn(ADistanceCharacter* NewPawn);


protected:

	/* Owning pawn */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ADistanceCharacter* MyPawn;

	/* Item data */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Data)
	FItemData ItemInfo;


private:

	/* Item sprite */
	UPROPERTY(VisibleDefaultsOnly, Category = Sprite)
	UPaperSpriteComponent* SpriteComponent;

protected:

	/* Use sound */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* UseSound;

	/* Equip sound */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* EquipSound;

	/* Is item equipped? */
	bool bIsEquipped : 1;

	/* Is item trying to be used? */
	bool bWantsToUse : 1;

	/* Whether item can regen */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Regen)
	uint32 bCanRegen : 1;

	/* Current energy */
	UPROPERTY(Transient, Replicated)
	float Energy;


	/////////////////////*** INPUT [SERVER SIDE] ***/////////////////////

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartUse();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopUse();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartRegen();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopRegen();


	/////////////////////*** REPLICATION HELPERS ***/////////////////////

	UFUNCTION()
	void OnRep_MyPawn();

	UFUNCTION()
	void OnRep_Regen();


	/////////////////////*** EFFECTS ***/////////////////////

	/* Use effects */
	virtual void StartUseEffects();

	/* Play item sounds */
	UAudioComponent* PlayItemSound(USoundCue* Sound);


	/////////////////////*** ITEM USAGE ***/////////////////////

	/* [local] Item specific use implementation */
	virtual void UseItem();

	/* [server] Use and update energy */
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleUsing();

	/* [local + server] Handle item use */
	void HandleUsing();


	/////////////////////*** SOCKETTING ***/////////////////////

	/* Attach item sprite to pawn mesh */
	void AttachSpriteToPawn();

	/* Detach item sprite from pawn mesh */
	void DetachSpriteFromPawn();

};
