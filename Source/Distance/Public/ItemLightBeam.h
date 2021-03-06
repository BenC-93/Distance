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
	virtual void Tick(float DeltaTime) override;
	virtual void StartUse() override;
	virtual void EndUse() override;

	float chargeRate;
	float chargeAmount;
	float totalChargedAmount;

	class AActor* targetActor;
	bool hasAttacked;

	UFUNCTION(BlueprintCallable, Category = Charge)
	void Charge();

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void AnimationTimer();

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
