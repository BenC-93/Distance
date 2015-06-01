// Copyright OvertimeGames 2015

#include "Distance.h"
#include "MinionItem.h"
#include "AIBoss_Betrayal_Minion.h"

AMinionItem::AMinionItem(const FObjectInitializer& ObjectInitializer)
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

FString AMinionItem::GetItemName() const
{
	return ItemName;
}

UTexture2D* AMinionItem::GetItemSprite() const
{
	return SpriteComponent->GetSprite()->GetBakedTexture();
}

void AMinionItem::SetOwningPawn(AAIBoss_Betrayal_Minion* NewOwner)
{
	if (OwningPawn != NewOwner)
	{
		Instigator = NewOwner;
		OwningPawn = NewOwner;
		// Net owner for RPC calls.
		SetOwner(NewOwner);
	}
}

void AMinionItem::OnEnterInventory(AAIBoss_Betrayal_Minion* NewOwner)
{
	SetOwningPawn(NewOwner);
	AttachToPawn();
}

void AMinionItem::OnLeaveInventory()
{
	DetachFromPawn();
	if (HasAuthority())
	{
		SetOwningPawn(nullptr);
	}
	OnUnequip();
}

void AMinionItem::StartUse()
{
	bIsInUse = true; //this should be the first thing that happens
}

void AMinionItem::EndUse()
{
	bIsInUse = false; //this should be the last thing that happens
}

void AMinionItem::OnEquip()
{
	SetActorHiddenInGame(false);
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().SetTimer(this, &AMinionItem::Regenerate, RegenRate, true);
}

void AMinionItem::OnUnequip()
{
	SetActorHiddenInGame(true);
	// Do any special effects like particles, or effects to the player
	// (that need to happen for all items)
	GetWorldTimerManager().ClearTimer(this, &AMinionItem::Regenerate);
}

void AMinionItem::AttachToPawn()
{
	USkeletalMeshComponent* PawnMesh = OwningPawn->GetMesh();
	FName AttachPoint = OwningPawn->GetItemAttachPoint();
	RootComponent->SetHiddenInGame(false);
	RootComponent->AttachTo(PawnMesh, AttachPoint, EAttachLocation::SnapToTarget);
}

void AMinionItem::DetachFromPawn()
{
	RootComponent->DetachFromParent();
	RootComponent->SetHiddenInGame(true);
}

void AMinionItem::ChangeAmount(float value)
{
	float tempAmount = ItemAmount + value;
	tempAmount = fmax(0.0f, fmin(MaxItemAmount, tempAmount));
	ItemAmount = tempAmount;
}

void AMinionItem::Regenerate()
{
	ChangeAmount(RegenAmount);
}

AAIBoss_Betrayal_Minion* AMinionItem::GetOwningPawn()
{
	return OwningPawn;
}

void AMinionItem::FlipForDirection(float xFlip)
{
	RootSceneComponent->SetRelativeScale3D(FVector(xFlip, 1.0f, 1.0f));
}
