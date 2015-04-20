// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "Item.h"
#include "Engine.h"
#include "UnrealNetwork.h"

ADistanceCharacter::ADistanceCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Networking
	bReplicates = true;
	bReplicateMovement = true;

	bWantsToUse = false;

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
	SpriteComponent->SetIsReplicated(true);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
	ItemComponent->ChildActorClass = AItem::StaticClass();
	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);
	ItemComponent->SetIsReplicated(true);
	
	ItemSocketPoint = TEXT("HandSocket");

	//some C++ class constructor 
	//Your BP of the base C++ class static ConstructorHelpers::FObjectFinder YourBPOb(TEXT("Blueprint'/Game/Characters/YourBP.YourBP'")); 
	//if (YourBPOb.Object != NULL) { YourBPBaseClassPtr = Cast(YourBPOb.Object->GeneratedClass); }
}

void ADistanceCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (HasAuthority())
	{
		Health = GetMaxHealth();
		CreateDefaultInventory();
	}
}

void ADistanceCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// regen health
	if (Health < GetMaxHealth())
	{
		ChangeHealth(HealthRegenAmount * DeltaSeconds);
	}
}

void ADistanceCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}


/////////////////////*** INPUT HANDLING ***/////////////////////

void ADistanceCharacter::OnNextItem()
{
	const int32 CurrItem = Inventory.IndexOfByKey(CurrentItem);
	AItem* NextItem = Inventory[(CurrItem + 1) % Inventory.Num()];
	EquipItem(NextItem);
}

void ADistanceCharacter::OnPrevItem()
{
	const int32 CurrItem = Inventory.IndexOfByKey(CurrentItem);
	AItem* PrevItem = Inventory[(CurrItem - 1) % Inventory.Num()];
	EquipItem(PrevItem);
}


/////////////////////*** HEALTH and DAMAGE ***/////////////////////

int32 ADistanceCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ADistanceCharacter>()->Health;
}

void ADistanceCharacter::ChangeHealth(float Amount)
{
	int tempHealth = Health + Amount;
	Health = FMath::Max(0, FMath::Min(100, tempHealth));
}


/////////////////////*** INVENTORY ***/////////////////////

void ADistanceCharacter::CreateDefaultInventory()
{
	if (Role < ROLE_Authority) { return; }
	//AItem* Lantern = (GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnLantern());
	//AddItem(Lantern);
	UE_LOG(LogTemp, Error, TEXT("Test String"));
	if (Inventory.Num() > 0) { EquipItem(Inventory[0]); }
}

void ADistanceCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority) { return; }
	for (int32 i = Inventory.Num(); i >= 0; i--)
	{
		AItem* Item = Inventory[i];
		if (Item)
		{
			RemoveItem(Item);
			Item->Destroy();
		}
	}
}

void ADistanceCharacter::SetCurrentItem(AItem* NewItem, AItem* LastItem)
{
	AItem* LocLastItem = NULL;
	if (LastItem != NULL)
	{
		LocLastItem = LastItem;
	}
	else if (NewItem != CurrentItem)
	{
		LocLastItem = CurrentItem;
	}
	if (LocLastItem)
	{
		LocLastItem->OnUnequip();
	}
	CurrentItem = NewItem;
	if (NewItem)
	{
		NewItem->SetOwner(this);
		NewItem->OnEquip();
	}
}

void ADistanceCharacter::OnRep_CurrentItem(AItem* LastItem)
{
	SetCurrentItem(CurrentItem, LastItem);
}

void ADistanceCharacter::AddItem(AItem* Item)
{
	if (Item && HasAuthority())
	{
		Item->OnEnterInventory(this);
		Inventory.AddUnique(Item);
	}
}

void ADistanceCharacter::RemoveItem(AItem* Item)
{
	if (Item && HasAuthority())
	{
		Item->OnLeaveInventory();
		Inventory.RemoveSingle(Item);
	}
}

AItem* ADistanceCharacter::FindItem(TSubclassOf<AItem> ItemClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(ItemClass)) { return Inventory[i]; }
	}
	return NULL;
}

void ADistanceCharacter::EquipItem(AItem* Item)
{
	if (Item)
	{
		if (HasAuthority())
		{
			SetCurrentItem(Item);
		}
		else
		{
			SetCurrentItem(Item);
			ServerEquipItem(Item);
		}
	}
}

bool ADistanceCharacter::ServerEquipItem_Validate(AItem* Item)
{
	return true;
}

void ADistanceCharacter::ServerEquipItem_Implementation(AItem* Item)
{
	EquipItem(Item);
}

int32 ADistanceCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

AItem* ADistanceCharacter::GetInventoryItem(int32 index) const
{
	return Inventory[index];
}


/////////////////////*** ITEM USAGE ***/////////////////////

void ADistanceCharacter::StartItemUse()
{
	if (!bWantsToUse)
	{
		bWantsToUse = true;
		if (CurrentItem) { CurrentItem->StartUse(); }
	}
}

void ADistanceCharacter::StopItemUse()
{
	if (bWantsToUse)
	{
		bWantsToUse = false;
		if (CurrentItem) { CurrentItem->StopUse(); }
	}
}

bool ADistanceCharacter::IsUsing() const
{
	return bWantsToUse;
}


/////////////////////*** ITEM DATA ***/////////////////////

AItem* ADistanceCharacter::GetItem() const
{
	return CurrentItem;
}

FName ADistanceCharacter::GetItemSocket() const
{
	return ItemSocketPoint;
}


/////////////////////*** OLD STUFF ***/////////////////////

void ADistanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	printScreen(FColor::Red, TEXT("Begin Play"));
}

void ADistanceCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ADistanceCharacter, Inventory, COND_OwnerOnly);
	DOREPLIFETIME(ADistanceCharacter, CurrentItem);
	DOREPLIFETIME(ADistanceCharacter, Health);
}

void ADistanceCharacter::AddItemOfClassToInventory(class TSubclassOf<class AItem> ItemClass)//called at the beginning only once
{
	//AItem* NewItem;
	//NewItem->ItemClass = ItemClass;
	// TODO: fix Jordan's terrible assumption
	//NewItem->name = TEXT("Lantern");
	//Inventory.Add(NewItem);
	//spriteInventory.Add(Inventory.Last()->GetTheSprite());
	//ItemPickedUp();
}

void ADistanceCharacter::PickupItem(AItem* Item)//TODO:  be able to drop items within the game HUD on click and drag potentially?
{
	if (Item)
	{
		//Inventory.Add(Item);
		spriteInventory.Add(Item->GetItemTexture());
		Item->OnEnterInventory(this);
		Inventory.AddUnique(Item);
		printScreen(FColor::Red, TEXT("Pickup happened!"));
		// No item equipped, equip this one
		if (CurrentItem == NULL)
		{
			SetCurrentItem(Item);
		}
		ItemPickedUp();
		//UE_LOG(LogTemp, Error, TEXT("Inventory length: %d"), Inventory.Num());
	}
}

AItem* ADistanceCharacter::DropItem(int32 InvSlot)//TODO: when you pickup more than 1 item and drop until there is 1 left, clicking on (equipping) the last one will Error
{
	if (Inventory.Num() != 0 && Inventory.IsValidIndex(InvSlot))
	{
		// Need to create the item in the world,
		// before removing it from the array
		AItem* droppedItem = GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnItemAtLocation(Inventory[InvSlot]->GetClass(), GetActorLocation() - FVector(150.0f, 0.0f, 0.0f));
		uint32 tempIndex = (EquippedSlot + 1) % Inventory.Num();
		EquipItem(0);//default equip lantern
		//UE_LOG(LogTemp, Warning, TEXT("EquippedSlot = %d and Name = %s"), EquippedSlot, *GetItemName());
		Inventory.RemoveAt(InvSlot);
		spriteInventory.RemoveAt(InvSlot);
		ItemPickedUp();//refresh gui inventory after drop, but not needed if i keep the one in equip item
		UE_LOG(LogTemp, Warning, TEXT("Inventory num = %d"), Inventory.Num());
		return droppedItem;
	}
	UE_LOG(LogTemp, Warning, TEXT("Inventory num = %d"), Inventory.Num());
	return NULL;
}

void ADistanceCharacter::EquipItemComponent(int32 InvSlot)
{
	if (Inventory.IsValidIndex(InvSlot))
	{
//		//class TSubclassOf<AItem> ComponentClass = Inventory[InvSlot]
//		ItemComponent->OnComponentDestroyed();
//		//ItemComponent->ChildActorClass = ComponentClass;
//		ItemComponent->OnComponentCreated();
//		ItemComponent->ChildActor = Inventory[InvSlot];
//		ItemComponent->ChildActor->AttachRootComponentToActor(this);
		UE_LOG(LogDistance, Warning, TEXT("Equipping item"));
		Inventory[InvSlot]->OnEquip();
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

TArray<class AItem*> ADistanceCharacter::GetInventory()
{
	return Inventory;
}

/* BELOW ARE THE OLD ITEM HANDLING FUNCTIONS. THEY ARE SUBJECT TO CHANGE.
 * PLEASE USE NEW FUNCTIONS ABOVE. */

void ADistanceCharacter::ChangeItemAmount(float itemAmount)
{
	if (GetItem() != NULL)
	{
		GetItem()->ChangeEnergy(itemAmount);
	}
}

float ADistanceCharacter::GetMaxItemAmount()
{
	if (GetItem() == NULL)
	{
		return -1;
	}
	return GetItem()->GetMaxEnergy();
}

bool ADistanceCharacter::GetItemEnabled()
{
	if (GetItem() == NULL)
	{
		return false;
	}
	return GetItem()->CanBeUsed();
}

void ADistanceCharacter::ChangeSpeed(float speedAmount)
{
	GetCharacterMovement()->MaxWalkSpeed = speedAmount;
}

float ADistanceCharacter::Attack(float extra)
{
	float damage = BaseDamage + extra;
	UE_LOG(LogTemp, Error, TEXT("Total Attack Damage: %f"), damage);
	return damage * -1;
}
