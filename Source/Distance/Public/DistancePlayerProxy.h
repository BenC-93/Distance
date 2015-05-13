// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "DistancePlayerProxy.generated.h"


/**
 * This class serves as a proxy for the client to track movements of
 * the real character on the server and send back controls
 */
UCLASS(Blueprintable)
class DISTANCE_API ADistancePlayerProxy : public ACharacter
{
	GENERATED_BODY()

	ADistancePlayerProxy(const FObjectInitializer& ObjectInitializer);
		
	// Top down camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;
	
	// Camera boom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	// Character class reference for spawning
	TSubclassOf<AActor> CharacterClass;
	
	// AI Controller for actually navigating the player
	class AAIController* PlayerAI;
	
	// Used for spawning the character and other things
	virtual void BeginPlay() override;
	
	// Keep actor in sync on tick
	void Tick(float DeltaTime);

public:
	// What the controller uses for client movement
	void MoveToLocation(const class ADistancePlayerController* Controller, const FVector& Location);
	
	void MoveFromInput(const FVector InputVector, float AxisValue);
	
	// Pointer to the actual character which is replicated
	UPROPERTY(BlueprintReadOnly, Replicated)
	class ADistanceCharacter* Character;
};
