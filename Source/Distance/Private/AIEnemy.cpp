// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	health = 100.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	//CollisionCapsule = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleComponent"));
	//RootComponent = CollisionCapsule;

	AITrigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITrigger"));
	AITrigger->Mobility = EComponentMobility::Movable;
	//AITrigger->AttachParent = RootComponent;

	AITrigger->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBegin);
	AITrigger->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEnd);

	/*
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;*/

	/*
	Creaete a radius field for checking if player is nearby*/
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
		//ToggleLight();
	}
}

void AAIEnemy::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//ToggleLight();
	}
}
