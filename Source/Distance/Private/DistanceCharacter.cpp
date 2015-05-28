// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "DItem.h"
#include "DItemPickup.h"
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
	
	// Default model faces left
	CurrentDirection = CharacterOrientation::LEFT;

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
	
	ItemSocket = TEXT("ItemSocket");

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
//	ItemComponent->ChildActorClass = AItem::StaticClass();
//	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);
	ItemComponent->SetIsReplicated(true);

	//some C++ class constructor 
	//Your BP of the base C++ class static ConstructorHelpers::FObjectFinder YourBPOb(TEXT("Blueprint'/Game/Characters/YourBP.YourBP'")); 
	//if (YourBPOb.Object != NULL) { YourBPBaseClassPtr = Cast(YourBPOb.Object->GeneratedClass); }
}

void ADistanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	//printScreen(FColor::Red, TEXT("Begin Play"));
	StartRegeneration();
	ItemPickedUp();
}

FName ADistanceCharacter::GetItemSocketPoint()
{
	return ItemSocketPoint;
}

void ADistanceCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADistanceCharacter, Health);
	DOREPLIFETIME(ADistanceCharacter, MaxHealth);
	DOREPLIFETIME(ADistanceCharacter, spriteInventory);
}

void ADistanceCharacter::PickupItem(ADItemPickup* Item)
{
	if (Item)
	{
		Item->OnUsed(this);
	}
}

bool ADistanceCharacter::ServerPickupItem_Validate(ADItemPickup* Item)
{
	return true;
}

void ADistanceCharacter::ServerPickupItem_Implementation(ADItemPickup* Item)
{
	PickupItem(Item);
}

AItem* ADistanceCharacter::DropItem(int32 InvSlot)
{
	//TODO: ALL THE DROP STUFF
//	if (Inventory.Num() != 0 && Inventory.IsValidIndex(InvSlot) && InvSlot != 0)
//	{
//		AItem* droppedItem = GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnItemAtLocation(Inventory[InvSlot]->ItemClass, GetActorLocation() - FVector(150.0f, 0.0f, 0.0f));
//		EquipItem(0);//equip lantern
//		Inventory.RemoveAt(InvSlot);
//		spriteInventory.RemoveAt(InvSlot);
//		ItemPickedUp();//gui refresh
//		UE_LOG(LogTemp, Warning, TEXT("Inventory num = %d"), Inventory.Num());
//		return droppedItem;
//	}
	return NULL;
}

void ADistanceCharacter::EquipItem(ADItem* Item)
{
	if (Item)
	{
		SetCurrentItem(Item);
	}
	
	// Notify the UI
	ItemPickedUp();
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
	for (ADItem* Item : Inventory)
	{
		if (Item)
			Sprites.Add(Item->GetItemSprite());
	}
	return Sprites;
}

void ADistanceCharacter::ToggleInventory()
{
	//cycle inventory items
	uint32 tempIndex = (EquippedSlot + 1) % Inventory.Num();
	EquipItem(Inventory[tempIndex]);
}

FString ADistanceCharacter::GetItemName()
{
	if (CurrentItem)
	{
		return CurrentItem->GetItemName();
	}
	return TEXT("Default");
}

bool ADistanceCharacter::GetIsItemDroppable()
{
	if (GetItem())
	{
		return GetItem()->bIsDroppable;
	}
	return false;
}

FName ADistanceCharacter::GetItemAttachPoint()
{
	return ItemSocket;
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
	//UE_LOG(LogDistance, Verbose, TEXT("Changing Health Breakpoint: %f"), Health);
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

//
//	New stuff here, sort later
//

void ADistanceCharacter::SetCurrentItem(class ADItem* NewItem, class ADItem* LastItem)
{
	ADItem* LocalLastItem = nullptr;
	if (LastItem)
	{
		LocalLastItem = LastItem;
	}
	else if (NewItem != CurrentItem)
	{
		LocalLastItem = CurrentItem;
	}
	
	// Unequip Current
	if (LocalLastItem)
	{
		LocalLastItem->OnUnequip();
	}
	
	// Find the item index of the item to be equipped, making sure it's in the inventory
	int32 IndexOfItem;
	if (ensure(Inventory.Find(NewItem, IndexOfItem)))
	{
		EquippedSlot = IndexOfItem;
	}
	
	CurrentItem = NewItem;
	
	if (NewItem)
	{
		NewItem->OnEquip();
	}
}

void ADistanceCharacter::AddItem(class ADItem* NewItem)
{
	if (NewItem && HasAuthority())
	{
		NewItem->OnEnterInventory(this);
		Inventory.Add(NewItem);
		
		// Equip Automatically?
		if (Inventory.Num() > 0)
		{
			ADItem* NewestItem = Inventory.Last();
			EquipItem(NewestItem);
		}
	}
}

void ADistanceCharacter::RemoveItem(ADItem* Item)
{
	if (Item && HasAuthority())
	{
		bool bIsCurrent = CurrentItem == Item;
		
		if (Inventory.Contains(Item))
		{
			Item->OnLeaveInventory();
		}
		Inventory.RemoveSingle(Item);
		
		// Replace the item just removed
		if (ensure(Inventory.Num() > 0))
		{
			if (bIsCurrent)
			{
				SetCurrentItem(Inventory[0]);
			}
		}
		else
		{
			// This shouldn't happen given our item framework
			SetCurrentItem(nullptr);
		}
		
		// Refresh the UI
		ItemPickedUp();
	}
}

bool ADistanceCharacter::HasInventorySpace()
{
	return Inventory.Num() < 5;
}

void ADistanceCharacter::FlipForDirection(float MovementInputDirection, bool bUseCurrentDirection)
{
	bool bShouldRefresh = false;
	CharacterOrientation direction;
	
	if (bUseCurrentDirection)
	{
		// MovementInputDirection is not useful, we just want to refresh the flippiness
		bShouldRefresh = true;
		direction = CurrentDirection;
	}
	else if (MovementInputDirection > 0.0f)
	{
		direction = CharacterOrientation::RIGHT;
		bShouldRefresh = (direction != CurrentDirection);
	}
	else if (MovementInputDirection < 0.0f)
	{
		direction = CharacterOrientation::LEFT;
		bShouldRefresh = (direction != CurrentDirection);
	}
	
	if (bShouldRefresh)
	{
		CurrentDirection = direction;
		// Pitch yaw and roll got switched up because the model itself is rotated now
		// Previously, we rotated the model in engine after import
		float roll = 0.0f;
		float pitch = DEFAULT_SPRITE_ROLL; // negate to flip
		float yaw = 0.0f; // Rotate by 180 to flip
		
		// Flips which side of the model is facing the camera
		float xScale = 1.0f;
		
		if (CurrentDirection == CharacterOrientation::LEFT)
		{
			// Don't scale, this is the natural orientation of the model
		}
		else if (CurrentDirection == CharacterOrientation::RIGHT)
		{
			// Rotate the character model
			pitch = -DEFAULT_SPRITE_ROLL;
			yaw = 180.0f;
			// Flip the character so the back side of the model doesn't show
			xScale = -1.0f;
		}
		GetMesh()->SetRelativeRotation(FRotator(pitch, yaw, roll));
		GetMesh()->SetRelativeScale3D(FVector(xScale, 1.0f, 1.0f));
		// Also scale the item in the X direction
		// For whatever reason this works whereas doing the y-scale without rotation was not
		CurrentItem->RootSceneComponent->SetRelativeScale3D(FVector(xScale, 1.0f, 1.0f));
	}
}

//
//
//

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
	return GetItem()->ItemAmount;
}

float ADistanceCharacter::GetMaxItemAmount()
{
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->MaxItemAmount;
}

bool ADistanceCharacter::GetItemEnabled()
{
	if (GetItem() == NULL)
	{
		return false;
	}
	return GetItem()->bIsInUse;
}

bool ADistanceCharacter::GetItemHasOwner()
{
	return (GetItem()->GetOwningPawn() != NULL);
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

ADItem* ADistanceCharacter::GetItem()
{
	return CurrentItem;
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
