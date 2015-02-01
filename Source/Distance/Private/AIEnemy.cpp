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

	if (drainHealth && drainCounter%10 == 0 && drainCounter < 100) 
	{
		class ADistanceCharacter* player = Cast<ADistanceCharacter>(currentPlayer);
		player->ChangeHealth(-1.0f);
		if (player->Health == 0)
		{
			drainHealth = false;
			moveAway = true;
			moveToPlayer = false;
		}
		UE_LOG(LogTemp, Warning, TEXT("Health decremented, %f"), player->Health);
		drainCounter++;
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
		class ADistanceCharacter* player = Cast<ADistanceCharacter>(currentPlayer);
		player->ChangeSpeed(400);//Works, but when do we set it back to normal??
		//GetWorldTimerManager().SetTimer(this, &AAIEnemy::Drain, 1.0f, true);

		if (player->getLightAmount() > 0)
		{
			if (player->getLightEnabled())
			{
				// TODO: eventually we want the AI to drain 
				// light when light is enabled, instead
				// of draining player health

				// TODO: AI should move away if it's finished draining
				//scare ai away
				moveToPlayer = false;
				moveAway = true;
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

/*void AAIEnemy::OnOverlapBeginAttack_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack thing!!!!!!!!!!!!!!!!!!!!!!!"));
	}
}

void AAIEnemy::OnOverlapEndAttack_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Exited Attack thing!!!!!!!!!!"));
	}
}*/
