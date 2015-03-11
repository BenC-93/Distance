// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "ItemCrystal.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemCrystal : public AItem
{
	GENERATED_BODY()
	
public:

	AItemCrystal(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	
};
