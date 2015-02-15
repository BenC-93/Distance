// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "Item.h"
#include "Engine.h"
#include <cmath>

ADistanceCharacter::ADistanceCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;
	BaseDamage = 20.0f;
	EquippedSlot = 0;

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
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
	ItemComponent->ChildActorClass = AItem::StaticClass();
	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);

	//some C++ class constructor 
	//Your BP of the base C++ class static ConstructorHelpers::FObjectFinder YourBPOb(TEXT("Blueprint'/Game/Characters/YourBP.YourBP'")); 
	//if (YourBPOb.Object != NULL) { YourBPBaseClassPtr = Cast(YourBPOb.Object->GeneratedClass); }
}

void ADistanceCharacter::PickupItem(AItem* Item)
{
	if (Item)
	{
		Inventory.Add(Item);
		Item->Pickup();
		printScreen(FColor::Red, TEXT("Pickup happened!"));
	}
}

void ADistanceCharacter::DropItem(int32 InvSlot)
{
	if (Inventory.IsValidIndex(InvSlot))
	{
		// Need to create the item in the world,
		// before removing it from the array
		Inventory.RemoveAt(InvSlot);
	}
}

void ADistanceCharacter::EquipItem(int32 InvSlot)
{
	if (Inventory.IsValidIndex(InvSlot))
	{
		//TODO: determine if this is where we stop item regeneration**********************************************
		Inventory[EquippedSlot]->OnUnequip();
		EquippedSlot = InvSlot;
		Inventory[EquippedSlot]->OnEquip();
		//TODO: determine if this is where we begin Item regeneration**********************************************
	}
	//TODO: Fix this to use the actual inventory, this just automagically equips the lantern
	ItemComponent->OnComponentDestroyed();
	ItemComponent->ChildActorClass = ((ADistanceGameMode*)GetWorld()->GetAuthGameMode())->ItemTypes[0];
	ItemComponent->CreateChildActor();
	ItemComponent->OnComponentCreated();
	ItemComponent->ChildActor->AttachRootComponentToActor(this);
}

void ADistanceCharacter::UseItem()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		Inventory[EquippedSlot]->StartUse();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: UseItem, Invalid Item."));
	}
}

TArray<class AItem*> ADistanceCharacter::GetInventory()
{
	return Inventory;
}

void ADistanceCharacter::ToggleInventory()
{
	// Toggle visibility of inventory GUI
}

FString ADistanceCharacter::GetItemName()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		return Inventory[EquippedSlot]->name;
	}
	return "Default";
}

/* BELOW ARE THE OLD ITEM HANDLING FUNCTIONS. THEY ARE SUBJECT TO CHANGE.
 * PLEASE USE NEW FUNCTIONS ABOVE. */

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
	if (int(Health) % 10 == 0 || abs(healthAmount) >= 10)
	{
		UE_LOG(LogDistance, Verbose, TEXT("Changing Health Breakpoint: %f"), Health);
	}
}

void ADistanceCharacter::RegenerateHealth()
{
	ChangeHealth(1.0);
	if (Health >= MaxHealth)
	{
		GetWorldTimerManager().ClearTimer(this, &ADistanceCharacter::RegenerateHealth);
		UE_LOG(LogDistance, Verbose, TEXT("Health regeneration timer stopped, max health reached"));
	}
}

//Will nolonger be used
void ADistanceCharacter::RegenerateLight()
{
	if (GetLight() != NULL)
	{
		ChangeLight(1.0);
		if (getLightAmount() >= getMaxLightAmount())
		{
			GetWorldTimerManager().ClearTimer(this, &ADistanceCharacter::RegenerateLight);
			UE_LOG(LogDistance, Verbose, TEXT("Light regeneration timer stopped, max light amount reached"));
		}
	}
}

void ADistanceCharacter::RegenerateItem()
{
	if (GetEquippedItem() != NULL)
	{
		ChangeItemAmount(1.0);
		if (GetItemAmount() >= GetMaxItemAmount())
		{
			GetWorldTimerManager().ClearTimer(this, &ADistanceCharacter::RegenerateItem);
			UE_LOG(LogDistance, Verbose, TEXT("Light regeneration timer stopped, max light amount reached"));
		}
	}
}

void ADistanceCharacter::StartItemRegeneration()
{
	GetWorldTimerManager().SetTimer(this, &ADistanceCharacter::RegenerateItem, 1.0f, true);
}

void ADistanceCharacter::StartRegeneration()
{
	GetWorldTimerManager().SetTimer(this, &ADistanceCharacter::RegenerateHealth, 1.0f, true);
	GetWorldTimerManager().SetTimer(this, &ADistanceCharacter::RegenerateLight, 1.0f, true);//TODO: comment this out when ready
	UE_LOG(LogDistance, Verbose, TEXT("Health and light regeneration timers are set"));
}
//*************************************************************************************************************Begin
void ADistanceCharacter::ChangeLight(float lightAmount)
{
	if (GetLight() != NULL)
	{
		float tempLight = getLightAmount() + lightAmount;
		if (tempLight <= getMaxLightAmount())
		{
			if (tempLight < 0)
			{
				GetLight()->amount = 0.0f;
			}
			else
			{
				GetLight()->amount = tempLight;
			}
		}
		// Send a verbose log message whenever reaching or passing light values divisible by 10
		if (int(getLightAmount()) % 10 == 0 || abs(lightAmount) >= 10)
		{
			UE_LOG(LogDistance, Verbose, TEXT("Changing Light Breakpoint: %f"), getLightAmount());
		}
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

float ADistanceCharacter::getMaxLightAmount()
{
	if (GetLight() == NULL)
	{
		return -1;
	}
	return GetLight()->maxAmount;
}

bool ADistanceCharacter::getLightEnabled()
{
	if (GetLight() == NULL)
	{
		return false;
	}
	return GetLight()->isInUse;
}
//*************************************************************************************************************End
void ADistanceCharacter::ChangeItemAmount(float lightAmount)
{
	if (GetEquippedItem() != NULL)
	{
		float tempLight = GetItemAmount() + lightAmount;
		if (tempLight <= GetMaxItemAmount())
		{
			if (tempLight < 0)
			{
				GetEquippedItem()->amount = 0.0f;
			}
			else
			{
				GetEquippedItem()->amount = tempLight;
			}
		}
		// Send a verbose log message whenever reaching or passing light values divisible by 10
		if (int(GetItemAmount()) % 10 == 0 || abs(lightAmount) >= 10)
		{
			UE_LOG(LogDistance, Verbose, TEXT("Changing Light Breakpoint: %f"), GetItemAmount());
		}
	}
}

float ADistanceCharacter::GetItemAmount()
{
	if (GetEquippedItem() == NULL)
	{
		return -1;
	}
	return GetEquippedItem()->amount;
}

float ADistanceCharacter::GetMaxItemAmount()
{
	if (GetEquippedItem() == NULL)
	{
		return -1;
	}
	return GetEquippedItem()->maxAmount;
}

bool ADistanceCharacter::GetItemEnabled()
{
	if (GetEquippedItem() == NULL)
	{
		return false;
	}
	return GetEquippedItem()->isInUse;
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

AItem* ADistanceCharacter::GetLight()
{
	return (AItem *)ItemComponent->ChildActor;
}

class AItem* ADistanceCharacter::GetEquippedItem()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		return Inventory[EquippedSlot];
	}
	return NULL;
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
