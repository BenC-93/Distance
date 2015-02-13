// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"

AAIBoss_Doubt::AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 100.0f;
	maxHealth = 100.0f;//TODO: will change
	baseDamage = 20.0f;//TODO: will change

	p1InTrigger = false;
	p2InTrigger = false;

	attackInterval = -1;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITriggerRange = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerRange"));
	AITriggerRange->Mobility = EComponentMobility::Movable;
	AITriggerRange->SetBoxExtent(FVector(750.0f, 750.0f, 60.0f), true);
	AITriggerRange->AttachTo(RootComponent);

	AITriggerRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBoss_Doubt::OnOverlapBegin);
	AITriggerRange->OnComponentEndOverlap.AddDynamic(this, &AAIBoss_Doubt::OnOverlapEnd);

	AITriggerAttack = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerAttack"));
	AITriggerAttack->Mobility = EComponentMobility::Movable;
	AITriggerAttack->SetBoxExtent(FVector(300.0f, 300.0f, 60.0f), true);
	AITriggerAttack->AttachTo(RootComponent);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
}

void AAIBoss_Doubt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (player != NULL)
	{
		if (health > 0)
		{
			if (p1InTrigger && p2InTrigger)//checks and sets the current player target to the closest player
			{
				if (ClosestPlayer() == player1)
				{
					currentPlayer = player1;
					player = Cast<ADistanceCharacter>(currentPlayer);
					if (attackInterval < 0)
					{
						StartAttackTimer();
					}
					//printScreen(FColor::Red, "Boss targeting: Player1");
				}
				else
				{
					currentPlayer = player2;
					player = Cast<ADistanceCharacter>(currentPlayer);
					if (attackInterval < 0)
					{
						StartAttackTimer();
					}
					//printScreen(FColor::Red, "Boss targeting: Player2");
				}
			}

			if (attackInterval == 0)//grabbed player
			{
				attackInterval = -1;//make sure this if statement only happens once
				printScreen(FColor::Red, "Boss making an attack");
				//PullPlayer();
			}
		}
	}
}

void AAIBoss_Doubt::PullPlayer()
{
	//stop player input to movement
	//player->GetController()->ClientSetLocation
}

void AAIBoss_Doubt::AttackTimer()
{
	if (attackInterval <= 0)
	{
		attackInterval = -1;
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	}
	attackInterval -= 1;
}

void AAIBoss_Doubt::StartAttackTimer()
{
	attackInterval = 3;//initialization of counter in seconds to countdown
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::AttackTimer, 1.0f, true);
}

void AAIBoss_Doubt::ChangeHealth(float amount)
{

}

void AAIBoss_Doubt::Attack(float amount)
{

}

ACharacter* AAIBoss_Doubt::ClosestPlayer()//assuming player 1 and player 2
{
	float p1Dist = AbsoluteVal((GetActorLocation() - player1->GetActorLocation()).Size());
	float p2Dist = AbsoluteVal((GetActorLocation() - player2->GetActorLocation()).Size());
	if (p1Dist < p2Dist)
	{
		return player1;
	}
	else
	{
		return player2;
	}
}

float AAIBoss_Doubt::AbsoluteVal(float amount)//fmath::abs didnt work so...
{
	if (amount < 0)
	{
		return 0 - amount;
	}
	return amount;
}

void AAIBoss_Doubt::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			if (player1 == OtherActor)
			{
				p1InTrigger = true;
			}
			else
			{
				p2InTrigger = true;
			}
			UE_LOG(LogTemp, Warning, TEXT("****Player Entered BOSS Triggered Area"));
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			if (attackInterval < 0)
			{
				StartAttackTimer();
			}
		}
	}
}

void AAIBoss_Doubt::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("****Player Exited BOSS Triggered Area"));
			if (player1 == OtherActor)
			{
				p1InTrigger = false;
			}
			else
			{
				p2InTrigger = false;
			}
		}
	}
}

void AAIBoss_Doubt::OnAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
		}
	}
}

void AAIBoss_Doubt::OnExitAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
		}
	}
}

bool AAIBoss_Doubt::CheckIfPlayer(class AActor* OtherActor)//TODO: not 100% positive im checking this correctly
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	//UE_LOG(LogTemp, Warning, TEXT("Player actually is: %s"), *player1->GetActorLabel());// these are messages for checking if we collide with the player or not
	//UE_LOG(LogTemp, Warning, TEXT("Player checked is: %s"), *OtherActor->GetActorLabel());
	//UE_LOG(LogTemp, Warning, TEXT("Are they the same?: %d"), ((player1 == OtherActor)));// ? "Yes" : "No"));
	if (player1 != NULL && player1 == OtherActor)//player1->GetActorLabel() == OtherActor->GetActorLabel())
	{
		//printScreen(FColor::Red, "Player 1 checked");
		//UE_LOG(LogTemp, Warning, TEXT("Player 1 checked"));
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)//player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		//printScreen(FColor::Red, "Player 2 checked");
		//UE_LOG(LogTemp, Warning, TEXT("Player 2 checked"));
		return true;
	}
	return false;
}