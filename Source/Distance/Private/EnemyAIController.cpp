// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Engine.h"
#include "AIEnemy.h"
#include "EnemyAIController.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerLoc = FVector(0.0f, 0.0f, 0.0f);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// keep updating the destination every tick while desired
	if (AIEnemyClass->moveToPlayer)
	{
		if (AIEnemyClass->player1 != NULL)//method for clostest todo
		{
			MoveToActor(AIEnemyClass->player1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 pooped and became null in controller."));
		}
		AIEnemyClass->moveToPlayer = false;
	}
	//UE_LOG(LogTemp, Warning, TEXT("My (Enemy) location is: %s"), *AIEnemyClass->GetActorLocation().ToString());
}

void AEnemyAIController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	AIEnemyClass = Cast<AAIEnemy>(InPawn);
}

void AEnemyAIController::ReactToPlayer()
{

}

void AEnemyAIController::Despawn()
{

}