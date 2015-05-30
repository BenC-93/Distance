// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistancePlayerController.h"
#include "AIEnemy.h"
#include "AIBoss_Doubt.h"
#include "ItemCrystal.h"
#include "DItemPickup.h"
#include "AI/Navigation/NavigationSystem.h"
#include "ConvergenceManager.h"

//this is here for testing the spawning of items
#include "ItemLantern.h"

ADistancePlayerController::ADistancePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	canMove = true;
	notTrappedByEnemy = true;//essentially, this is canMove
	canUseItem = true;
	hitActor = NULL;
	rangeToItem = 250.0f;

	rangeToShrine = 350.0f;

	converged = false;//for testing

}

void ADistancePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	//if (bMoveToMouseCursor)
	//{
	//	MoveToMouseCursor();
	//}
}

void ADistancePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//DEPRECIATED
	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADistancePlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("SetDestination", IE_Released, this, &ADistancePlayerController::OnSetDestinationReleased);

	//InputComponent->BindAction("SetTarget", IE_Pressed, this, &ADistancePlayerController::OnSetTargetPressed);
	//InputComponent->BindAction("SetTarget", IE_Released, this, &ADistancePlayerController::OnSetTargetReleased);
	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADistancePlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADistancePlayerController::MoveToTouchLocation);

	InputComponent->BindAction("UseItem", IE_Pressed, this, &ADistancePlayerController::OnUseItemPressed);
	InputComponent->BindAction("UseItem", IE_Released, this, &ADistancePlayerController::OnUseItemReleased);

	InputComponent->BindAction("Inventory", IE_Pressed, this, &ADistancePlayerController::OnInventoryPressed);

	InputComponent->BindAction("AttackBoss", IE_Pressed, this, &ADistancePlayerController::OnGetLocation);//Temporary Binding, for location testing***************************************
	//InputComponent->BindAction("SwitchItem", IE_Pressed, this, &ADistancePlayerController::OnConvergenceBegin);//Temporary Binding, for starting convergence***************************************

	InputComponent->BindAxis("MoveForward", this, &ADistancePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADistancePlayerController::MoveRight);
	InputComponent->BindAction("CycleInventory", IE_Pressed, this, &ADistancePlayerController::CycleInventory);
	InputComponent->BindAction("ItemPickup", IE_Pressed, this, &ADistancePlayerController::ItemPickup);
	//InputComponent->BindAction("ItemDrop", IE_Pressed, this, &ADistancePlayerController::ItemDrop);

	//Temp input for crystal
	//InputComponent->BindAction("OtherUseItem", IE_Pressed, this, &ADistancePlayerController::OnOtherUseItemPressed);
	//InputComponent->BindAction("OtherUseItem", IE_Released, this, &ADistancePlayerController::OnUseItemReleased);
}

void ADistancePlayerController::MoveForward(float val)
{
	if (notTrappedByEnemy && canMove) {
		DistanceCharacterClass->AddMovementInput(FVector(1.0, 0.0, 0.0), GetInputAxisValue("MoveForward"));
	}
}

void ADistancePlayerController::MoveRight(float val)
{
	if (notTrappedByEnemy && canMove) {
		DistanceCharacterClass->AddMovementInput(FVector(0.0, 1.0, 0.0), GetInputAxisValue("MoveRight"));
		DistanceCharacterClass->FlipForDirection(val);
	}
}

void ADistancePlayerController::CycleInventory()
{
	DistanceCharacterClass->ToggleInventory();
}

void ADistancePlayerController::ItemPickup()
{
	for (TActorIterator<ADItemPickup> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (DistanceCharacterClass->GetDistanceTo(*ActorItr) <= rangeToItem)
		{
			if (DistanceCharacterClass->HasInventorySpace())
			{
				DistanceCharacterClass->PickupItem(*ActorItr);
			}
			else
			{
				//printScreen(FColor::Red, TEXT("Inventory Full! Did Not pick up item!"));
				UE_LOG(LogTemp, Warning, TEXT("Inventory Full! Did Not pick up item!"));
			}
			break;
		}
	}
}

void ADistancePlayerController::ItemDrop()
{
	DistanceCharacterClass->DropItem(DistanceCharacterClass->EquippedSlot);
}

void ADistancePlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit && notTrappedByEnemy && canMove)
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
	if (HitResult.bBlockingHit && notTrappedByEnemy && canMove)
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

			float direction = DestLocation.Y - DistanceCharacterClass->GetActorLocation().Y;
			DistanceCharacterClass->FlipForDirection(direction);
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
	if (hitActor == NULL){
		return;
	}
	if (hitActor->IsA(ADItemPickup::StaticClass()))//if we click on an Item
	{
		//printScreen(FColor::Red, TEXT("Clicked an Item"));
		UE_LOG(LogTemp, Warning, TEXT("Clicked an Item"));
		ADItemPickup* item = Cast<ADItemPickup>(hitActor);
		if (DistanceCharacterClass->GetDistanceTo(item) < 250.0f)
		{
			//UE_LOG(LogTemp, Error, TEXT("Inventory length: %d"), DistanceCharacterClass->GetInventory().Num());
			if (DistanceCharacterClass->HasInventorySpace())
			{
				DistanceCharacterClass->PickupItem(item);
			}
			else
			{
				//printScreen(FColor::Red, TEXT("Inventory Full! Did Not pick up item!"));
				UE_LOG(LogTemp, Error, TEXT("Inventory Full! Did Not pick up item!"));
			}
		}
		else
		{
			SetNewMoveDestination(Hit.ImpactPoint);
		}

	}
	else if (hitActor->IsA(AAIEnemy::StaticClass()))
	{
		//printScreen(FColor::Red, TEXT("Clicked an enemy"));
		UE_LOG(LogTemp, Warning, TEXT("Clicked an enemy"));
		enemyActor = hitActor;
		SetNewMoveDestination(Hit.ImpactPoint);
	}
	else if (hitActor->IsA(AAIBoss_Doubt::StaticClass()))
	{
		//printScreen(FColor::Red, TEXT("Clicked a boss"));
		UE_LOG(LogTemp, Warning, TEXT("Clicked a boss"));
		enemyActor = hitActor;
	}
	else if (hitActor->IsA(ATentacle::StaticClass()))
	{
		//printScreen(FColor::Red, TEXT("Clicked a Tentacle"));
		UE_LOG(LogTemp, Warning, TEXT("Clicked a Tentacle"));
		enemyActor = hitActor;
	}
	else if (Hit.bBlockingHit && notTrappedByEnemy && canMove)
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
	if (canUseItem)
	{
		DistanceCharacterClass->GetItem()->StartUse();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Item Pressed: isInUse: %d"), DistanceCharacterClass->GetItemEnabled());
}

//temp
void ADistancePlayerController::OnOtherUseItemPressed()
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
	//UE_LOG(LogTemp, Warning, TEXT("Item Pressed: isInUse: %d"), DistanceCharacterClass->GetItemEnabled());
}

void ADistancePlayerController::OnUseItemReleased()
{
	if (DistanceCharacterClass == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	ADItem* item = DistanceCharacterClass->GetItem();
	if (item == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}
	if (canUseItem)
	{
		item->EndUse();
	}
	/*UE_LOG(LogTemp, Warning, TEXT("Item Released: isInUse: %d"), DistanceCharacterClass->GetItemEnabled());
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
		else if (enemyActor != NULL && enemyActor->IsA(ATentacle::StaticClass()))
		{
			//printScreen(FColor::Red, "Attacking the boss woooooooo");
			class AItemLightBeam* lightBeam = Cast<AItemLightBeam>(item);
			Cast<ATentacle>(enemyActor)->ChangeHealth(DistanceCharacterClass->Attack(lightBeam->totalChargedAmount));
			lightBeam->totalChargedAmount = 0.0f;
			if (enemyActor != NULL && Cast<ATentacle>(enemyActor)->health <= 0)
			{
				enemyActor = NULL;
			}
		}
	}*/
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
	if (!converged)
	{
		//printScreen(FColor::Red, TEXT("Beginning Convergence"));
		UE_LOG(LogTemp, Warning, TEXT("Beginning Convergence"));
		ConvergenceManager::StartConvergence();
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(true);
	}
	else//make sure to get rid of this code when done testing with pushing a button to converge and diverge
	{
		//printScreen(FColor::Red, TEXT("Ending Convergence"));
		UE_LOG(LogTemp, Warning, TEXT("Ending Convergence"));
		ConvergenceManager::EndConvergence();
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);
	}
	converged = !converged;
}

void ADistancePlayerController::OnConvergenceEnd()
{
	//printScreen(FColor::Red, TEXT("Ending Convergence"));
	UE_LOG(LogTemp, Warning, TEXT("Ending Convergence"));
	ConvergenceManager::EndConvergence();
}

void ADistancePlayerController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	// TODO: Create Lantern
	DistanceCharacterClass = Cast<ADistanceCharacter>(InPawn);
	DistanceCharacterClass->PickupItem(GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnLantern(DistanceCharacterClass));

//	DistanceCharacterClass->EquipItemComponent(0);
//	DistanceCharacterClass->ItemPickedUp();
}
