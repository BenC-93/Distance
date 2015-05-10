// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "Shrine.h"

AShrine::AShrine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);
}