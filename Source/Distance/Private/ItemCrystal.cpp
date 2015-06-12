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
	ItemName = "Crystal";
	rangeToShrine = 350.f;
}

void AItemCrystal::StartUse()
{
	if (!bIsInUse && OwningPawn != NULL)
	{
		bool shrineFound = false;
		ADistanceGameMode* GameMode = GetWorld()->GetAuthGameMode<ADistanceGameMode>();
		for (TActorIterator<AShrine> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (OwningPawn->GetDistanceTo(*ActorItr) <= rangeToShrine)
			{
				//printScreen(FColor::Red, TEXT("Shrine Found"));
				UE_LOG(LogDistance, Warning, TEXT("Shrine Found"));
				shrineFound = true;
				break;
			}
		}
		if (shrineFound && OwningPawn->PlayerConvergenceState != ConvergenceState::CONVERGENCE)
		{
			GameMode->SpawnBossForConvergence();
			ConvergenceManager::StartConvergence();
			GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(true);
			SpawnConvergenceCrystal();
			// play crystal use sound
			BPPlayUseSound();
			GameMode->OnCrystalUsed(true);
		}
		else if (OwningPawn->PlayerConvergenceState != ConvergenceState::CONVERGENCE)
		{
			SpawnSpirit();
			// play crystal use sound
			BPPlayUseSound();
			GameMode->OnCrystalUsed(false);
		}
		else
		{
			// TODO: Something else should probably happen here other than the crystal just getting consumed
			GameMode->OnCrystalUsed(false);
		}
		
		// Remove from inventory and consume
		OwningPawn->RemoveItem(this);
		Destroy();
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
	AConvergenceCrystal* crystal = GetWorld()->SpawnActor<AConvergenceCrystal>(ConvergenceCrystalClass, FVector(OwningPawn->GetActorLocation()) - FVector(150.0f, 0.0f, 0.0f), FRotator(OwningPawn->GetActorRotation()));
	
	// Notify players that the crystal spawned
	ADistanceCharacter* p1 = Cast<ADistanceCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ADistanceCharacter* p2 = Cast<ADistanceCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 1));
	p1->OnConvergenceCrystalSpawned(crystal);
	p2->OnConvergenceCrystalSpawned(crystal);
	
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