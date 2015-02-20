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
	hitActor = NULL;

	switchedItem = false;//Temporary Bool, for boss testing***************************************
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
	InputComponent->BindAction("UseItem", IE_Released, this, &ADistancePlayerController::OnUseItemReleased);

	InputComponent->BindAction("Inventory", IE_Pressed, this, &ADistancePlayerController::OnInventoryPressed);

	InputComponent->BindAction("AttackBoss", IE_Pressed, this, &ADistancePlayerController::OnGetLocation);//Temporary Binding, for location testing***************************************
	InputComponent->BindAction("SwitchItem", IE_Pressed, this, &ADistancePlayerController::OnConvergenceBegin);//Temporary Binding, for starting convergence***************************************
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
	hitActor = Hit.GetActor();
	if (hitActor && hitActor->IsA(AItem::StaticClass()))//if we click on an Item
	{
		printScreen(FColor::Red, TEXT("Clicked an Item"));
		AItem* item = Cast<AItem>(hitActor);
		if (DistanceCharacterClass->GetDistanceTo(item) < 250.0f)
		{
			//UE_LOG(LogTemp, Error, TEXT("Inventory length: %d"), DistanceCharacterClass->GetInventory().Num());
			if (DistanceCharacterClass->GetInventory().Num() <= 4)//Something strange might be happening where it doesnt recognize the last item pushed yet? we get the exact num within the pickup function
			{
				DistanceCharacterClass->PickupItem(item);
			}
			else
			{
				printScreen(FColor::Red, TEXT("Inventory Full! Did Not pick up item!"));
			}
		}
		else
		{
			SetNewMoveDestination(Hit.ImpactPoint);
		}

	}
	else if (hitActor->IsA(AAIEnemy::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked an enemy"));
		enemyActor = hitActor;
		SetNewMoveDestination(Hit.ImpactPoint);
	}
	else if (hitActor->IsA(AAIBoss_Doubt::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked a boss"));
		enemyActor = hitActor;
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
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	if (DistanceCharacterClass->GetItem() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}
	DistanceCharacterClass->GetItem()->StartUse();
	UE_LOG(LogTemp, Warning, TEXT("Item Pressed: isInUse: %d"), DistanceCharacterClass->GetItemEnabled());
}

void ADistancePlayerController::OnUseItemReleased()
{
	if (DistanceCharacterClass == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	AItem* item = DistanceCharacterClass->GetItem();
	if (item == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}
	item->EndUse();
	UE_LOG(LogTemp, Warning, TEXT("Item Released: isInUse: %d"), DistanceCharacterClass->GetItemEnabled());
	//UE_LOG(LogTemp, Warning, TEXT("Item name: %s"), *DistanceCharacterClass->GetItemName());
	if (DistanceCharacterClass->GetItemName() == "LightBeam")
	{
		//printScreen(FColor::Red, "Used Light Beam");
		if (enemyActor != NULL && enemyActor->IsA(AAIBoss_Doubt::StaticClass()))
		{
			//printScreen(FColor::Red, "Attacking the boss woooooooo");
			class AItemLightBeam* lightBeam = Cast<AItemLightBeam>(item);
			Cast<AAIBoss_Doubt>(enemyActor)->ChangeHealth(DistanceCharacterClass->Attack(lightBeam->totalChargedAmount));
			//UE_LOG(LogTemp, Error, TEXT("totalChargedAmount before: %f"), lightBeam->totalChargedAmount);
			lightBeam->totalChargedAmount = 0.0f;
			//UE_LOG(LogTemp, Error, TEXT("totalChargedAmount  after: %d"), lightBeam->totalChargedAmount);
			if (Cast<AAIBoss_Doubt>(enemyActor)->GetHealth() <= 0)
			{
				enemyActor = NULL;
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("AttackBoss is true?: %d, and hitActor is: %s"), attackBoss, *hitActor->GetName());
}

void ADistancePlayerController::OnInventoryPressed()
{
	switchedItem = true;
	DistanceCharacterClass->ToggleInventory();
}

void ADistancePlayerController::OnGetLocation()//Temporary Binding, for location testing***************************************
{
	UE_LOG(LogTemp, Error, TEXT("Location is = %s"), *DistanceCharacterClass->GetActorLocation().ToString());
}

void ADistancePlayerController::OnConvergenceBegin()//Temporary Binding, for convergence start***************************************
{
	printScreen(FColor::Red, "Beginning Convergence");
	DistanceCharacterClass->TeleportTo(FVector(500, 0, 0) + DistanceCharacterClass->GetActorLocation(), DistanceCharacterClass->GetActorRotation());//moves 500 upwards
}

void ADistancePlayerController::OnConvergenceEnd()
{
	printScreen(FColor::Red, "Ending Convergence");
	DistanceCharacterClass->TeleportTo(FVector(-502, -341, DistanceCharacterClass->GetActorLocation().Z), DistanceCharacterClass->GetActorRotation());
	SetNewMoveDestination(DistanceCharacterClass->GetActorLocation() - FVector(121.0f, 0.0f, 0.0f));
}

void ADistancePlayerController::PleaseSpawnItem()//Temp for testing ****
{
	((ADistanceGameMode*)GetWorld()->GetAuthGameMode())->SpawnLantern();
}

void ADistancePlayerController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	DistanceCharacterClass = Cast<ADistanceCharacter>(InPawn);
	DistanceCharacterClass->PickupItem(GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnLantern());
	//DistanceCharacterClass->EquipItem(0);
	//DistanceCharacterClass->AddItemOfClassToInventory(((ADistanceGameMode*)GetWorld()->GetAuthGameMode())->ItemFromIndex(0));
	DistanceCharacterClass->EquipItemComponent(0);
	DistanceCharacterClass->ItemPickedUp();
}
