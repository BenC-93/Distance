// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "Item.h"
#include "Engine.h"

ADistanceCharacter::ADistanceCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	BaseDamage = 20.0f;

	//Light = Cast<AItem>(ConstructObject<AItem>(AItem::StaticClass()));//Doesnt work, TODO initialize this item correctly

	//Light = new AItem(ObjectInitializer);


	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-70.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Create a sprite...
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
	ItemComponent->ChildActorClass = AItem::StaticClass();
	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);
}

/**
* ChangeHealth()
* 
* healthAmount is negative if it represents health change from an attack
* healthAmount is positive if it represents health change from a heal item
*
*/

void ADistanceCharacter::ChangeHealth(float healthAmount) 
{
	float tempHealth = Health + healthAmount;

	if (tempHealth <= MaxHealth)
	{
		if (tempHealth < 0)
		{
			Health = 0.0f;
		}
		else
		{
			Health = tempHealth;
		}
	}
}

void ADistanceCharacter::RegenerateHealth()
{
	ChangeHealth(1.0);
}

void ADistanceCharacter::StartRegeneration()
{
	GetWorldTimerManager().SetTimer(this, &ADistanceCharacter::RegenerateHealth, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("Health regeneration timer is set"));
	if (Health == MaxHealth)
	{
		GetWorldTimerManager().PauseTimer(this, &ADistanceCharacter::RegenerateHealth);
		UE_LOG(LogTemp, Warning, TEXT("Health regeneration timer is paused"));
	}

}

void ADistanceCharacter::ChangeLight(float lightAmount)
{
	if (GetLight() != NULL)
	{
		GetLight()->amount = lightAmount;
	}
}

float ADistanceCharacter::getLightAmount()
{
	if (GetLight() == NULL)
	{
		return -1;
	}
	return GetLight()->amount;
}

bool ADistanceCharacter::getLightEnabled()
{
	if (GetLight() == NULL)
	{
		return false;
	}
	return GetLight()->bIsEnabled;
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

AItem* ADistanceCharacter::GetLight()
{
	return (AItem *)ItemComponent->ChildActor;
}

/**
* Attack()
*
* calculates and returns player damage
*
*/
float ADistanceCharacter::Attack()
{
	float damage = BaseDamage;
	return damage;
}
