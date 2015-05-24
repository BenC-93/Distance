// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Engine.h"
#include "AIEnemy.h"
//#include "UnrealNetwork.h"
#include "EnemyAIController.h"
#include "Navigation/NavigationComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerLoc = FVector(0.0f, 0.0f, 0.0f);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIEnemyClass->currentPlayer)
	{
		if (AIEnemyClass->moveToPlayer)//Constantly moves toward player until light or life is drained or something
		{
			//AIMoveToPlayer(AIEnemyClass->currentPlayer);
			//AIEnemyClass->moveToPlayer = false;//if we don't want AI to constantly follow, we uncomment this line
		}
		else if (AIEnemyClass->moveAway)
		{
			AIMoveAwayFromPlayer(AIEnemyClass->currentPlayer);
			AIEnemyClass->moveAway = false;
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Player1 pooped and became null in controller."));
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
	EPathFollowingRequestResult::Type newLocPath = MoveToActor(player, 0.1f);
	if (newLocPath == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTemp, Error, TEXT("Moving to player: new location path: Failed"));
	}
	else if (newLocPath == EPathFollowingRequestResult::RequestSuccessful)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Moving to player: new location path: Success"));
	}
	else if (newLocPath == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		//UE_LOG(LogTemp, Error, TEXT("Moving to player: new location path: Already At Goal"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Moving to player: new location path: IDK"));
	}
}

void AEnemyAIController::AIMoveAwayFromPlayer(class ACharacter* player)
{
	//if (!AIEnemyClass->prepareToDie)
	//{
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

		//UE_LOG(LogTemp, Warning, TEXT("Rotation after: %s"), *playerDirection.ToString());
		//FVector myPos = AIEnemyClass->GetVelocity();//my position
		FVector myPos = AIEnemyClass->GetActorLocation();
		//myPos = playerDirection.RotateVector(myPos);//getting a vector of the opposite direction from the player
		FVector dir = playerDirection.Vector();
		if (dir.Equals(FVector(0, 0, 0)))
		{
			dir = FVector(1, 0, 0);
		}
		//UE_LOG(LogTemp, Warning, TEXT("vector of rotation after: %s"), *dir.ToString());
		float lengthDir = 1100.0f;
		//dir.ToDirectionAndLength(dir, lengthDir);
		dir *= lengthDir;
		UE_LOG(LogTemp, Error, TEXT("vector of rotation with length: %s"), *dir.ToString());
		UE_LOG(LogTemp, Warning, TEXT("old location: %s"), *myPos.ToString());
		FVector loc = myPos + dir;
		UE_LOG(LogTemp, Warning, TEXT("new location: %s"), *loc.ToString());

		while (!GetNavComponent()->FindPathToLocation(loc))
		{
			// TODO: This can get stuck in an infinite loop
			loc += FVector(1, 0, 0) + dir;
			UE_LOG(LogTemp, Warning, TEXT("Changed Locationg for running away"));
		}
		UE_LOG(LogTemp, Warning, TEXT("Found valid run away location"));

		EPathFollowingRequestResult::Type newLocPath = MoveToLocation(loc, 0.1f);//Moves AI to the location ***********

		if (newLocPath == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(LogTemp, Error, TEXT("Moving away from player: new location path: Failed"));
		}
		else if (newLocPath == EPathFollowingRequestResult::RequestSuccessful)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Moving away from player: new location path: Success"));
		}
		else if (newLocPath == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			//UE_LOG(LogTemp, Error, TEXT("Moving away from player: new location path: Already At Goal"));
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Moving away from player: new location path: IDK"));
		}
		// Drop an item

		//reverse player direction
		if (playerDirection.Yaw > 0)//positive
		{
			playerDirection.Yaw -= 180;
		}
		else if (playerDirection.Yaw <= 0)//negative
		{
			playerDirection.Yaw += 180;//chaning it to the opposite direction
		}
		dir = playerDirection.Vector();
		lengthDir = 200.0f;
		dir *= lengthDir;
		loc = myPos + dir;
		DropRandomItem(loc);
		// You killed my father
		AIEnemyClass->prepareToDie = true;
	//}
}

void AEnemyAIController::DropRandomItem(FVector loc)
{
	if (!AIEnemyClass->prepareToDie)
	{
		//might need to find random num here then input into method for spawn specific item
		GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnRandomItemAtLocation(loc);
	}
	if (Role < ROLE_Authority)
	{
		ServerDropRandomItem(loc);
	}
}

bool AEnemyAIController::ServerDropRandomItem_Validate(FVector loc)
{
	return true;
}

void AEnemyAIController::ServerDropRandomItem_Implementation(FVector loc)
{
	DropRandomItem(loc);
}

void AEnemyAIController::ReactToPlayer()
{

}

void AEnemyAIController::Despawn()
{

}