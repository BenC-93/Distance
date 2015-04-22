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

	void AIMoveToPlayer(class ACharacter* player);

	void AIMoveAwayFromPlayer(class ACharacter* player);

	void ReactToPlayer();
	
	void DropRandomItem(FVector loc);

	void Despawn();

	virtual void Possess(class APawn *InPawn) override;

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropRandomItem(FVector loc);
	//virtual bool ServerDropRandomItem_Validate(FVector loc);
	//virtual void ServerDropRandomItem_Implementation(FVector loc);

	UPROPERTY(transient)
	class AAIEnemy* AIEnemyClass;

	FVector PlayerLoc;
};
