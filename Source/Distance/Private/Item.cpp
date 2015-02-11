// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Item.h"

AItem::AItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsEnabled = false;
	amount = 100.0f;
	maxAmount = 100.0f;


	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -60.f);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite> (TEXT("/Game/Sprites/Lantern_Sprite")));
}

void AItem::Use()
{
	bIsEnabled = !bIsEnabled;
	if (bIsEnabled)//may or may not need to check this if im just toggling
	{
		UE_LOG(LogTemp, Warning, TEXT("Item is: Enabled"));
		SpriteComponent->SetVisibility(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item is: Disabled"));
		SpriteComponent->SetVisibility(false);
	}
}

void AItem::OnEquip()
{

}

void AItem::OnUnequip()
{

}

void AItem::Pickup()
{
	if (SpriteComponent)
	{
		SpriteComponent->DestroyComponent();
	}
}

void AItem::Drop()
{

}
