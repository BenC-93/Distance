// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "EnemyAIController.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// keep updating the destination every tick while desired

}

void AEnemyAIController::ReactToPlayer()
{

}

void AEnemyAIController::MoveToPlayer()
{

}

void AEnemyAIController::Despawn()
{

}