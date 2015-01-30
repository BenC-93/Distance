// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	moveToPlayer = false;
	moveAway = false;
	player1 = NULL;
	player2 = NULL;

	health = 100.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.0f, 90.0f, -65.0f);//y,z,x
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
		player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
		if (player1 != NULL && player1->GetActorLabel() == OtherActor->GetActorLabel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving to Player1"));
			moveToPlayer = true;
		}
		else if (player2 != NULL && player2->GetActorLabel() == OtherActor->GetActorLabel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving to Player2"));
			moveToPlayer = true;
		}
		else if (player1 == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 Null"));
		}
		else if (player2 == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player2 Null"));
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
	class ADistanceCharacter* player = Cast<ADistanceCharacter>(OtherActor);//TODO: need to check if player is not null or 2nd player or something
	player->ChangeSpeed(-200);//Sorta Works, but when do we set it back to normal?? there is a bug here that crashes sometimes because of the TODO that needs to happen or something.
	//The bug involves going near the weird green thing in the middle of the screen, while the ai is chasing the player. It might be a multiple trigger conflict and not checking if its a player
	if (player->getLightAmount() > 0)
	{
		if (player->getLightEnabled())
		{
			//scare ai away
			moveToPlayer = false;
			moveAway = true;
		}
		else
		{
			//drain from light: TODO
		}
	}
	else
	{
		//drain health from player: TODO
	}
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
