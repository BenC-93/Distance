// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "ItemCrystal.h"
#include "Spirit.h"
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
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(true);
		if (OwningPawn != NULL) OwningPawn->DropItem(OwningPawn->EquippedSlot)->Destroy();
		Drop();
	}
}

void AItemCrystal::StartUse(bool spawnSpirit)
{
	if (spawnSpirit)
	{
		if (!isInUse)
		{
			printScreen(FColor::Red, TEXT("Spawning Spirit"));
			if (OwningPawn != NULL)
			{
				SpawnSpirit();

				OwningPawn->DropItem(OwningPawn->EquippedSlot)->Destroy();
			}
			Drop();
		}
	}
	else
	{
		StartUse();
	}
}

void AItemCrystal::SpawnSpirit()
{
	ASpirit* spirit = Cast<ASpirit>(GetWorld()->SpawnActor<ASpirit>(SpiritClass, FVector(OwningPawn->GetActorLocation()) - FVector(150.0f, 0.0f, 0.0f), FRotator(OwningPawn->GetActorRotation())));
	spirit->SetOwner(OwningPawn);
	if (Role < ROLE_Authority)
	{
		ServerSpawnSpirit();
	}
}

bool AItemCrystal::ServerSpawnSpirit_Validate()
{
	return true;
}

void AItemCrystal::ServerSpawnSpirit_Implementation()
{
	SpawnSpirit();
}