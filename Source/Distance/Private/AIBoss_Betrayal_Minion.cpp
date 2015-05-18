// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Betrayal.h"
#include "AIBoss_Betrayal_Minion.h"


AAIBoss_Betrayal_Minion::AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);
	SpriteComponent->AttachTo(RootComponent);

	Health = MaxHealth = 15.f;
	TargetActor = this;
	TargetLocation = GetActorLocation();
	TargetSpeed = 10.f;
	CopyLength = FVector(0.f, 0.f, 0.f);
	ActiveMoveState = STATIC;
	OwningPawn = NULL;
}

void AAIBoss_Betrayal_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetActorLocation() != TargetLocation)
	{
		if (TargetSpeed <= 0.f)
		{
			SetActorLocation(TargetLocation);
		}
		else
		{
			FVector temp = TargetLocation - GetActorLocation();
			temp.Normalize();
			temp = (DeltaTime * temp * TargetSpeed) + GetActorLocation();
			SetActorLocation(temp);
		}
	}

	if (ActiveMoveState == COPY)
	{
		MoveCopy();
		return;
	}
	else if (ActiveMoveState == FOLLOW)
	{
		MoveFollow();
		return;
	}
}

//~~~ Health ~~~//

void AAIBoss_Betrayal_Minion::ChangeHealth(float h)
{
	float temp = GetHealth() + h;
	if (temp <= 0.f)
	{
		OwningPawn->NotifyDeath();
		Destroy();
	}
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

void AAIBoss_Betrayal_Minion::SetMoveState(MoveState m, ACharacter* c)
{
	if (ActiveMoveState == RANDOM)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Betrayal_Minion::MoveRandom);
	}

	ActiveMoveState = m;
	if (c == NULL)
	{
		SetTargetActor(this);
	}
	else
	{
		SetTargetActor(c);
	}

	switch (ActiveMoveState)
	{
	case STATIC: MoveStatic(); break;
	case COPY: CopyLength = TargetActor->GetActorLocation() - GetActorLocation(); CopyLength.Normalize(); MoveCopy(); break;
	case FOLLOW: MoveFollow(); break;
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
	TargetLocation = GetActorLocation();
	TargetSpeed = -1.f;
}

void AAIBoss_Betrayal_Minion::MoveCopy()
{
	TargetLocation = TargetActor->GetActorLocation() + (150 * CopyLength);
	TargetSpeed = -1.f;
}

void AAIBoss_Betrayal_Minion::MoveFollow()
{
	TargetLocation = TargetActor->GetActorLocation();
	TargetSpeed = 300.f;
}

void AAIBoss_Betrayal_Minion::MoveRandom()
{
	//set TargetLocation to a random location nearby
	float distRange = 300.f;
	float speedRange = 300.f;
	FVector temp = FVector(FMath::FRandRange(-distRange, distRange), FMath::FRandRange(-distRange, distRange), 0.f);
	temp.Normalize();
	TargetLocation = GetActorLocation() + temp;
	TargetSpeed = FMath::RandRange(100.f, speedRange);
}

void AAIBoss_Betrayal_Minion::StartMoveRandomTimer()
{
	float timeTilMove = FMath::FRandRange(1.f, 5.f); //add a range to this func call, then uncomment
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal_Minion::MoveRandom, timeTilMove, true);
}

//~~~ Owning Monster ~~~//

void AAIBoss_Betrayal_Minion::SetOwner(class AAIBoss_Betrayal* b)
{
	OwningPawn = b;
}