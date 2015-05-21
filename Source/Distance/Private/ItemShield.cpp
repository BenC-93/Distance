// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemShield.h"
#include "DistancePlayerController.h"

AItemShield::AItemShield(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemName = "Shield";
	drainRate = 1.0f;
	drainAmount = -2.0f;
}

void AItemShield::StartUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (!bIsInUse && ItemAmount > 0 && bCanUse)
	{
		GetWorldTimerManager().SetTimer(this, &AItemShield::Drain, drainRate, true);
		// start use animation
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemShield::AnimationTimer, 0.68f, false);
		bIsInUse = true;
	}
}

void AItemShield::EndUse()
{
	if (bIsInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemShield::Drain);
		bIsInUse = false;
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