// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemShield.h"
#include "DistancePlayerController.h"

AItemShield::AItemShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	name = "Shield";
	drainRate = 1.0f;
	drainAmount = -2.0f;

	

	LightIntensity = 100.0f * amount;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;
}

void AItemShield::StartUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (!isInUse && amount > 0 && canUse)
	{
		
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		// start blueprint drain events
		BPStartDrain();

		// start use animation
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemShield::AnimationTimer, 0.68f, false);
		isInUse = true;
		LightSource->SetVisibility(true);
	}
}

void AItemShield::EndUse()
{
	if (isInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemShield::Drain);
		// stop blueprint drain events
		BPEndDrain();
		isInUse = false;
		//BPStartRegen();
		LightSource->SetVisibility(false);
	}
}

void AItemShield::AnimationTimer()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	playerController->canMove = true;
	canUse = true;
	GetOwningPawn()->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void AItemShield::Drain()
{
	ChangeAmount(drainAmount);
}

void AItemShield::ChangeAmount(float value)
{
	Super::ChangeAmount(value);
	LightIntensity = 100.0f * amount;
	LightSource->SetIntensity(LightIntensity);
}