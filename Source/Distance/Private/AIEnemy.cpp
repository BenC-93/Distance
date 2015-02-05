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
	player1 = NULL;
	player2 = NULL;

	health = 100.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	drainCounter = 0; 
	drainRate = 3;//must be greater than 0

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
	if (player != NULL)
	{
		if (drainHealth)
		{
			if (drainCounter < 100 * drainRate && drainCounter % drainRate == 0)
			{
				player->ChangeHealth(-1.0f);
				if (player->Health == 0)
				{
					drainHealth = false;
					moveAway = true;
					moveToPlayer = false;
				}
				UE_LOG(LogTemp, Warning, TEXT("Health decremented, %f"), player->Health);
			}
			drainCounter++;
		}
		if (moveToPlayer)
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
				UE_LOG(LogTemp, Warning, TEXT("Same direction"));
			}
			else if (myDirection.Yaw > playerRevDir.Yaw - range && myDirection.Yaw < playerRevDir.Yaw + range)
			{//this is where if light is not enabled (maybe light doesnt matter, dont know), but player slowly moves slower until he cannot escape
				UE_LOG(LogTemp, Warning, TEXT("Opposite direction"));
			}
			else
			{//not needed, just for testing purposes
				//UE_LOG(LogTemp, Warning, TEXT("some other direction"));
			}
			//UE_LOG(LogTemp, Warning, TEXT("AI rotation: %s"), *myDirection.ToString());
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
		UE_LOG(LogTemp, Warning, TEXT("-----Entered Triggered Area"));
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving to Player"));
			moveToPlayer = true;
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			if (player->getLightAmount() > 0 && player->getLightEnabled())//This is where the enemy is supposed to run away
			{
				moveToPlayer = false;
				moveAway = true;
				UE_LOG(LogTemp, Warning, TEXT("Moving away from Player"));
			}
		}
	}
}

void AAIEnemy::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("-------Exited Triggered Area fool!"));
	}
}

void AAIEnemy::OnAttackTrigger(class AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("The AI will destroy you!"));
	if (CheckIfPlayer(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Slowed Player1 and beginning drain"));
		currentPlayer = Cast<ADistanceCharacter>(OtherActor);
		player = Cast<ADistanceCharacter>(currentPlayer);
		player->ChangeSpeed(400);//Works, but when do we set it back to normal??
		//GetWorldTimerManager().SetTimer(this, &AAIEnemy::Drain, 1.0f, true);

		if (player->getLightAmount() > 0 && player->getLightEnabled())//I believe this belongs in the Tick method, since this only happens once on overlap
		{
			if (player->getLightEnabled())
			{
				// TODO: eventually we want the AI to drain 
				// light when light is enabled, instead
				// of draining player health

				// TODO: AI should move away if it's finished draining
				//scare ai away
				//moveToPlayer = false;
				//moveAway = true;
			}
		}
		else
		{
			//drain health from player: TODO
			drainHealth = true;
		}
	}
}

bool AAIEnemy::CheckIfPlayer(class AActor* OtherActor)
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	if (player1 != NULL && player1->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	else if (player2 != NULL && player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	return false;
}

