// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "DistanceCharacter.h"
#include "ItemLightBeam.h"
#include "DistancePlayerController.generated.h"

UCLASS()
class ADistancePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADistancePlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void Possess(class APawn *InPawn) override;
	virtual void BeginPlay() override;

	/** Navigate player to the given world location. Local and Server */
	void SetNewMoveDestination(const FVector DestLocation);
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetNewMoveDestination(const FVector DestLocation);
	
	// Pickup Item, local and server flavors
	void PickupItem(AItem* Item);
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerPickupItem(AItem* Item);

	bool canMove;

	class AActor* hitActor;
	class AActor* enemyActor;

	bool switchedItem;//Temporary Bool, for boss testing***************************************

	void OnConvergenceEnd();
	
	class ADistanceCharacter* DCharacter() const;

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
	void OnUseItemReleased();
	void OnInventoryPressed();

	void OnGetLocation();//Temporary Binding, for location testing***************************************
	void OnConvergenceBegin();//Temporary Binding, for starting Convergence***************************************
};


