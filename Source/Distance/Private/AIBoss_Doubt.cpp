// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"

AAIBoss_Doubt::AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;//TODO: will change
	baseDamage = -1.0f;//TODO: will change along with other vars here
	drainRate = 0.25f;
	tentacleHealth = 10.0f;
	numTentacles = 5;

	swallowedPlayer = NULL;

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
	AITriggerAttack->SetBoxExtent(FVector(150.0f, 100.0f, 60.0f), true);
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
					if (closestPlayer == player1)
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
				//if swallowedPlayer == player -> then currentPlayer needs to be other player
				if (swallowedPlayer != NULL)
				{
					if (swallowedPlayer == player)
					{
						printScreen(FColor::Red, "Resetting Target, one player has been swallowed");
						if (player == player1)
						{
							currentPlayer = player2;
							player = Cast<ADistanceCharacter>(currentPlayer);
							playerController = Cast<ADistancePlayerController>(player->GetController());
						}
						else
						{
							currentPlayer = player1;
							player = Cast<ADistanceCharacter>(currentPlayer);
							playerController = Cast<ADistancePlayerController>(player->GetController());
						}
						StartAttackTimer(3.0f);//attack the new target
					}
				}
			}
		}

		//BEGIN TERRIBLENESS FOR PLAYER DEALING DAMAGE TO TENTACLE
		player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
		if (player1 != NULL)//Temporary Check for player dealing damage to boss*********************************************
		{
			class ADistancePlayerController* tPController1 = Cast<ADistancePlayerController>(player1->GetController());
			if (tPController1->attackBoss)//Players may use the '1' key/number on the keybord to attack
			{
				printScreen(FColor::Red, "Player1 Dealt Damage!!!!!!!!!!!!!!!!!!!!!!!!!");
				ChangeHealth(-5.0f);
				UE_LOG(LogTemp, Warning, TEXT("Boss health: %f, Tentacle Health: %f, Num of Tentacles: %d"), Health, tentacleHealth, numTentacles);
				tPController1->attackBoss = false;
			}
			if (tPController1->switchedItem)//shield on = when this is false
			{
				if (!tPController1->canMove)
				{
					printScreen(FColor::Red, "Player1 was Released by switching Items");
					ReleasePlayer(player1);
				}
				tPController1->switchedItem = false;
			}
		}
		if (player2 != NULL)
		{
			class ADistancePlayerController* tPController2 = Cast<ADistancePlayerController>(player2->GetController());
			if (tPController2->attackBoss)
			{
				printScreen(FColor::Red, "Player2 Dealt Damage!!!!!!!!!!!!!!!!!!!!!!!!!");
				ChangeHealth(-5.0f);
				UE_LOG(LogTemp, Warning, TEXT("Boss health: %f, Tentacle Health: %f, Num of Tentacles: %d"), Health, tentacleHealth, numTentacles);
				tPController2->attackBoss = false;
			}
			if (tPController2->switchedItem)
			{
				if (!tPController2->canMove)
				{
					printScreen(FColor::Red, "Player2 was Released by switching Items");
					ReleasePlayer(player2);
				}
				tPController2->switchedItem = false;
			}
		}
		if (player1 == NULL && player2 == NULL)
		{
			printScreen(FColor::Red, "What happened, Bad stuff!");
		}
		//END TERRIBLENESS
	}
}

void AAIBoss_Doubt::PullPlayer(class ACharacter* tempChar)
{
	if (tempChar == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: PullPlayer, tempChar is Null."));
		return;
	}

	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (tempPlayer->GetItemName() == "Default" && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)//TODO: change 'Default' to "Shield"
	{
		tempPlayer->ChangeSpeed(50);//shield is enabled
	}
	else
	{
		tempPlayer->ChangeSpeed(100);
	}

	tempPlayerController->canMove = false;
	tempPlayerController->SetNewMoveDestination(GetActorLocation());
}

void AAIBoss_Doubt::ReleasePlayer(class ACharacter* tempChar)
{
	if (tempChar == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: ReleasePlayer, tempChar is Null."));
		return;
	}

	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (!tempPlayerController->canMove)//if they were under ai control, stop moving them
	{
		tempPlayerController->SetNewMoveDestination(tempPlayer->GetActorLocation() - FVector(121.0f, 0.0f, 0.0f));//for stopping the last forced move
	}
	tempPlayerController->canMove = true;
	tempPlayer->ChangeSpeed(600);
	if (tempPlayer == swallowedPlayer)//if releasing swallowed player -> stop its timer, and reset swallowed player to null so we can get a current closest player again (might happen instantly, idk)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
		swallowedPlayer = NULL;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	//printScreen(FColor::Red, "Player was Released Normally");
}

void AAIBoss_Doubt::DrainPlayer(class ADistanceCharacter* tempPlayer)
{
	if (tempPlayer == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer, tempPlayer is Null."));
		return;
	}
	if (tempPlayer->GetItemName() == "Default" && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)//TODO: change 'Default' to "Shield"
	{
		tempPlayer->ChangeItemAmount(baseDamage);//shield is enabled
	}
	else
	{
		tempPlayer->ChangeHealth(baseDamage);
	}
}

void AAIBoss_Doubt::AttackTimer()
{
	printScreen(FColor::Red, "Boss making an attack");
	PullPlayer(player);
	StartDrainTimer(drainRate);
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
	DrainPlayer(player);
	if (player->Health == 0)//we "killed" the player, oops lol
	{
		ReleasePlayer(player);
	}
}

void AAIBoss_Doubt::StartDrainTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::DrainTimer, rate, true);
}

void AAIBoss_Doubt::SwallowedTimer()
{
	if (swallowedPlayer == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: SwallowedTimer, swallowedPlayer is Null."));
		return;
	}
	//printScreen(FColor::Red, "Draining Swallowed Player");
	DrainPlayer(swallowedPlayer);
	if (swallowedPlayer->Health == 0)//we "killed" the player, oops lol
	{
		ReleasePlayer(swallowedPlayer);
	}
}

void AAIBoss_Doubt::StartSwallowedTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::SwallowedTimer, rate, true);
}

void AAIBoss_Doubt::EndOfBoss()
{
	if (swallowedPlayer != NULL)
	{
		ReleasePlayer(swallowedPlayer);
	}
	if (!playerController->canMove)
	{
		ReleasePlayer(player);
		player = NULL;
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	printScreen(FColor::Red, "End of Boss");
}

void AAIBoss_Doubt::ChangeHealth(float healthAmount)//does damage to the boss or it's tentacles if it has any
{//TODO: something weird is happening where if player1 does damage its fine, but then if player2 does damage, its as if the boss has full health again, and when the boss has 0 health it still does stuff
	if (!playerController->canMove && numTentacles > 0)//if a player is grabbed and tentacle has health, deal the damage to the tentacle
	{
		float tempTentacleHealth = tentacleHealth + healthAmount;
		if (tempTentacleHealth <= 0)//defeated current tentacle
		{
			tentacleHealth = 0;
			//begin extra boss dmg-----
			float tempHealth = Health - 10;//since we defeated a tentacle, we do small amount of damage to boss too!
			if (tempHealth <= MaxHealth)
			{
				if (tempHealth < 0)
				{
					Health = 0.0f;//Defeated boss!
					EndOfBoss();
				}
				else
				{
					Health = tempHealth;
				}
			}
			//end extra boss dmg-----
			ReleasePlayer(player);
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
				EndOfBoss();
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
			if (!playerController->canMove && swallowedPlayer == NULL)//Can't move, therefore, i've dragged them in, and swallowed them
			{
				//set swallowed player
				printScreen(FColor::Red, "Beginning Draining Swallowed Player.");
				swallowedPlayer = player;
				StartSwallowedTimer(drainRate / 1.5f);//drainRate = 0.25f normally
			}
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