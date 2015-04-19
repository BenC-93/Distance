// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemShield.h"

AItemShield::AItemShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.Name = "Shield";
	drainRate = 1.0f;
	drainAmount = -2.0f;
}

void AItemShield::StartUse()
{
	if (!bWantsToUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		bWantsToUse = true;
	}
}

void AItemShield::StopUse()
{
	if (bWantsToUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemShield::Drain);
		bWantsToUse = false;
	}
}

void AItemShield::Drain()
{
	ChangeEnergy(drainAmount);
}