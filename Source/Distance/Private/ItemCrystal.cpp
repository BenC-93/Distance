// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "ItemCrystal.h"
#include "Spirit.h"
#include "Shrine.h"
#include "ConvergenceManager.h"

AItemCrystal::AItemCrystal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "Crystal";
	rangeToShrine = 350.f;
}

void AItemCrystal::StartUse()
{
	if (!isInUse && OwningPawn != NULL)
	{
		bool shrineFound = false;
		for (TActorIterator<AShrine> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (OwningPawn->GetDistanceTo(*ActorItr) <= rangeToShrine)
			{
				printScreen(FColor::Red, TEXT("Shrine Found"));
				shrineFound = true;
				break;
			}
		}
		if (shrineFound && OwningPawn->PlayerConvergenceState != ConvergenceState::CONVERGENCE)
		{
			ConvergenceManager::StartConvergence();
			GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(true);
			SpawnConvergenceCrystal();
		}
		else if (OwningPawn->PlayerConvergenceState != ConvergenceState::CONVERGENCE)
		{
			SpawnSpirit();
		}
		OwningPawn->DropItem(OwningPawn->EquippedSlot)->Destroy();
		Drop();
	}
}

void AItemCrystal::SpawnSpirit()
{
	ASpirit* spirit = Cast<ASpirit>(GetWorld()->SpawnActor<ASpirit>(SpiritClass, FVector(OwningPawn->GetActorLocation()) - FVector(150.0f, 0.0f, 0.0f), FRotator(OwningPawn->GetActorRotation())));
	spirit->SetOwner(OwningPawn);
	/*if (Role < ROLE_Authority)
	{
		ServerSpawnSpirit();
	}*/
}

bool AItemCrystal::ServerSpawnSpirit_Validate()
{
	return true;
}

void AItemCrystal::ServerSpawnSpirit_Implementation()
{
	SpawnSpirit();
}

void AItemCrystal::SpawnConvergenceCrystal()
{
	GetWorld()->SpawnActor<AConvergenceCrystal>(ConvergenceCrystalClass, FVector(OwningPawn->GetActorLocation()) - FVector(150.0f, 0.0f, 0.0f), FRotator(OwningPawn->GetActorRotation()));
	/*if (Role < ROLE_Authority)
	{
	ServerSpawnSpirit();
	}*/
}

bool AItemCrystal::ServerSpawnConvergenceCrystal_Validate()
{
	return true;
}

void AItemCrystal::ServerSpawnConvergenceCrystal_Implementation()
{
	SpawnConvergenceCrystal();
}