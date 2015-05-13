// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "ConvergenceCrystal.h"
#include "ItemCrystal.generated.h"

class ADistanceCharacter;

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemCrystal : public AItem
{
	GENERATED_BODY()
	
public:

	float rangeToShrine;

	AItemCrystal(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;

	UPROPERTY(Category = Spirit, EditAnywhere)
	TSubclassOf<class ASpirit> SpiritClass;

	UPROPERTY(Category = Crystal, EditAnywhere)
	TSubclassOf<class AConvergenceCrystal> ConvergenceCrystalClass;
	
	void SpawnSpirit();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnSpirit();

	void SpawnConvergenceCrystal();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnConvergenceCrystal();
};
