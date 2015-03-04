// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLantern.h"

AItemLantern::AItemLantern(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
//	SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite>(TEXT("/Game/Sprites/Lantern_Sprite")));

	name = "Lantern";
	droppable = false;
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LightIntensity = 10000.0f;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = true;


}

void AItemLantern::StartUse()
{
	if (isInUse)
	{
		isInUse = false;
		return;
	}
	else
	{
		isInUse = true;
		//LightSource->bVisible = isInUse;
	}
	LightSource->ToggleVisibility();
	printScreen(FColor::Red, TEXT("Light visibility = %d"), LightSource->bVisible);
}

void AItemLantern::EndUse()
{
	return;
}