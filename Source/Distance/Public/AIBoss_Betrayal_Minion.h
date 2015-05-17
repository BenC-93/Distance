// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AIBoss_Betrayal_Minion.generated.h"

enum MoveState { STATIC, COPY, FOLLOW, RANDOM };

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIBoss_Betrayal_Minion : public ACharacter
{
	GENERATED_BODY()
	
public:
	AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	virtual void Tick(float DeltaTime) override;

	//~~~ Health ~~~//
	float Health;
	float MaxHealth;

	void ChangeHealth(float h);
	float GetHealth();
	float GetMaxHealth();
	
	//~~~ Targeting  ~~~//
	AActor* TargetActor;
	FVector TargetLocation;
	float TargetSpeed;

	FVector CopyLength;

	void SetTargetActor(AActor* t);
	AActor* GetTargetActor();

	//~~~ Move states(States) and functions ~~~//
	MoveState ActiveMoveState;

	void SetMoveState(MoveState m, ACharacter* c);
	MoveState GetMoveState();

	void MoveStatic();
	void MoveCopy();
	void MoveFollow();
	void MoveRandom();

	void StartMoveRandomTimer();

	//~~~ Owning Monster ~~~//
	class AAIBoss_Betrayal* OwningPawn;

	void SetOwner(class AAIBoss_Betrayal* b);
};
