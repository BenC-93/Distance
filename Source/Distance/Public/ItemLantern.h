// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DItem.h"
#include "ItemLantern.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DISTANCE_API AItemLantern : public ADItem
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	float LightIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	class UPointLightComponent* LightSource;

	AItemLantern(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	virtual void EndUse() override;
	virtual void ChangeAmount(float value) override;
	
};