// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "ItemLantern.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DISTANCE_API AItemLantern : public AItem
{
	GENERATED_BODY()

public:

	AItemLantern(const class FObjectInitializer& ObjectInitializer);
	virtual void StartUse() override;
	virtual void EndUse() override;
	
};
