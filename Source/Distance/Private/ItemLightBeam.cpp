// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLightBeam.h"

AItemLightBeam::AItemLightBeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "LightBeam";
	chargeRate = 1.0f;
	chargeAmount = 2.0f;
	totalChargedAmount = 0.0f;
}

void AItemLightBeam::StartUse()
{
	if (!isInUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::Charge, chargeRate, true);
		isInUse = true;
	}
}

void AItemLightBeam::EndUse()
{
	if (isInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemLightBeam::Charge);
		isInUse = false;
	}
}

void AItemLightBeam::Charge()
{
	ChangeAmount(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
}