// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"

AAIBoss_Doubt::AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;//TODO: will change
	baseDamage = -1.0f;//TODO: will change along with other vars here
	tentacleHealth = 10.0f;
	numTentacles = 5;

	p1InTrigger = false;
	p2InTrigger = false;

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
		if (Health > 0)
		{
			if (p1InTrigger && p2InTrigger)//checks and sets the current player target to the closest player
			{
				ACharacter *closestPlayer = ClosestPlayer();
				if (closestPlayer != player)
				{
					if (ClosestPlayer() == player1)
					{
						currentPlayer = player1;
						player = Cast<ADistanceCharacter>(currentPlayer);
						playerController = Cast<ADistancePlayerController>(player->GetController());
						//printScreen(FColor::Red, "Boss targeting: Player1");
					}
					else
					{
						currentPlayer = player2;
						player = Cast<ADistanceCharacter>(currentPlayer);
						playerController = Cast<ADistancePlayerController>(player->GetController());
						//printScreen(FColor::Red, "Boss targeting: Player2");
					}
				}
			}
		}

		//BEGIN TERRIBLENESS FOR PLAYER DEALING DAMAGE TO TENTACLE
		player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
		if (player1 != NULL)//Temporary Check for player dealing damage to boss*********************************************
		{
			if (Cast<ADistancePlayerController>(player1->GetController())->attackBoss)//Players may use the '1' key/number on the keybord to attack
			{
				printScreen(FColor::Red, "Player1 Dealt Damage!!!!!!!!!!!!!!!!!!!!!!!!!");
				ChangeHealth(-5.0f);
				UE_LOG(LogTemp, Warning, TEXT("Boss health: %f, Tentacle Health: %f, Num of Tentacles: %d"), Health, tentacleHealth, numTentacles);
				Cast<ADistancePlayerController>(player1->GetController())->attackBoss = false;
			}
		}
		if (player2 != NULL)
		{
			if (Cast<ADistancePlayerController>(player2->GetController())->attackBoss)
			{
				printScreen(FColor::Red, "Player2 Dealt Damage!!!!!!!!!!!!!!!!!!!!!!!!!");
				ChangeHealth(-5.0f);
				UE_LOG(LogTemp, Warning, TEXT("Boss health: %f, Tentacle Health: %f, Num of Tentacles: %d"), Health, tentacleHealth, numTentacles);
				Cast<ADistancePlayerController>(player2->GetController())->attackBoss = false;
			}
		}
		if (player1 == NULL && player2 == NULL)
		{
			printScreen(FColor::Red, "What happened, Bad stuff!");
		}
		//END TERRIBLENESS
	}
}

void AAIBoss_Doubt::PullPlayer()
{
	player->ChangeSpeed(100);
	playerController->canMove = false;
	playerController->SetNewMoveDestination(GetActorLocation());
}

void AAIBoss_Doubt::ReleasePlayer()
{
	if (!playerController->canMove)//if they were under ai control, stop moving them
	{
		playerController->SetNewMoveDestination(player->GetActorLocation() - FVector(121.0f, 0.0f, 0.0f));//for stopping the last forced move
	}
	playerController->canMove = true;
	player->ChangeSpeed(600);
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
}

void AAIBoss_Doubt::DrainPlayer()
{
	if (player->getLightEnabled() && player->getLightAmount() > 0)
	{
		player->ChangeLight(baseDamage);
	}
	else
	{
		player->ChangeHealth(baseDamage);
	}
}

void AAIBoss_Doubt::AttackTimer()
{
	printScreen(FColor::Red, "Boss making an attack");
	PullPlayer();
	StartDrainTimer(0.25f);
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
}

void AAIBoss_Doubt::StartAttackTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::AttackTimer, rate, true);
}

void AAIBoss_Doubt::DrainTimer()
{
	//printScreen(FColor::Red, "Draining");
	DrainPlayer();
	if (player->Health == 0)//we "killed" the player, oops lol
	{
		ReleasePlayer();
	}
}

void AAIBoss_Doubt::StartDrainTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::DrainTimer, rate, true);
}

void AAIBoss_Doubt::ChangeHealth(float healthAmount)//does damage to the boss or it's tentacles if it has any
{
	if (!playerController->canMove && numTentacles > 0)//if a player is grabbed and tentacle has health, deal the damage to the tentacle
	{
		float tempTentacleHealth = tentacleHealth + healthAmount;
		if (tempTentacleHealth <= 0)//defeated current tentacle
		{
			tentacleHealth = 0;
			float tempHealth = Health - 10;//since we defeated a tentacle, we do small amount of damage to boss too!
			if (tempHealth <= MaxHealth)
			{
				if (tempHealth < 0)
				{
					Health = 0.0f;//Defeated boss!
				}
				else
				{
					Health = tempHealth;
				}
			}
			ReleasePlayer();
			numTentacles--;
			if (numTentacles > 0)//there are still tentacles left 
			{
				tentacleHealth = 10;//reset tentacle health
				StartAttackTimer(3.0f);
			}
		}
		else //tentacle still has health
		{
			tentacleHealth = tempTentacleHealth;
		}
	}
	else if (playerController->canMove)//else if no player is grabbed, deal damage to the actual boss
	{
		float tempHealth = Health + healthAmount;
		if (tempHealth <= MaxHealth)
		{
			if (tempHealth < 0)
			{
				Health = 0.0f;//Defeated boss!
				ReleasePlayer();
			}
			else
			{
				Health = tempHealth;
			}
		}
	}
}

float AAIBoss_Doubt::GetHealth()
{
	return Health;
}

float AAIBoss_Doubt::GetMaxTentacleHealth()
{
	return tentacleHealth;
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
			playerController = Cast<ADistancePlayerController>(player->GetController());
			StartAttackTimer(3.0f);
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