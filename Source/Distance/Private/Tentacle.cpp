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
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -65.f);
	SpriteComponent->RelativeScale3D = FVector(7.0f, 3.0f, 3.0f);
	SpriteComponent->AttachTo(RootComponent);
	
	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("TriggerBox"));
	TriggerBox->Mobility = EComponentMobility::Movable;
	TriggerBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f), true);
	TriggerBox->RelativeLocation = FVector(70.0f, -270.0f, 0.0f);
	TriggerBox->AttachTo(RootComponent);

}

void ATentacle::SetBossParent(class AAIBoss_Doubt* parent)
{
	doubtParent = parent;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(doubtParent, &AAIBoss_Doubt::OnTentacleOverlap);
}

float ATentacle::ChangeHealth(float amount)
{
	float tempHealth = health + amount;

	if (tempHealth <= maxHealth)
	{
		if (tempHealth < 0)
		{
			health = 0.0f;
			Destroy();
		}
		else
		{
			health = tempHealth;
		}
	}
	return health;
}