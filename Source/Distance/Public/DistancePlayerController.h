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

	/** Navigate player to the given world location. */
	UFUNCTION(reliable, server, WithValidation)
	void SetNewMoveDestination(const FVector DestLocation);

	bool converged;
	float rangeToItem;

	bool canMove;

	class AActor* hitActor;
	class AActor* enemyActor;

	bool switchedItem;//Temporary Bool, for boss testing***************************************

	void OnConvergenceEnd();
	
	FORCEINLINE class ADistanceCharacter* GetDistanceCharacter() const { return DistanceCharacterClass; }

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

	void MoveForward(float val);
	void MoveRight(float val);
	void CycleInventory();
	void ItemPickup();
	void ItemDrop();

	void OnGetLocation();//Temporary Binding, for location testing***************************************
	void OnConvergenceBegin();//Temporary Binding, for starting Convergence***************************************

	UPROPERTY(transient)
	class ADistanceCharacter* DistanceCharacterClass;
};


