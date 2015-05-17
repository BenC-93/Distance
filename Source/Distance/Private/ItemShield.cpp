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
}

void AItemShield::StartUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (!isInUse && amount > 0 && canUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		// start use animation
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemShield::AnimationTimer, 0.68f, false);
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