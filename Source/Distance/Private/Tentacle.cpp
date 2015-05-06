// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Tentacle.h"
#include "AIBoss_Doubt.h"

ATentacle::ATentacle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 10.0f;
	maxHealth = 10.0f;

	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.0f, 90.0f, 0.0f);//y,z,x
	SpriteComponent->RelativeScale3D = FVector(7.0f, 3.0f, 3.0f);
	SpriteComponent->AttachTo(RootComponent);
	
	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("TriggerBox"));
	TriggerBox->Mobility = EComponentMobility::Movable;
	TriggerBox->SetBoxExtent(FVector(175.0f, 65.0f, 500.0f), true);
	TriggerBox->RelativeLocation = FVector(-474.0f, -76.0f, 0.0f);
	TriggerBox->AttachTo(RootComponent);

}

void ATentacle::SetBossParent(class AAIBoss_Doubt* parent)
{
	UE_LOG(LogDistance, Error, TEXT("Tentacle parents are set!"));
	doubtParent = parent;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(doubtParent, &AAIBoss_Doubt::OnTentacleOverlap);
}

float ATentacle::ChangeHealth(float amount)
{
	float tempHealth = health + amount;

	if (tempHealth <= maxHealth)
	{
		if (tempHealth <= 0)
		{
			health = 0.0f;
			if (doubtParent)
			{
				UE_LOG(LogDistance, Error, TEXT("Did damage to boss"));
				doubtParent->ChangeHealth(-25);
			}
			DestroyTentacle();
		}
		else
		{
			health = tempHealth;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Tentacle health: %f"), health);
	return health;
}

void ATentacle::DestroyTentacle()
{
	Destroy();
	/*if (Role < ROLE_Authority)
	{
		ServerDestroyTentacle();
	}*/
}

bool ATentacle::ServerDestroyTentacle_Validate()
{
	return true;
}

void ATentacle::ServerDestroyTentacle_Implementation()
{
	DestroyTentacle();
}