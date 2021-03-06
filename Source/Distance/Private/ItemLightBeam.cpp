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

	targetActor = NULL;
	hasAttacked = false;//so the tick if statement only happens once per attack

	PrimaryActorTick.bCanEverTick = true;

	TriggerBox->SetBoxExtent(FVector(70.0f, 50.0f, 125.0f), true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemLightBeam::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AItemLightBeam::OnOverlapEnd);
}

void AItemLightBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!hasAttacked && targetActor && GetWorldTimerManager().IsTimerActive(this, &AItemLightBeam::AnimationTimer))
	{
		//UE_LOG(LogDistance, Error, TEXT("timer active and targetActor is: %s"), *targetActor->GetName());
		//check for boss collision stuff
		if (targetActor->IsA(ATentacle::StaticClass()))//tentacles
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the tentacle!"));
			Cast<ATentacle>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
		}
	}
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
	hasAttacked = false;
}

void AItemLightBeam::Charge()
{
	ChangeAmount(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
}

void AItemLightBeam::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		targetActor = OtherActor;
		UE_LOG(LogDistance, Warning, TEXT("Overlaped with: %s"), *targetActor->GetName());
	}
}

void AItemLightBeam::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//UE_LOG(LogDistance, Warning, TEXT("Exit Overlap with: %s"), *OtherActor->GetName());
		targetActor = NULL;
		UE_LOG(LogDistance, Warning, TEXT("TargetActor is NULL"));
	}
}