// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLightBeam.h"

AItemLightBeam::AItemLightBeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.Name = "LightBeam";
	chargeRate = 1.0f;
	chargeAmount = 2.0f;
	totalChargedAmount = 0.0f;
}

void AItemLightBeam::StartUse()
{
	if (!bWantsToUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::Charge, chargeRate, true);
		bWantsToUse = true;
	}
}

void AItemLightBeam::StopUse()
{
	if (bWantsToUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemLightBeam::Charge);
		bWantsToUse = false;
	}
}

void AItemLightBeam::Charge()
{
	ChangeEnergy(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
}