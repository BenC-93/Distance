// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "Item.h"
#include "Engine.h"
#include "UnrealNetwork.h"
#include <cmath>

ADistanceCharacter::ADistanceCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Networking
	bReplicates = true;

	Health = 100.0f;
	MaxHealth = 100.0f;
	BaseDamage = 1.0f;
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

void ADistanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	printScreen(FColor::Red, "Begin Play");
	StartRegeneration();
	ItemPickedUp();
}

void ADistanceCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADistanceCharacter, Health);
	DOREPLIFETIME(ADistanceCharacter, MaxHealth);
}

void ADistanceCharacter::AddItemOfClassToInventory(class TSubclassOf<class AItem> ItemClass)//called at the beginning only once
{
	InventoryItem* NewItem = new InventoryItem();
	NewItem->ItemClass = ItemClass;
	// TODO: fix Jordan's terrible assumption
	NewItem->name = TEXT("Lantern");
	Inventory.Add(NewItem);
	spriteInventory.Add(Inventory.Last()->GetItemSprite());
	//ItemPickedUp();
}

void ADistanceCharacter::PickupItem(AItem* Item)//TODO:  be able to drop items within the game HUD on click and drag potentially?
{
	if (Item)
	{
		Inventory.Add(new InventoryItem(Item));
		spriteInventory.Add(Inventory.Last()->GetItemSprite());
		Item->Pickup();
		printScreen(FColor::Red, TEXT("Pickup happened!"));
		ItemPickedUp();
		//UE_LOG(LogTemp, Error, TEXT("Inventory length: %d"), Inventory.Num());
	}
}

void ADistanceCharacter::DropItem(int32 InvSlot)//TODO: when you pickup more than 1 item and drop until there is 1 left, clicking on (equipping) the last one will Error
{
	if (Inventory.Num() != 0 && Inventory.IsValidIndex(InvSlot))
	{
		// Need to create the item in the world,
		// before removing it from the array
		GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnItemAtLocation(Inventory[InvSlot]->ItemClass, GetActorLocation() - FVector(150.0f, 0.0f, 0.0f));
		uint32 tempIndex = (EquippedSlot + 1) % Inventory.Num();
		EquipItem(0);//default equip lantern
		UE_LOG(LogTemp, Warning, TEXT("EquippedSlot = %d and Name = %s"), EquippedSlot, *GetItemName());
		Inventory.RemoveAt(InvSlot);
		spriteInventory.RemoveAt(InvSlot);
		ItemPickedUp();//refresh gui inventory after drop, but not needed if i keep the one in equip item
	}
	UE_LOG(LogTemp, Warning, TEXT("Inventory num = %d"), Inventory.Num());
}

void ADistanceCharacter::EquipItem(int32 InvSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory length: %d"), Inventory.Num());
	UE_LOG(LogTemp, Warning, TEXT("InvSlot to be equipped: %d"), InvSlot);
	if (Inventory.Num() != 0 && Inventory.IsValidIndex(InvSlot))//TODO: fix bug that happens because of drop item
	{
		//Inventory[EquippedSlot]->OnUnequip();
		if (GetItem() != NULL)
		{
			GetItem()->OnUnequip();
			if (Inventory.IsValidIndex(EquippedSlot))
			{
				Inventory[EquippedSlot]->Update(GetItem());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EquipItem: Error: Previous Item is Null!"));
		}
		EquippedSlot = InvSlot;
		//Inventory[EquippedSlot]->OnEquip();
		EquipItemComponent(EquippedSlot);
		if (GetItem() != NULL)
		{
			GetItem()->OnEquip();
			GetItem()->Update(Inventory[EquippedSlot]);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EquipItem: Error: Equipped Item is Null!"));
		}
		//Refresh gui inventory with ItemPickedUp()???? if we want some glow thing on current item then id say yes
		ItemPickedUp();
	}
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
	if (Inventory.IsValidIndex(EquippedSlot) && GetItem() != NULL)
	{
		//Inventory[EquippedSlot]->StartUse();
		GetItem()->StartUse();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: UseItem, Invalid Item."));
	}
}

TArray<class UTexture2D*> ADistanceCharacter::GetSpriteInventory()
{
	return spriteInventory;
}

TArray<class InventoryItem*> ADistanceCharacter::GetInventory()
{
	return Inventory;
}

void ADistanceCharacter::ToggleInventory()
{
	// Toggle visibility of inventory GUI
	uint32 tempIndex = (EquippedSlot + 1) % Inventory.Num();
	EquipItem(tempIndex);
	UE_LOG(LogTemp, Warning, TEXT("EquippedSlot = %d and Name = %s"), EquippedSlot, *GetItemName());
}

FString ADistanceCharacter::GetItemName()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		return Inventory[EquippedSlot]->name;
	}
	return "Default";
}

bool ADistanceCharacter::GetIsItemDroppable()
{
	if (GetItem())
	{
		return GetItem()->droppable;
	}
	return false;
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
	float RegenAmount = 1.0f;
	if (Health < MaxHealth)
	{
		ChangeHealth(RegenAmount);
	}
}

void ADistanceCharacter::StartRegeneration()
{
	float RegenInterval = 1.0f;
	GetWorldTimerManager().SetTimer(this, &ADistanceCharacter::RegenerateHealth, RegenInterval, true);
	UE_LOG(LogDistance, Verbose, TEXT("Health regeneration timer is set"));
}

void ADistanceCharacter::ChangeItemAmount(float itemAmount)
{
	if (GetItem() != NULL)
	{
		float tempItemAmount = GetItemAmount() + itemAmount;
		if (tempItemAmount <= GetMaxItemAmount())
		{
			if (tempItemAmount < 0)
			{
				GetItem()->amount = 0.0f;
			}
			else
			{
				GetItem()->amount = tempItemAmount;
			}
		}
		// Send a verbose log message whenever reaching or passing Item values divisible by 10
		if (int(GetItemAmount()) % 10 == 0 || abs(itemAmount) >= 10)
		{
			UE_LOG(LogDistance, Verbose, TEXT("Changing Item Breakpoint: %f"), GetItemAmount());
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
float ADistanceCharacter::Attack(float extra)
{
	float damage = BaseDamage + extra;
	UE_LOG(LogTemp, Error, TEXT("Total Attack Damage: %f"), damage);
	return damage * -1;
}
