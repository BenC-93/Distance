// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	moveToPlayer = false;
	moveAway = false;
	drainHealth = false;
	drainLight = false;
	drainTrigger = false;
	player1 = NULL;
	player2 = NULL;

	health = 0.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	drainCounter = 0.0f;
	drainRate = 0.1f;// tenth of a second

	speedCounter = 1.0f;
	speedLimit = 1200.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITriggerRange = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerRange"));
	AITriggerRange->Mobility = EComponentMobility::Movable;
	AITriggerRange->SetBoxExtent(FVector(750.0f, 750.0f, 60.0f), true);
	AITriggerRange->AttachTo(RootComponent);

	AITriggerRange->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBegin);
	AITriggerRange->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEnd);

	AITriggerAttack = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerAttack"));
	AITriggerAttack->Mobility = EComponentMobility::Movable;
	AITriggerAttack->SetBoxExtent(FVector(300.0f,300.0f, 60.0f), true);
	AITriggerAttack->AttachTo(RootComponent);

	//AITriggerAttack->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBeginAttack);
	//AITriggerAttack->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEndAttack);
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Delta Time, %f"), DeltaTime);
	if (player != NULL)
	{
		// Player is in range of close trigger and Enemy is not "full"
		if (drainTrigger && health < maxHealth)
		{
			// Add time to drain tick
			drainCounter += DeltaTime;
			if (player->getLightAmount() > 0 && player->getLightEnabled())
			{
				drainLight = true;
				drainHealth = false;
			}
			else//drain health
			{
				drainHealth = true;
				drainLight = false;
			}
		}

		if (drainHealth && !drainLight && !moveAway)//drain health
		{
			if (health < maxHealth && drainCounter > drainRate)
			{
				// Tick the drain
				drainCounter -= drainRate;
				health += 1;
				player->ChangeHealth(-1.0f);
				if (player->Health == 0)//health has depleted
				{
					GetCharacterMovement()->MaxWalkSpeed = 400;
					//drainCounter = 0;
					drainHealth = false;
					moveAway = true;
					moveToPlayer = false;
				}
				UE_LOG(LogTemp, Warning, TEXT("Health decremented, %f"), player->Health);
			}
			else if (health >= maxHealth)//AI is full
			{
				GetCharacterMovement()->MaxWalkSpeed = 400;
				//drainCounter = 0;
				drainHealth = false;
				moveAway = true;
				moveToPlayer = false;
			}
			//drainCounter++;//need to comment this out
		}
		else if (drainLight && !drainHealth && !moveAway)//drain light
		{
			if (health < maxHealth && drainCounter > drainRate)
			{
				// Tick the drain
				drainCounter -= drainRate;
				health += 1;
				player->ChangeLight(-1.0f);
				if (player->getLightAmount() == 0)//dont end here, just go to health
				{
					drainLight = false;
					drainHealth = true;
					/*
					GetCharacterMovement()->MaxWalkSpeed = 400;
					//drainCounter = 0;
					drainLight = false;
					moveAway = true;
					moveToPlayer = false;
					*/
				}
				UE_LOG(LogTemp, Warning, TEXT("Light decremented, %f"), player->getLightAmount());
			}
			else if (health >= maxHealth)//Ai is full
			{
				GetCharacterMovement()->MaxWalkSpeed = 400;
				//drainCounter = 0;
				drainLight = false;
				moveAway = true;
				moveToPlayer = false;
			}
			//drainCounter++;//need to comment this out
		}

		if (moveToPlayer && !drainTrigger)
		{
			FRotator playerDirection = player->GetVelocity().Rotation();
			FRotator myDirection = GetVelocity().Rotation();
			FRotator playerRevDir = playerDirection;
			FRotator myRevDir = myDirection;
			if (playerRevDir.Yaw > 0)//positive
			{
				playerRevDir.Yaw -= 180;
			}
			else if (playerRevDir.Yaw <= 0)//negative
			{
				playerRevDir.Yaw += 180;
			}
			if (myRevDir.Yaw > 0)//positive
			{
				myRevDir.Yaw -= 180;
			}
			else if (myRevDir.Yaw <= 0)//negative
			{
				myRevDir.Yaw += 180;
			}

			float range = 10.0f;
			if (myDirection.Yaw > playerDirection.Yaw - range && myDirection.Yaw < playerDirection.Yaw + range)
			{//this is where if light is enabled, player should be slowly moving faster and killing the monster
				//UE_LOG(LogTemp, Warning, TEXT("Same direction"));//speed up ai
				float aiSpeed = GetCharacterMovement()->MaxWalkSpeed;
				if (aiSpeed < speedLimit)
				{
					GetCharacterMovement()->MaxWalkSpeed = aiSpeed + speedCounter;
					//UE_LOG(LogTemp, Warning, TEXT("AI speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
				}
			}
			else if (myDirection.Yaw > playerRevDir.Yaw - range && myDirection.Yaw < playerRevDir.Yaw + range)
			{//this is where if light is not enabled (maybe light doesnt matter, dont know), but player slowly moves slower until he cannot escape
				//UE_LOG(LogTemp, Warning, TEXT("Opposite direction"));
				GetCharacterMovement()->MaxWalkSpeed = 400;
				speedCounter = 1.0f;
				moveToPlayer = false;
				moveAway = true;
			}
			else
			{//not needed, just for testing purposes
				//UE_LOG(LogTemp, Warning, TEXT("some other direction"));
			}
			//UE_LOG(LogTemp, Warning, TEXT("AI rotation: %s"), *myDirection.ToString());
			speedCounter += 2;// *DeltaTime;// (speedCounter + 1.0f);
			if (speedCounter > speedLimit - GetCharacterMovement()->MaxWalkSpeed)
			{
				speedCounter = speedLimit - GetCharacterMovement()->MaxWalkSpeed;
			}
			//UE_LOG(LogTemp, Warning, TEXT("speedCounter: %f"), speedCounter);
		}
	}


}

void AAIEnemy::ChangeHealth(float amount)
{

}

void AAIEnemy::Attack(float amount)
{

}

void AAIEnemy::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("-----Entered Triggered Area"));
			moveToPlayer = true;//outer trigger boolean
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			/*if (player->getLightAmount() > 0 && player->getLightEnabled())//This is where the enemy is supposed to run away
			{
				moveToPlayer = false;
				moveAway = true;
				UE_LOG(LogTemp, Warning, TEXT("Moving away from Player"));
			}*/
		}
	}
}

void AAIEnemy::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("-------Exited Triggered Area fool!"));
			// start player health regeneration
			player = Cast<ADistanceCharacter>(OtherActor);
			if (player != NULL)
			{
				player->StartRegeneration();
				UE_LOG(LogTemp, Warning, TEXT("Player health regeneration started"));
			}
		}
	}
}

void AAIEnemy::OnAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Entered drain trigger"));
			//UE_LOG(LogTemp, Warning, TEXT("Slowed Player1 and beginning drain"));
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);
			player->ChangeSpeed(400);//Works, but when do we set it back to normal??
			//GetWorldTimerManager().SetTimer(this, &AAIEnemy::Drain, 1.0f, true);

			if (player->getLightAmount() > 0 && player->getLightEnabled())
			{
				drainLight = true;
				drainHealth = false;
			}
			else
			{
				drainHealth = true;
				drainLight = false;
			}
			drainTrigger = true;
		}
	}
}

void AAIEnemy::OnExitAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Exited drain trigger."));
			drainTrigger = false;
			drainHealth = false;
			drainLight = false;
		}
	}
}

bool AAIEnemy::CheckIfPlayer(class AActor* OtherActor)//TODO: not 100% positive im checking this correctly
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	//UE_LOG(LogTemp, Warning, TEXT("Player actually is: %s"), *player1->GetActorLabel());// these are messages for checking if we collide with the player or not
	//UE_LOG(LogTemp, Warning, TEXT("Player checked is: %s"), *OtherActor->GetActorLabel());
	//UE_LOG(LogTemp, Warning, TEXT("Are they the same?: %d"), ((player1 == OtherActor)));// ? "Yes" : "No"));
	if (player1 != NULL && player1 == OtherActor)//player1->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)//player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	return false;
}

