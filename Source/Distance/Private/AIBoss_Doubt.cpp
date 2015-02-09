// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"

AAIBoss_Doubt::AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 0.0f;
	maxHealth = 100.0f;//TODO: will change
	baseDamage = 20.0f;//TODO: will change

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
}

void AAIBoss_Doubt::ChangeHealth(float amount)
{

}

void AAIBoss_Doubt::Attack(float amount)
{

}

void AAIBoss_Doubt::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("****Player Entered BOSS Triggered Area"));
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
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
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)//player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	return false;
}