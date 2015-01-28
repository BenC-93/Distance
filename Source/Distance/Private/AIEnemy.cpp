// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	moveToPlayer = false;
	player1 = NULL;
	player2 = NULL;

	health = 100.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.0f, 90.0f, -65.0f);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITrigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITrigger"));
	AITrigger->Mobility = EComponentMobility::Movable;
	AITrigger->AttachTo(RootComponent);

	AITrigger->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBegin);
	AITrigger->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEnd);
	
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
		UE_LOG(LogTemp, Warning, TEXT("Entered Triggered Area"));
		player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
		if (player1 != NULL && player1->GetActorLabel() == OtherActor->GetActorLabel())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Moving to Player1"));
			moveToPlayer = true;
		}
		else if (player2 != NULL && player2->GetActorLabel() == OtherActor->GetActorLabel())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Moving to Player2"));
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
		UE_LOG(LogTemp, Warning, TEXT("Exited Triggered Area fool!"));
	}
}