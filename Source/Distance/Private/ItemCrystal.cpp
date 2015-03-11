// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemCrystal.h"
#include "ConvergenceManager.h"

AItemCrystal::AItemCrystal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "Crystal";
}

void AItemCrystal::StartUse()
{
	if (!isInUse)
	{
		printScreen(FColor::Red, TEXT("Beginning Convergence"));
		ConvergenceManager::StartConvergence();
		//drop itself
		//destroy itself
		//Destroy();
	}
}