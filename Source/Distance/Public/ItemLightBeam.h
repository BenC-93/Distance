// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DItem.h"
#include "ItemLightBeam.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemLightBeam : public ADItem
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers)
	class UBoxComponent *TriggerBox;

	AItemLightBeam(const class FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void StartUse() override;
	virtual void EndUse() override;

	// ooooooh prettyyyyyyy
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	float LightIntensity;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	class UPointLightComponent* LightSource;

	float chargeRate;
	float chargeAmount;
	float totalChargedAmount;
	float lightChargeAmount;

	class AActor* targetActor;
	bool hasAttacked;

	UFUNCTION(BlueprintCallable, Category = Charge)
	void Charge();

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void AnimationTimer();

	/* Plays hit/swing sound in blueprint*/
	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void BPPlayHitSound();

	/* Plays charging sound in blueprint*/
	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void BPStartCharging();

	/* Ends charging sound in blueprint*/
	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void BPEndCharging();

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
