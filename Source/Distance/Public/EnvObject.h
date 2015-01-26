// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "EnvObject.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AEnvObject : public AActor
{
	GENERATED_BODY()

		/**
		* stuff goes here
		*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* SpriteComponent;
	
public:
	AEnvObject(const FObjectInitializer& ObjectInitializer);
};
