// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemLantern.h"

AItemLantern::AItemLantern(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
//	SpriteComponent->SetSprite(ConstructorHelpers::FClassFinder<UPaperSprite>(TEXT("/Game/Sprites/Lantern_Sprite")));

	ItemName = TEXT("Lantern");
	bIsDroppable = false;
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LightIntensity = 100.0f * ItemAmount;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;


}

void AItemLantern::StartUse()
{
	if (bIsInUse)
	{
		bIsInUse = false;
		LightSource->SetVisibility(false);
	}
	else
	{
		bIsInUse = true;
		LightSource->SetVisibility(true);

	}
}

void AItemLantern::EndUse()
{
	//isInUse = false;
	return;
}

void AItemLantern::ChangeAmount(float value)
{
	Super::ChangeAmount(value);
	LightIntensity = 100.0f * ItemAmount;
	LightSource->SetIntensity(LightIntensity);
	if (ItemAmount < MaxItemAmount)
	{
		GetWorldTimerManager().SetTimer(this, &ADItem::Regenerate, RegenRate, true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(this, &ADItem::Regenerate);
	}
}

void AItemLantern::FlipForDirection(float xFlip)
{
	Super::FlipForDirection(xFlip);
	FVector lightPos = LightSource->GetRelativeTransform().GetLocation();
	lightPos.Y = -1.0f * xFlip * FMath::Abs(lightPos.Y);
	LightSource->SetRelativeLocation(lightPos);
}
