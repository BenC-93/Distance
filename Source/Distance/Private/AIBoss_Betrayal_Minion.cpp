// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Betrayal.h"
#include "AIBoss_Betrayal_Minion.h"
#include "DistanceCharacter.h"
#include "DItem.h"

// items pls
#include "ItemLantern.h"
#include "ItemLightBeam.h"
#include "ItemShield.h"
#include "ItemCrystal.h"
#include "ItemDagger.h"
// no more items pls

AAIBoss_Betrayal_Minion::AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemSocket = TEXT("ItemSocket");

	CurrentItem = NULL;
	PlayerItem = NULL;
	CurrDirection = MinionOrientation::LEFT;

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
			if (PlayerItem != player->CurrentItem)
			{
				PlayerItem = player->CurrentItem;
				EquipCorrectClassItem(PlayerItem);
			}

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
					FlipForDirection(1.f);
				}
				else if (temp.Y < 0.f)
				{
					FlipForDirection(-1.f);
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

void AAIBoss_Betrayal_Minion::EquipCorrectClassItem(ADItem *blah)
{
	if (CurrentItem != NULL)
	{
		CurrentItem->Destroy();
	}
	if (blah->IsA(AItemLantern::StaticClass()))
	{
		CurrentItem = GetWorld()->SpawnActor<AItemLantern>(LanternClass);
	}
	else if (blah->IsA(AItemLightBeam::StaticClass()))
	{
		CurrentItem = GetWorld()->SpawnActor<AItemLightBeam>(StaffClass);
	}
	else if (blah->IsA(AItemShield::StaticClass()))
	{
		CurrentItem = GetWorld()->SpawnActor<AItemShield>(ShieldClass);
	}
	else if (blah->IsA(AItemCrystal::StaticClass()))
	{
		CurrentItem = GetWorld()->SpawnActor<AItemCrystal>(CrystalClass);
	}
	else if (blah->IsA(AItemDagger::StaticClass()))
	{
		CurrentItem = GetWorld()->SpawnActor<AItemDagger>(SwordClass);
	}
	CurrentItem->GetRootComponent()->AttachTo(GetMesh(), FName("ItemSocket"));
	FlipForDirection(0.0f, true);
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
		CurrentItem->Destroy();
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
		FlipForDirection(1.f);
	}
	else if (TargetActor->GetVelocity().Y < 0.f)
	{
		FlipForDirection(-1.f);
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
	TargetLocation = GetActorLocation() + temp;
	TargetSpeed = FMath::RandRange(100.f, speedRange);
	currentSpeed = TargetSpeed;
}

void AAIBoss_Betrayal_Minion::StartMoveRandomTimer()
{
	float timeTilMove = FMath::FRandRange(1.f, 5.f);
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

FName AAIBoss_Betrayal_Minion::GetItemAttachPoint()
{
	return ItemSocket;
}

void AAIBoss_Betrayal_Minion::FlipForDirection(float MovementInputDirection, bool bUseCurrentDirection)
{
	bool bShouldRefresh = false;
	MinionOrientation direction;

	if (bUseCurrentDirection)
	{
		// MovementInputDirection is not useful, we just want to refresh the flippiness
		bShouldRefresh = true;
		direction = CurrDirection;
	}
	else if (MovementInputDirection > 0.0f)
	{
		direction = MinionOrientation::RIGHT;
		bShouldRefresh = (direction != CurrDirection);
	}
	else if (MovementInputDirection < 0.0f)
	{
		direction = MinionOrientation::LEFT;
		bShouldRefresh = (direction != CurrDirection);
	}

	if (bShouldRefresh)
	{
		CurrDirection = direction;
		// Pitch yaw and roll got switched up because the model itself is rotated now
		// Previously, we rotated the model in engine after import
		float roll = 0.0f;
		float pitch = DEFAULT_SPRITE_ROLL; // negate to flip
		float yaw = 0.0f; // Rotate by 180 to flip

		// Flips which side of the model is facing the camera
		float xScale = 1.0f;

		if (CurrDirection == MinionOrientation::LEFT)
		{
			// Don't scale, this is the natural orientation of the model
		}
		else if (CurrDirection == MinionOrientation::RIGHT)
		{
			// Rotate the character model
			pitch = -DEFAULT_SPRITE_ROLL;
			yaw = 180.0f;
			// Flip the character so the back side of the model doesn't show
			xScale = -1.0f;
		}
		GetMesh()->SetRelativeRotation(FRotator(pitch, yaw, roll));
		GetMesh()->SetRelativeScale3D(FVector(xScale, 1.0f, 1.0f));
		// Also scale the item in the X direction and potentially do other item specific transformation hacks
		if (CurrentItem != NULL) { CurrentItem->FlipForDirection(xScale); }
	}
}