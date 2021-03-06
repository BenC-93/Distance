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
	bReplicateMovement = true;

	Health = 100.0f;
	MaxHealth = 100.0f;
	BaseDamage = 1.0f;
	EquippedSlot = 0;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(20.f, 60.f);

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
	SpriteComponent->SetIsReplicated(true);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
	// It can't actually default to being an item actor since item is abstract
//	ItemComponent->ChildActorClass = AItem::StaticClass();
	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);
	ItemComponent->SetIsReplicated(true);

	//some C++ class constructor 
	//Your BP of the base C++ class static ConstructorHelpers::FObjectFinder YourBPOb(TEXT("Blueprint'/Game/Characters/YourBP.YourBP'")); 
	//if (YourBPOb.Object != NULL) { YourBPBaseClassPtr = Cast(YourBPOb.Object->GeneratedClass); }
}

void ADistanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	printScreen(FColor::Red, TEXT("Begin Play"));
	StartRegeneration();
	ItemPickedUp();
}

void ADistanceCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADistanceCharacter, Health);
	DOREPLIFETIME(ADistanceCharacter, MaxHealth);
	DOREPLIFETIME(ADistanceCharacter, Inventory);
}

void ADistanceCharacter::PickupItem(AItem* Item)
{
	if (Item && Inventory.Num() < 5)
	{
		ClientPickupItem(Item);
		//UE_LOG(LogTemp, Error, TEXT("Inventory length: %d"), Inventory.Num());
		if (Role == ROLE_Authority)
		{
			AInventoryItem* InvItem = GetWorld()->SpawnActor<AInventoryItem>();
			InvItem->CreateFromItem(Item);
			Inventory.Add(InvItem);
			Item->Destroy();
		} else
		{
			printScreen(FColor::Red, TEXT("Pickup happened!"));
		}
	}
}

void ADistanceCharacter::ClientPickupItem_Implementation(AItem* Item)
{
	if (Item == NULL) {
		return;
	}
	ItemPickedUp();
}

bool ADistanceCharacter::ServerPickupItem_Validate(AItem* Item)
{
	return true;
}

void ADistanceCharacter::ServerPickupItem_Implementation(AItem* Item)
{
	PickupItem(Item);
}

AItem* ADistanceCharacter::DropItem(int32 InvSlot)
{
	if (Inventory.Num() != 0 && Inventory.IsValidIndex(InvSlot) && InvSlot != 0)
	{
		AItem* droppedItem = GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnItemAtLocation(Inventory[InvSlot]->ItemClass, GetActorLocation() - FVector(150.0f, 0.0f, 0.0f));
		EquipItem(0);//equip lantern
		Inventory.RemoveAt(InvSlot);
		ItemPickedUp();//refresh gui inventory after drop, but not needed if i keep the one in equip item
		UE_LOG(LogTemp, Warning, TEXT("Inventory num = %d"), Inventory.Num());
		return droppedItem;
	}
	return NULL;
}

void ADistanceCharacter::EquipItem(int32 InvSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory length: %d"), Inventory.Num());
	UE_LOG(LogTemp, Warning, TEXT("InvSlot to be equipped: %d"), InvSlot);
	if (EquippedSlot == 0 && InvSlot == 0) { return; }
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
	ItemPickedUp();
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
		AItem* currItem = Cast<AItem>(ItemComponent->ChildActor);
		if (currItem) { currItem->OwningPawn = this; }
	}
}

void ADistanceCharacter::UseItem()
{
	if (Inventory.IsValidIndex(EquippedSlot) && GetItem() != NULL)
	{
		GetItem()->StartUse();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: UseItem, Invalid Item."));
	}
}

TArray<class UTexture2D*> ADistanceCharacter::GetSpriteInventory()
{
	TArray<UTexture2D*> Sprites;
	for (AInventoryItem* Item : Inventory)
	{
		if (Item)
			Sprites.Add(Item->GetItemSprite());
	}
	return Sprites;
}

TArray<class AInventoryItem*> ADistanceCharacter::GetInventory()
{
	return Inventory;
}

void ADistanceCharacter::ToggleInventory()
{
	//cycle inventory items
	uint32 tempIndex = (EquippedSlot + 1) % Inventory.Num();
	EquipItem(tempIndex);
}

FString ADistanceCharacter::GetItemName()
{
	if (Inventory.IsValidIndex(EquippedSlot))
	{
		AInventoryItem *currentItem = Inventory[EquippedSlot];
		return Inventory[EquippedSlot]->GetItemName();
	}
	return TEXT("Default");
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

/*
 * ChangeHealth()
 * 
 * healthAmount is negative if it represents health change from an attack
 * healthAmount is positive if it represents health change from a heal item
 *
 */

void ADistanceCharacter::ChangeHealth(float healthAmount)
{
	float tempHealth = Health + healthAmount;
	Health = fmax(0.0f, fmin(MaxHealth, tempHealth));
	UE_LOG(LogDistance, Verbose, TEXT("Changing Health Breakpoint: %f"), Health);
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
		GetItem()->ChangeAmount(itemAmount);
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

bool ADistanceCharacter::GetItemHasOwner()
{
	return (GetItem()->GetOwningPawn() != NULL);
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

void ADistanceCharacter::FlipForMovement_RPC_Implementation(const FVector DestLocation)
{
	if (DestLocation.Y >= GetActorLocation().Y)
	{
		// Rotation sucks, I'm sorry for relying on scale #yolo
		GetMesh()->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
	}
	else
	{
		// Rotation sucks, I'm sorry for using scale, please forgive me...
		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}

AItem* ADistanceCharacter::GetItem()
{
	return (AItem *)ItemComponent->ChildActor;
}

/*
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
