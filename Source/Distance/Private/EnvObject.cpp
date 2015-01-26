// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "EnvObject.h"


AEnvObject::AEnvObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Create a sprite...
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -60.f);
}

