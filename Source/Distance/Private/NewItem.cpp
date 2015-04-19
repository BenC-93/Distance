//// Fill out your copyright notice in the Description page of Project Settings.
//
#include "Distance.h"
#include "NewItem.h"
//#include "DistanceCharacter.h"
//#include "UnrealNetwork.h"
//
//ANewItem::ANewItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
//{
//	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
//	RootComponent = SpriteComponent;
//	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -65.f);
//
//	bIsEquipped = false;
//	bWantsToUse = false;
//	bCanRegen = false;
//
//	Energy = 100.f;
//
//	bReplicates = true;
//
//}
//
//void ANewItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ANewItem, MyPawn);
//	DOREPLIFETIME(ANewItem, Energy);
//	DOREPLIFETIME(ANewItem, bCanRegen);
//}
//
//void ANewItem::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//	Energy = GetMaxEnergy();
//	DetachSpriteFromPawn();
//}
//
//void ANewItem::Tick(float DeltaSeconds)
//{
//	Super::Tick(DeltaSeconds);
//	
//	if (IsEquipped() && CanRegen() && GetEnergy() < GetMaxEnergy())
//	{
//		ChangeEnergy(GetEnergyRegenAmount() * DeltaSeconds);
//	}
//}
//
//
///////////////////////*** ENERGY ***/////////////////////
//
//void ANewItem::ChangeEnergy(float amount)
//{
//	float tempEnergy = Energy + amount;
//	Energy = FMath::Max(0.f, FMath::Min(100.f, tempEnergy));
//
//	if (GetEnergy() < 100.f && CanRegen() && MyPawn->GetItem() == this) { StartRegen(); }
//}
//
//
///////////////////////*** INVENTORY ***/////////////////////
//
//void ANewItem::OnEquip()
//{
//	AttachSpriteToPawn();
//	bIsEquipped = true;
//	if (MyPawn)
//	{
//		if (MyPawn->IsLocallyControlled() && CanRegen()) { StartRegen(); }
//	}
//}
//
//void ANewItem::OnUnequip()
//{
//	DetachSpriteFromPawn();
//	bIsEquipped = false;
//	StopUse();
//	if (bCanRegen) { bCanRegen = false; }
//}
//
//void ANewItem::OnEnterInventory(ADistanceCharacter* NewOwner)
//{
//	SetOwningPawn(NewOwner);
//}
//
//void ANewItem::OnLeaveInventory()
//{
//	if (HasAuthority()) { SetOwningPawn(NULL); }
//	if (IsEquipped()) { OnUnequip(); }
//}
//
//bool ANewItem::IsEquipped() const
//{
//	return bIsEquipped;
//}
//
//
///////////////////////*** INPUT ***/////////////////////
//
//void ANewItem::StartUse()
//{
//	if (Role < ROLE_Authority) { ServerStartUse(); }
//	if (!bWantsToUse) { bWantsToUse = true; }
//}
//
//void ANewItem::StopUse()
//{
//	if (Role < ROLE_Authority) { ServerStopUse(); }
//	if (bWantsToUse) { bWantsToUse = false; }
//}
//
//void ANewItem::StartRegen(bool bFromRep)
//{
//	if (!bFromRep && Role < ROLE_Authority) { ServerStartRegen(); }
//	if (bFromRep || CanRegen())
//	{
//		bCanRegen = true;
//	}
//}
//
//void ANewItem::StopRegen()
//{
//	bCanRegen = false;
//}
//
//
///////////////////////*** CONTROL ***/////////////////////
//
//bool ANewItem::CanBeUsed() const
//{
//	return (GetEnergy() >= GetUseAmount());
//}
//
//bool ANewItem::CanRegen() const
//{
//	return (bCanRegen);
//}
//
//
///////////////////////*** DATA ***/////////////////////
//
//FString ANewItem::GetName() const
//{
//	return ItemInfo.Name;
//}
//
//bool ANewItem::GetIsDroppable() const
//{
//	return ItemInfo.bIsDroppable;
//}
//
//float ANewItem::GetEnergy() const
//{
//	return Energy;
//}
//
//float ANewItem::GetMaxEnergy() const
//{
//	return ItemInfo.MaxEnergy;
//}
//
//float ANewItem::GetEnergyRegenAmount() const
//{
//	return ItemInfo.EnergyRegenAmount;
//}
//
//float ANewItem::GetUseAmount() const
//{
//	return ItemInfo.EnergyUseAmount;
//}
//
//UPaperSpriteComponent* ANewItem::GetItemSprite() const
//{
//	return SpriteComponent;
//}
//
//ADistanceCharacter* ANewItem::GetCharacterOwner() const
//{
//	return MyPawn;
//}
//
//void ANewItem::SetOwningPawn(ADistanceCharacter* NewPawn)
//{
//	if (MyPawn != NewPawn)
//	{
//		Instigator = NewPawn;
//		MyPawn = NewPawn;
//		SetOwner(NewPawn);
//	}
//}
//
//
///////////////////////*** INPUT [SERVER SIDE] ***/////////////////////
//
//bool ANewItem::ServerStartUse_Validate()
//{
//	return true;
//}
//
//void ANewItem::ServerStartUse_Implementation()
//{
//	StartUse();
//}
//
//bool ANewItem::ServerStopUse_Validate()
//{
//	return true;
//}
//
//void ANewItem::ServerStopUse_Implementation()
//{
//	StopUse();
//}
//
//bool ANewItem::ServerStartRegen_Validate()
//{
//	return true;
//}
//
//void ANewItem::ServerStartRegen_Implementation()
//{
//	StartRegen();
//}
//
//bool ANewItem::ServerStopRegen_Validate()
//{
//	return true;
//}
//
//void ANewItem::ServerStopRegen_Implementation()
//{
//	StopRegen();
//}
//
//
///////////////////////*** REPLICATION HELPERS ***/////////////////////
//
//void ANewItem::OnRep_MyPawn()
//{
//	if (MyPawn)
//	{
//		OnEnterInventory(MyPawn);
//	}
//	else
//	{
//		OnLeaveInventory();
//	}
//}
//
//void ANewItem::OnRep_Regen()
//{
//	if (bCanRegen)
//	{
//		StartRegen(true);
//	}
//	else
//	{
//		StopRegen();
//	}
//}
//
//
///////////////////////*** EFFECTS ***/////////////////////
//
//void ANewItem::StartUseEffects()
//{
//	return;
//}
//
//UAudioComponent* ANewItem::PlayItemSound(USoundCue* Sound)
//{
//	UAudioComponent* AC = NULL;
//	if (Sound && MyPawn) { AC = UGameplayStatics::PlaySoundAttached(Sound, MyPawn->GetRootComponent()); }
//	return AC;
//}
//
//
///////////////////////*** ITEM USAGE ***/////////////////////
//
//// Only used for item subtypes (PURE_VIRTUAL):
//void ANewItem::UseItem()
//{
//	return;
//}
//
//bool ANewItem::ServerHandleUsing_Validate()
//{
//	return true;
//}
//
//void ANewItem::ServerHandleUsing_Implementation()
//{
//	const bool bShouldUpdateEnergy = (GetEnergy() > 0 && CanBeUsed());
//	HandleUsing();
//	if (bShouldUpdateEnergy) { ChangeEnergy(GetUseAmount()); }
//}
//
//void ANewItem::HandleUsing()
//{
//	if (GetEnergy() > 0.f && CanBeUsed())
//	{
//		if (MyPawn && MyPawn->IsLocallyControlled())
//		{
//			UseItem();
//			ChangeEnergy(GetUseAmount());
//		}
//	}
//	if (CanRegen()) { StartRegen(); }
//}
//
//
///////////////////////*** SOCKETTING ***/////////////////////
//
//void ANewItem::AttachSpriteToPawn()
//{
//	if (MyPawn)
//	{
//		DetachSpriteFromPawn();
//		FName AttachPoint = MyPawn->GetItemSocket();
//		if (MyPawn->IsLocallyControlled() == true)
//		{
//			USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
//			SpriteComponent->AttachTo(PawnMesh, AttachPoint);
//			SpriteComponent->SetHiddenInGame(false);
//		}
//		else
//		{
//			UPaperSpriteComponent* UseItemSprite = GetItemSprite();
//			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
//			UseItemSprite->AttachTo(UsePawnMesh, AttachPoint);
//			UseItemSprite->SetHiddenInGame(false);
//		}
//	}
//}
//
//void ANewItem::DetachSpriteFromPawn()
//{
//	SpriteComponent->DetachFromParent();
//	SpriteComponent->SetHiddenInGame(true);
//}
