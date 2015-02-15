// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "DistanceCharacter.h"

//for tesrting
#include "ItemLantern.h"

#include "DistancePlayerController.generated.h"

UCLASS()
class ADistancePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADistancePlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void Possess(class APawn *InPawn) override;

	/** Navigate player to the given world location. */
	UFUNCTION(reliable, server, WithValidation)
	void SetNewMoveDestination(const FVector DestLocation);

	bool canMove;

	bool attackBoss;//Temporary Bool, for boss testing***************************************

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	/* Handle all mouse/touch input */
	void OnSetTargetPressed();
	void OnSetTargetReleased();
	void OnUseItemPressed();
	void OnInventoryPressed();

	void OnAttackBoss();//Temporary Binding, for boss testing***************************************
	void PleaseSpawnItem();//Temp for testing *******
	TSubclassOf<class AItemLantern> LanternClass;

	UPROPERTY(transient)
	class ADistanceCharacter* DistanceCharacterClass;
};


