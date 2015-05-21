// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DItem.h"
#include "ItemShield.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemShield : public ADItem
{
	GENERATED_BODY()
	
public:

	AItemShield(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	virtual void EndUse() override;

	float drainRate;
	float drainAmount;

	UFUNCTION(BlueprintCallable, Category = Amount)
	void Drain();

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void AnimationTimer();
	
};
