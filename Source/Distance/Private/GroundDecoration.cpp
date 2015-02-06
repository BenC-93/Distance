// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "GroundDecoration.h"

AGroundDecoration::AGroundDecoration(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	// Create a sprite...
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootSceneComponent);
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, -90.0f);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


