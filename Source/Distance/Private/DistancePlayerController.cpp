// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistancePlayerController.h"
#include "AI/Navigation/NavigationSystem.h"

ADistancePlayerController::ADistancePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	canMove = true;

	attackBoss = false;//Temporary Bool, for boss testing***************************************
}

void ADistancePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ADistancePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADistancePlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADistancePlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("SetTarget", IE_Pressed, this, &ADistancePlayerController::OnSetTargetPressed);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADistancePlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADistancePlayerController::MoveToTouchLocation);

	InputComponent->BindAction("UseItem", IE_Pressed, this, &ADistancePlayerController::OnUseItemPressed);
	InputComponent->BindAction("Inventory", IE_Pressed, this, &ADistancePlayerController::OnInventoryPressed);

	InputComponent->BindAction("AttackBoss", IE_Pressed, this, &ADistancePlayerController::OnAttackBoss);//Temporary Binding, for boss testing***************************************
}

void ADistancePlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit && canMove)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ADistancePlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit && canMove)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ADistancePlayerController::SetNewMoveDestination_Implementation(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = UNavigationSystem::GetCurrent(this);
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

bool ADistancePlayerController::SetNewMoveDestination_Validate(const FVector DestLocation)
{
	return true;
}

void ADistancePlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ADistancePlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ADistancePlayerController::OnSetTargetPressed()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	// Handle different Hit types here!
}

void ADistancePlayerController::OnUseItemPressed()
{
	if (DistanceCharacterClass == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("DistanceCharacter is null."));
		return;
	}
	if (DistanceCharacterClass->GetLight() == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Light is null."));
		return;
	}
	DistanceCharacterClass->GetLight()->Use();
}

void ADistancePlayerController::OnInventoryPressed()
{
	DistanceCharacterClass->ToggleInventory();
}

void ADistancePlayerController::OnAttackBoss()//Temporary Binding, for boss testing***************************************
{
	attackBoss = true;
}

void ADistancePlayerController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	DistanceCharacterClass = Cast<ADistanceCharacter>(InPawn);
}
