// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistancePlayerController.h"
#include "AIEnemy.h"
#include "AIBoss_Doubt.h"
#include "AI/Navigation/NavigationSystem.h"

//this is here for testing the spawning of items
#include "ItemLantern.h"

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

	//DEPRECIATED
	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADistancePlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("SetDestination", IE_Released, this, &ADistancePlayerController::OnSetDestinationReleased);

	//This is for testing the spawning of items
	InputComponent->BindAction("SpawnItem", IE_Pressed, this, &ADistancePlayerController::PleaseSpawnItem);

	InputComponent->BindAction("SetTarget", IE_Pressed, this, &ADistancePlayerController::OnSetTargetPressed);
	InputComponent->BindAction("SetTarget", IE_Released, this, &ADistancePlayerController::OnSetTargetReleased);
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
	AActor* hitActor = Hit.GetActor();
	if (hitActor->IsA(AItem::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked an Item"));
		AItem* item = Cast<AItem>(hitActor);
		if (DistanceCharacterClass->GetDistanceTo(item) < 250.0f)
		{
			DistanceCharacterClass->PickupItem(item);
		}
		else
		{
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
	else if (hitActor->IsA(AAIEnemy::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked an enemy"));
		SetNewMoveDestination(Hit.ImpactPoint);
	}
	else if (hitActor->IsA(AAIBoss_Doubt::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked a boss"));
		OnAttackBoss();
	}
	else if (Hit.bBlockingHit && canMove)
	{
		SetNewMoveDestination(Hit.ImpactPoint);
		bMoveToMouseCursor = true;
	}
}

void ADistancePlayerController::OnSetTargetReleased()
{
	bMoveToMouseCursor = false;
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
	DistanceCharacterClass->GetLight()->StartUse();
}

void ADistancePlayerController::OnInventoryPressed()
{
	DistanceCharacterClass->ToggleInventory();
}

void ADistancePlayerController::OnAttackBoss()//Temporary Binding, for boss testing***************************************
{
	attackBoss = true;
}

void ADistancePlayerController::PleaseSpawnItem()//Temp for testing ****
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (Hit.bBlockingHit)
	{
		FVector blah = FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z+100.0f);
		GetWorld()->SpawnActor<AItemLantern>(LanternClass, blah, FRotator(0,0,0));
	}
}

void ADistancePlayerController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	DistanceCharacterClass = Cast<ADistanceCharacter>(InPawn);
}
