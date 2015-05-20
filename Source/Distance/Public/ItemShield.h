// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "ItemShield.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemShield : public AItem
{
	GENERATED_BODY()
	
public:

	AItemShield(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	virtual void EndUse() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Drain)
	float drainRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Drain)
	float drainAmount;

	UFUNCTION(BlueprintCallable, Category = Amount)
	void Drain();

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void AnimationTimer();

	UFUNCTION(BlueprintImplementableEvent, Category = Shield)
	void BPStartDrain();

	UFUNCTION(BlueprintImplementableEvent, Category = Shield)
	void BPEndDrain();

	UFUNCTION(BlueprintImplementableEvent, Category = Shield)
	void BPStartRegen();

	UFUNCTION(BlueprintImplementableEvent, Category = Shield)
	void BPEndRegen();


	
};
