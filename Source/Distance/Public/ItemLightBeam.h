// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "ItemLightBeam.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemLightBeam : public AItem
{
	GENERATED_BODY()
	
public:

	AItemLightBeam(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	virtual void EndUse() override;

	float chargeRate;
	float chargeAmount;
	float totalChargedAmount;

	UFUNCTION(BlueprintCallable, Category = Charge)
	void Charge();
	
	
};
