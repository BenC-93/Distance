// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Betrayal_Minion.h"


AAIBoss_Betrayal_Minion::AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);
	SpriteComponent->AttachTo(RootComponent);

	Health = MaxHealth = 100.f;
	TargetActor = NULL;
	TargetLocation = GetActorLocation();
	TargetSpeed = 10.f;
	ActiveMoveState = STATIC;
}

//~~~ Health ~~~//

void AAIBoss_Betrayal_Minion::ChangeHealth(float h)
{
	float temp = GetHealth() + h;
	Health = fmax(0.f, fmin(temp, GetMaxHealth()));
}

float AAIBoss_Betrayal_Minion::GetHealth()
{
	return Health;
}

float AAIBoss_Betrayal_Minion::GetMaxHealth()
{
	return MaxHealth;
}

//~~~ Targeting ~~~//

void AAIBoss_Betrayal_Minion::SetTargetActor(AActor* t)
{
	if (t != NULL) { TargetActor = t; }
}

AActor* AAIBoss_Betrayal_Minion::GetTargetActor()
{
	return TargetActor;
}

//~~~ Move states ~~~//

void AAIBoss_Betrayal_Minion::SetMoveState(MoveState m)
{
	if (ActiveMoveState == RANDOM)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Betrayal_Minion::MoveRandom);
	}
	ActiveMoveState = m;
	switch (ActiveMoveState)
	{
	case STATIC: MoveStatic(); break;
	case RANDOM: StartMoveRandomTimer(); break;
	default: break;
	}
}

MoveState AAIBoss_Betrayal_Minion::GetMoveState()
{
	return ActiveMoveState;
}

void AAIBoss_Betrayal_Minion::MoveStatic()
{
	//set the TargetLocation to the current location
	//set TargetSpeed to 0
}

void AAIBoss_Betrayal_Minion::MoveCopy()
{
	//set velocities equal to TargetActor's velocities
}

void AAIBoss_Betrayal_Minion::MoveFollow()
{
	//set TargetLocation to TargetActor's location
	//set a TargetSpeed
}

void AAIBoss_Betrayal_Minion::MoveRandom()
{
	//set TargetLocation to a random location nearby
	//set a random TargetSpeed
}

void AAIBoss_Betrayal_Minion::StartMoveRandomTimer()
{
	float timeTilMove = 10.f; //= FMath::FRandRange(); add a range to this func call, then uncomment
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal_Minion::MoveRandom, timeTilMove, true);
}