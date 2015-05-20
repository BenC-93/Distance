// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLantern.h"

AItemLantern::AItemLantern(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
//	SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite>(TEXT("/Game/Sprites/Lantern_Sprite")));

	name = TEXT("Lantern");
	droppable = false;
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LightIntensity = 100.0f * amount;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;


}

void AItemLantern::StartUse()
{
	if (isInUse)
	{
		isInUse = false;
		LightSource->SetVisibility(false);
	}
	else
	{
		isInUse = true;
		LightSource->SetVisibility(true);

	}
}

void AItemLantern::EndUse()
{
	isInUse = false;
}

void AItemLantern::ChangeAmount(float value)
{
	Super::ChangeAmount(value);
	LightIntensity = 100.0f * amount;
	LightSource->SetIntensity(LightIntensity);
}