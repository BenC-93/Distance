// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLantern.h"

AItemLantern::AItemLantern(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
//	SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite>(TEXT("/Game/Sprites/Lantern_Sprite")));

	ItemInfo.Name = "Lantern";
	ItemInfo.bIsDroppable = false;
	GetItemSprite()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LightIntensity = 100.0f * GetEnergy();

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;


}

void AItemLantern::StartUse()
{
	if (bWantsToUse)
	{
		bWantsToUse = false;
		LightSource->SetVisibility(false);
	}
	else
	{
		bWantsToUse = true;
		LightSource->SetVisibility(true);
	}
}

void AItemLantern::StopUse()
{
	return;
}

void AItemLantern::ChangeEnergy(float value)
{
	Super::ChangeEnergy(value);
	LightIntensity = 100.0f * GetEnergy();
	LightSource->SetIntensity(LightIntensity);
}