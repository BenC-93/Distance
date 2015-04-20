// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"
#include "DistanceCharacter.h"
#include "UnrealNetwork.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	RootComponent = SpriteComponent;
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -65.f);

	bIsEquipped = false;
	bWantsToUse = false;
	bCanRegen = false;

	Energy = 100.f;

	bReplicates = true;

	ItemInfo = FItemData();

}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, MyPawn);
	DOREPLIFETIME(AItem, Energy);
	DOREPLIFETIME(AItem, bCanRegen);
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Energy = GetMaxEnergy();
	DetachSpriteFromPawn();
}

void AItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsEquipped() && CanRegen() && GetEnergy() < GetMaxEnergy())
	{
		ChangeEnergy(GetEnergyRegenAmount() * DeltaSeconds);
	}
}


/////////////////////*** ENERGY ***/////////////////////

void AItem::ChangeEnergy(float amount)
{
	float tempEnergy = Energy + amount;
	Energy = FMath::Max(0.f, FMath::Min(100.f, tempEnergy));

	if (GetEnergy() < 100.f && CanRegen() && MyPawn->GetItem() == this) { StartRegen(); }
}


/////////////////////*** INVENTORY ***/////////////////////

void AItem::OnEquip()
{
	AttachSpriteToPawn();
	bIsEquipped = true;
	if (MyPawn)
	{
		if (MyPawn->IsLocallyControlled() && CanRegen()) { StartRegen(); }
	}
}

void AItem::OnUnequip()
{
	DetachSpriteFromPawn();
	bIsEquipped = false;
	StopUse();
	if (bCanRegen) { bCanRegen = false; }
}

void AItem::OnEnterInventory(ADistanceCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void AItem::OnLeaveInventory()
{
	if (HasAuthority()) { SetOwningPawn(NULL); }
	if (IsEquipped()) { OnUnequip(); }
}

bool AItem::IsEquipped() const
{
	return bIsEquipped;
}


/////////////////////*** INPUT ***/////////////////////

void AItem::StartUse()
{
	if (Role < ROLE_Authority) { ServerStartUse(); }
	if (!bWantsToUse) { bWantsToUse = true; }
}

void AItem::StopUse()
{
	if (Role < ROLE_Authority) { ServerStopUse(); }
	if (bWantsToUse) { bWantsToUse = false; }
}

void AItem::StartRegen(bool bFromRep)
{
	if (!bFromRep && Role < ROLE_Authority) { ServerStartRegen(); }
	if (bFromRep || CanRegen())
	{
		bCanRegen = true;
	}
}

void AItem::StopRegen()
{
	bCanRegen = false;
}


/////////////////////*** CONTROL ***/////////////////////

bool AItem::CanBeUsed() const
{
	return (GetEnergy() >= GetUseAmount());
}

bool AItem::CanRegen() const
{
	return (bCanRegen);
}


/////////////////////*** DATA ***/////////////////////

FString AItem::GetName() const
{
	return ItemInfo.Name;
}

bool AItem::GetIsDroppable() const
{
	return ItemInfo.bIsDroppable;
}

float AItem::GetEnergy() const
{
	return Energy;
}

float AItem::GetMaxEnergy() const
{
	return ItemInfo.MaxEnergy;
}

float AItem::GetEnergyRegenAmount() const
{
	return ItemInfo.EnergyRegenAmount;
}

float AItem::GetUseAmount() const
{
	return ItemInfo.EnergyUseAmount;
}

UPaperSpriteComponent* AItem::GetItemSprite() const
{
	return SpriteComponent;
}

ADistanceCharacter* AItem::GetCharacterOwner() const
{
	return MyPawn;
}

void AItem::SetOwningPawn(ADistanceCharacter* NewPawn)
{
	if (MyPawn != NewPawn)
	{
		Instigator = NewPawn;
		MyPawn = NewPawn;
		SetOwner(NewPawn);
	}
}


/////////////////////*** INPUT [SERVER SIDE] ***/////////////////////

bool AItem::ServerStartUse_Validate()
{
	return true;
}

void AItem::ServerStartUse_Implementation()
{
	StartUse();
}

bool AItem::ServerStopUse_Validate()
{
	return true;
}

void AItem::ServerStopUse_Implementation()
{
	StopUse();
}

bool AItem::ServerStartRegen_Validate()
{
	return true;
}

void AItem::ServerStartRegen_Implementation()
{
	StartRegen();
}

bool AItem::ServerStopRegen_Validate()
{
	return true;
}

void AItem::ServerStopRegen_Implementation()
{
	StopRegen();
}


/////////////////////*** REPLICATION HELPERS ***/////////////////////

void AItem::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void AItem::OnRep_Regen()
{
	if (bCanRegen)
	{
		StartRegen(true);
	}
	else
	{
		StopRegen();
	}
}


/////////////////////*** EFFECTS ***/////////////////////

void AItem::StartUseEffects()
{
	return;
}

UAudioComponent* AItem::PlayItemSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn) { AC = UGameplayStatics::PlaySoundAttached(Sound, MyPawn->GetRootComponent()); }
	return AC;
}


/////////////////////*** ITEM USAGE ***/////////////////////

// Only used for item subtypes (PURE_VIRTUAL):
void AItem::UseItem()
{
	return;
}

bool AItem::ServerHandleUsing_Validate()
{
	return true;
}

void AItem::ServerHandleUsing_Implementation()
{
	const bool bShouldUpdateEnergy = (GetEnergy() > 0 && CanBeUsed());
	HandleUsing();
	if (bShouldUpdateEnergy) { ChangeEnergy(GetUseAmount()); }
}

void AItem::HandleUsing()
{
	if (GetEnergy() > 0.f && CanBeUsed())
	{
		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			UseItem();
			ChangeEnergy(GetUseAmount());
		}
	}
	if (CanRegen()) { StartRegen(); }
}


/////////////////////*** SOCKETTING ***/////////////////////

void AItem::AttachSpriteToPawn()
{
	if (MyPawn)
	{
		DetachSpriteFromPawn();
		FName AttachPoint = MyPawn->GetItemSocket();
		USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
		RootComponent->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
		RootComponent->SetHiddenInGame(false);
//		if (MyPawn->IsLocallyControlled() == true)
//		{
//			USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
//			SpriteComponent->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
//			SpriteComponent->SetHiddenInGame(false);
//		}
//		else
//		{
//			UPaperSpriteComponent* UseItemSprite = GetItemSprite();
//			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
//			UseItemSprite->AttachTo(UsePawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
//			UseItemSprite->SetHiddenInGame(false);
//		}
	}
}

void AItem::DetachSpriteFromPawn()
{
	RootComponent->DetachFromParent();
	RootComponent->SetHiddenInGame(true);
}
