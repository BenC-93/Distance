// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemShield.h"

AItemShield::AItemShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "Shield";
	drainRate = 1.0f;
	drainAmount = 2.0f;
}

void AItemShield::StartUse()
{
	if (!isInUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		isInUse = true;
	}
}

void AItemShield::EndUse()
{
	if (isInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemShield::Drain);
		isInUse = false;
	}
}

void AItemShield::Drain()
{
	ChangeAmount(drainAmount);
}