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
	FRotator playerDirection = player->GetVelocity().Rotation();//Yaw is only thing that changes

	UE_LOG(LogTemp, Warning, TEXT("Rotation before: %s"), *playerDirection.ToString());
	/*
	Yaw	| Unit Circle
	0		90
	90		0
	45		45
	-45		135
	-90		180
	180		270
	-180	270
	*/
	/*
	//for reversing direction
	if (playerDirection.Yaw > 0)//positive
	{
		playerDirection.Yaw -= 180;
	}
	else if (playerDirection.Yaw <= 0)//negative
	{
		playerDirection.Yaw += 180;//chaning it to the opposite direction
	}*/

	UE_LOG(LogTemp, Warning, TEXT("Rotation after: %s"), *playerDirection.ToString());
	//FVector myPos = AIEnemyClass->GetVelocity();//my position
	FVector myPos = AIEnemyClass->GetActorLocation();
	//myPos = playerDirection.RotateVector(myPos);//getting a vector of the opposite direction from the player
	FVector dir = playerDirection.Vector();
	UE_LOG(LogTemp, Warning, TEXT("vector of rotation after: %s"), *dir.ToString());
	float lengthDir = 600.0f;
	//dir.ToDirectionAndLength(dir, lengthDir);
	dir *= lengthDir;
	UE_LOG(LogTemp, Warning, TEXT("vector of rotation with length: %s"), *dir.ToString());
	UE_LOG(LogTemp, Warning, TEXT("old location: %s"), *myPos.ToString());
	FVector loc = myPos + dir;
	UE_LOG(LogTemp, Warning, TEXT("new location: %s"), *loc.ToString());
	EPathFollowingRequestResult::Type newLocPath = MoveToLocation(loc, 0.5f);
	if (newLocPath == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("new location path: Failed"));
	}
	else if (newLocPath == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("new location path: Success"));
	}
	else if (newLocPath == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		UE_LOG(LogTemp, Warning, TEXT("new location path: Already At Goal"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("new location path: IDK"));
	}
}

void AEnemyAIController::ReactToPlayer()
{

}

void AEnemyAIController::Despawn()
{

}