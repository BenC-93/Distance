// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DItem.h"
#include "DistanceCharacter.h"

ADItem::ADItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsDroppable = true;
	bCanUse = true;
	bIsInUse = false;
	ItemAmount = 100.0f;
	MaxItemAmount = 100.0f;
	RegenRate = 1.0f;
	RegenAmount = 1.0f;
	ItemName = TEXT("Default");
	OwningPawn = NULL;
	
	
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FString ADItem::GetItemName() const
{
	return ItemName;
}

UTexture2D* ADItem::GetItemSprite() const
{
	return SpriteComponent->GetSprite()->GetBakedTexture();
}

void ADItem::SetOwningPawn(ADistanceCharacter* NewOwner)
{
	if (OwningPawn != NewOwner)
	{
		Instigator = NewOwner;
		OwningPawn = NewOwner;
		// Net owner for RPC calls.
		SetOwner(NewOwner);
	}
}

void ADItem::OnEnterInventory(ADistanceCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void ADItem::StartUse()
{
	bIsInUse = true; //this should be the first thing that happens
}

void ADItem::EndUse()
{
	bIsInUse = false; //this should be the last thing that happens
}

void ADItem::OnEquip()
{
	AttachToPawn();
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().SetTimer(this, &ADItem::Regenerate, RegenRate, true);
}

void ADItem::OnUnequip()
{
	DetachFromPawn();
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().ClearTimer(this, &ADItem::Regenerate);
}

void ADItem::AttachToPawn()
{
	USkeletalMeshComponent* PawnMesh = OwningPawn->GetMesh();
	FName AttachPoint = OwningPawn->GetItemAttachPoint();
	RootComponent->SetHiddenInGame(false);
	RootComponent->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
}

void ADItem::DetachFromPawn()
{
	RootComponent->DetachFromParent();
	RootComponent->SetHiddenInGame(true);
}

void ADItem::ChangeAmount(float value)
{
	float tempAmount = ItemAmount + value;
	tempAmount = fmax(0.0f, fmin(MaxItemAmount, tempAmount));
	ItemAmount = tempAmount;
}

void ADItem::Regenerate()
{
	ChangeAmount(RegenAmount);
}

ADistanceCharacter* ADItem::GetOwningPawn()
{
	return OwningPawn;
}
