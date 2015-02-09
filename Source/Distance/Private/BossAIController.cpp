// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "BossAIController.h"

ABossAIController::ABossAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ABossAIController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	AIBoss_DoubtClass = Cast<AAIBoss_Doubt>(InPawn);
}