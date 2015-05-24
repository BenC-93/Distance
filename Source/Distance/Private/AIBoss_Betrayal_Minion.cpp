// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Betrayal.h"
#include "AIBoss_Betrayal_Minion.h"
#include "DistanceCharacter.h"
#include "DItem.h"

AAIBoss_Betrayal_Minion::AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);
	SpriteComponent->AttachTo(RootComponent);

	HeldItem = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("HeldItem"));
	HeldItem->AttachTo(RootComponent);

	Health = MaxHealth = 10.f;
	TargetActor = this;
	TargetLocation = GetActorLocation();
	TargetSpeed = 10.f;
	CopyLength = FVector(0.f, 0.f, 0.f);
	ActiveMoveState = STATIC;
	OwningPawn = NULL;
	currentSpeed = 0.0f;
	canMove = true;
}

void AAIBoss_Betrayal_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetActorLocation() != TargetLocation)
	{
		if (TargetActor != NULL && TargetActor != this)
		{
			ADistanceCharacter* player = Cast<ADistanceCharacter>(TargetActor);
			// TODO: this isn't going to work
//			HeldItem->SetSprite(player->GetItem()->SpriteComponent->GetSprite());
			HeldItem->SetRelativeRotation(FRotator(0.f, 90.f, -65.f));
		}

		if (TargetSpeed <= 0.f && canMove)
		{
			SetActorLocation(TargetLocation);
		}
		else
		{
			FVector dist = GetActorLocation() - TargetLocation;
			if (dist.Size() > 5.f)
			{
				FVector temp = TargetLocation - GetActorLocation();
				temp.Normalize();
				if (temp.Y > 0.f)
				{
					GetMesh()->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
				}
				else if (temp.Y < 0.f)
				{
					GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
				}
				temp = (DeltaTime * temp * TargetSpeed) + GetActorLocation();
				if (ActiveMoveState == FOLLOW)
				{
					if (GetDistanceTo(TargetActor) > 150.f && canMove)
					{
						SetActorLocation(temp);
						currentSpeed = TargetSpeed;
					}
					else
					{
						currentSpeed = 0.0f;
					}
				}
				else
				{
					if (canMove)
					{
						SetActorLocation(temp);
					}
				}
			}
			else
			{
				if (canMove)
				{
					SetActorLocation(TargetLocation);
				}
			}
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

FVector AAIBoss_Betrayal_Minion::GetVelocity() const
{
	return FVector(currentSpeed, 0.0f, 0.0f);
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
	UE_LOG(LogTemp, Warning, TEXT("Minion health: %f"), Health);
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

	StartAttackRandomTimer();
}

MoveState AAIBoss_Betrayal_Minion::GetMoveState()
{
	return ActiveMoveState;
}

void AAIBoss_Betrayal_Minion::MoveStatic()
{
	TargetLocation = GetActorLocation();
	TargetSpeed = -1.f;
	currentSpeed = 0.0f;
}

void AAIBoss_Betrayal_Minion::MoveCopy()
{
	TargetLocation = TargetActor->GetActorLocation() + (150 * CopyLength);
	TargetSpeed = -1.f;
	currentSpeed = TargetActor->GetVelocity().Size();
	if (TargetActor->GetVelocity().Y > 0.f)
	{
		GetMesh()->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
	}
	else if (TargetActor->GetVelocity().Y < 0.f)
	{
		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
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
	//temp.Normalize();
	TargetLocation = GetActorLocation() + temp;
	TargetSpeed = FMath::RandRange(100.f, speedRange);
	currentSpeed = TargetSpeed;
}

void AAIBoss_Betrayal_Minion::StartMoveRandomTimer()
{
	float timeTilMove = FMath::FRandRange(1.f, 5.f); //add a range to this func call, then uncomment
	currentSpeed = 0.0f;
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal_Minion::MoveRandom, timeTilMove, true);
}

//~~~ Owning Monster ~~~//

void AAIBoss_Betrayal_Minion::SetOwner(class AAIBoss_Betrayal* b)
{
	OwningPawn = b;
}

void AAIBoss_Betrayal_Minion::AnimationTimer()
{
	canMove = true;
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	StartAttackRandomTimer();
}

void AAIBoss_Betrayal_Minion::AttackTimer()
{
	canMove = false;
	if (AttackAnimation)
	{
		GetMesh()->PlayAnimation(AttackAnimation, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Minion Attack timer, attack animation is Null."));
	}
	currentSpeed = 0.0f;
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal_Minion::AnimationTimer, 0.85f, false);
}

void AAIBoss_Betrayal_Minion::StartAttackRandomTimer()
{
	float timeTilAttack = FMath::FRandRange(2.f, 10.f);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal_Minion::AttackTimer, timeTilAttack, false);
}