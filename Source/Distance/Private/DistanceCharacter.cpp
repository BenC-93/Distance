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

void ADistanceCharacter::AddItemOfClassToInventory(class TSubclassOf<class AItem> ItemClass)
{
	InventoryItem* NewItem = new InventoryItem();
	NewItem->ItemClass = ItemClass;
	// TODO: fix Jordan's terrible assumption
	NewItem->name = TEXT("Lantern");
	Inventory.Add(NewItem);
}

void ADistanceCharacter::PickupItem(AItem* Item)
{
	if (Item)
	{
		Inventory.Add(new InventoryItem(Item));
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
		//Inventory[EquippedSlot]->OnUnequip();
		GetItem()->OnUnequip();
		Inventory[EquippedSlot]->Update(GetItem());
		EquippedSlot = InvSlot;
		//Inventory[EquippedSlot]->OnEquip();
		EquipItemComponent(EquippedSlot);
		GetItem()->OnEquip();
		GetItem()->Update(Inventory[EquippedSlot]);
	}
	//TODO: Fix this to use the actual inventory, this just automagically equips the lantern
	
}

void ADistanceCharacter::EquipItemComponent(int32 InvSlot)
{
	if (Inventory.IsValidIndex(InvSlot))
	{
		class TSubclassOf<AItem> ComponentClass = Inventory[InvSlot]->ItemClass;
		ItemComponent->OnComponentDestroyed();
		ItemComponent->ChildActorClass = ComponentClass;
		ItemComponent->OnComponentCreated();
		ItemComponent->ChildActor->AttachRootComponentToActor(this);
	}
}

void ADistanceCharacter::UseItem()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		//Inventory[EquippedSlot]->StartUse();
		GetItem()->StartUse();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: UseItem, Invalid Item."));
	}
}

TArray<class InventoryItem*> ADistanceCharacter::GetInventory()
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
	if (GetItem() != NULL)
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
	if (GetItem() != NULL)
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
	if (GetItem() != NULL)
	{
		float tempLight = getLightAmount() + lightAmount;
		if (tempLight <= getMaxLightAmount())
		{
			if (tempLight < 0)
			{
				GetItem()->amount = 0.0f;
			}
			else
			{
				GetItem()->amount = tempLight;
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
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->amount;
}

float ADistanceCharacter::getMaxLightAmount()
{
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->maxAmount;
}

bool ADistanceCharacter::getLightEnabled()
{
	if (GetItem() == NULL)
	{
		return false;
	}
	return GetItem()->isInUse;
}
//*************************************************************************************************************End
void ADistanceCharacter::ChangeItemAmount(float lightAmount)
{
	if (GetItem() != NULL)
	{
		float tempLight = GetItemAmount() + lightAmount;
		if (tempLight <= GetMaxItemAmount())
		{
			if (tempLight < 0)
			{
				GetItem()->amount = 0.0f;
			}
			else
			{
				GetItem()->amount = tempLight;
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
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->amount;
}

float ADistanceCharacter::GetMaxItemAmount()
{
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->maxAmount;
}

bool ADistanceCharacter::GetItemEnabled()
{
	if (GetItem() == NULL)
	{
		return false;
	}
	return GetItem()->isInUse;
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

AItem* ADistanceCharacter::GetItem()
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
