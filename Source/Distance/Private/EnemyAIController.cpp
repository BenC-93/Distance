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

	if (AIEnemyClass->moveToPlayer)//Constantly moves toward player until light or life is drained or something
	{
		if (AIEnemyClass->currentPlayer != NULL)//method for clostest todo!!!!
		{
			AIMoveToPlayer(AIEnemyClass->currentPlayer);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 pooped and became null in controller."));
		}
		//AIEnemyClass->moveToPlayer = false;//if we don't want AI to constantly follow, we uncomment this line
	}

	if (AIEnemyClass->moveAway)
	{
		if (AIEnemyClass->currentPlayer != NULL)//method for clostest todo!!!!
		{
			AIMoveAwayFromPlayer(AIEnemyClass->currentPlayer);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 pooped and became null in controller."));
		}
		AIEnemyClass->moveAway = false;
	}
	//UE_LOG(LogTemp, Warning, TEXT("My (Enemy) location is: %s"), *AIEnemyClass->GetActorLocation().ToString());
}

void AEnemyAIController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	AIEnemyClass = Cast<AAIEnemy>(InPawn);
}

void AEnemyAIController::AIMoveToPlayer(class ACharacter* player)
{
	MoveToActor(player);
}

void AEnemyAIController::AIMoveAwayFromPlayer(class ACharacter* player)//No clue if this works yet
{
	FRotator playerDirection = player->GetVelocity().Rotation();
	playerDirection.Pitch += 180;
	playerDirection.Yaw += 180;//chaning it to the opposite direction
	FVector myPos = AIEnemyClass->GetVelocity();//my position
	//myPos = playerDirection.RotateVector(myPos);//getting a vector of the opposite direction from the player
	FVector dir = playerDirection.Vector();
	float lengthDir = 100.0f;
	myPos.ToDirectionAndLength(dir, lengthDir);
	MoveToLocation(myPos);
}

void AEnemyAIController::ReactToPlayer()
{

}

void AEnemyAIController::Despawn()
{

}