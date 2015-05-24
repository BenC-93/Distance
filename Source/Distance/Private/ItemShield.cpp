// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemShield.h"
#include "DistancePlayerController.h"

AItemShield::AItemShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemName = "Shield";
	drainRate = 0.1f;
	drainAmount = -1.0f;

	

	LightIntensity = 100.0f * ItemAmount;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;
}

void AItemShield::StartUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (!bIsInUse && ItemAmount > 0 && bCanUse)
	{
		
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		// start blueprint drain events
		BPStartDrain();

		// start use animation
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemShield::AnimationTimer, 0.68f, false);
		bIsInUse = true;
		LightSource->SetVisibility(true);
	}
}

void AItemShield::EndUse()
{
	if (bIsInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemShield::Drain);
		bIsInUse = false;
		//BPStartRegen();
		LightSource->SetVisibility(false);

		// stop blueprint drain events
		BPEndDrain();
	}
}

void AItemShield::AnimationTimer()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	playerController->canMove = true;
	bCanUse = true;
	GetOwningPawn()->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void AItemShield::Drain()
{
	ChangeAmount(drainAmount);
}

void AItemShield::ChangeAmount(float value)
{
	Super::ChangeAmount(value);
	LightIntensity = 100.0f * ItemAmount;
	LightSource->SetIntensity(LightIntensity);
}