// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLantern.h"

AItemLantern::AItemLantern(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
//	SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite>(TEXT("/Game/Sprites/Lantern_Sprite")));

	droppable = false;
}

void AItemLantern::StartUse()
{
	if (isInUse)
	{
		EndUse();
		return;
	}
	else
	{
		isInUse = true;
	}
}