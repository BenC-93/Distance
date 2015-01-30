// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "AIEnemy.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	void ReactToPlayer();

	void Despawn();

	virtual void Possess(class APawn *InPawn) override;

private:

	UPROPERTY(transient)
	class AAIEnemy* AIEnemyClass;

	FVector PlayerLoc;
};
