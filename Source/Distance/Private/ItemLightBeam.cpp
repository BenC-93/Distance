// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLightBeam.h"

AItemLightBeam::AItemLightBeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "LightBeam";
	chargeRate = 0.1f;
	chargeAmount = 0.5f;
	totalChargedAmount = 0.0f;

	TriggerBox->SetBoxExtent(FVector(70.0f, 50.0f, 125.0f), true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemLightBeam::OnOverlapBegin);
}

void AItemLightBeam::StartUse()
{
	if (!isInUse && amount > 0 && canUse == true)
	{
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::Charge, chargeRate, true);
		//start charging animation
		isInUse = true;
	}
}

void AItemLightBeam::EndUse()
{
	if (isInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemLightBeam::Charge);
		isInUse = false;
		canUse = false;
		//start attack animation
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::AnimationTimer, 2.0f, false);
	}
}

void AItemLightBeam::AnimationTimer()
{
	canUse = true;
}

void AItemLightBeam::Charge()
{
	ChangeAmount(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
}

void AItemLightBeam::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogDistance, Error, TEXT("Overlaped with: %s"), *OtherActor->GetName());
	if (isInUse)//totally doesnt work!
	{
		//check for boss collision stuff
		if (OtherActor && OtherActor->IsA(ATentacle::StaticClass()))//tentacles
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the tentacle!"));
			Cast<ATentacle>(OtherActor)->ChangeHealth(1 + totalChargedAmount);
			totalChargedAmount = 0.0f;
		}
	}
}