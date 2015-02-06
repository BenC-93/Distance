// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "GroundDecoration.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AGroundDecoration : public AActor
{
	GENERATED_BODY()
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* SpriteComponent;
	
public:
	AGroundDecoration(const FObjectInitializer& ObjectInitializer);
};
