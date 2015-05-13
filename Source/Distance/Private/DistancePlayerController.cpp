// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Distance.h"
#include "DistancePlayerController.h"
#include "AIEnemy.h"
#include "AIBoss_Doubt.h"
#include "ItemCrystal.h"
#include "Shrine.h"
#include "AI/Navigation/NavigationSystem.h"
#include "ConvergenceManager.h"
#include "DistancePlayerProxy.h"
#include "UnrealNetwork.h"

//this is here for testing the spawning of items
#include "ItemLantern.h"

ADistancePlayerController::ADistancePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	canMove = true;
	hitActor = NULL;
	rangeToItem = 250.0f;

	rangeToShrine = 350.0f;

	converged = false;//for testing

	switchedItem = false;//Temporary Bool, for boss testing***************************************
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

	InputComponent->BindAxis("MoveForward", this, &ADistancePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADistancePlayerController::MoveRight);
	InputComponent->BindAction("CycleInventory", IE_Pressed, this, &ADistancePlayerController::CycleInventory);
	InputComponent->BindAction("ItemPickup", IE_Pressed, this, &ADistancePlayerController::ItemPickup);
	InputComponent->BindAction("ItemDrop", IE_Pressed, this, &ADistancePlayerController::ItemDrop);

	//Temp input for crystal
	InputComponent->BindAction("OtherUseItem", IE_Pressed, this, &ADistancePlayerController::OnOtherUseItemPressed);
	InputComponent->BindAction("OtherUseItem", IE_Released, this, &ADistancePlayerController::OnUseItemReleased);
}

void ADistancePlayerController::MoveForward(float val)
{
	Cast<ADistancePlayerProxy>(GetPawn())->MoveFromInput(FVector(1.0, 0.0, 0.0), GetInputAxisValue("MoveForward"));
}

void ADistancePlayerController::MoveRight(float val)
{
	printLog(TEXT("input right: %f"), GetInputAxisValue("MoveRight"));
	Cast<ADistancePlayerProxy>(GetPawn())->MoveFromInput(FVector(0.0, 1.0, 0.0), GetInputAxisValue("MoveRight"));
	if (val > 0.f)
	{
		DCharacter()->GetMesh()->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
	}
	else if (val < 0.f)
	{
		DCharacter()->GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}

void ADistancePlayerController::CycleInventory()
{
	switchedItem = true;
	DCharacter()->ToggleInventory();
}

void ADistancePlayerController::ItemPickup()
{
	for (TActorIterator<AItem> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->IsA(AItemLantern::StaticClass()))
		{
			continue;
		}
		if (DCharacter()->GetDistanceTo(*ActorItr) <= rangeToItem)
		{
			if (DCharacter()->GetInventory().Num() <= 4)
			{
				if (*ActorItr == DCharacter()->GetItem()) { break; }
				DCharacter()->PickupItem(*ActorItr);
			}
			else
			{
				printScreen(FColor::Red, TEXT("Inventory Full! Did Not pick up item!"));
			}
			break;
		}
	}
}

void ADistancePlayerController::ItemDrop()
{
	DCharacter()->DropItem(DCharacter()->EquippedSlot);
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

void ADistancePlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	ServerSetNewMoveDestination(DestLocation);
}

void ADistancePlayerController::ServerSetNewMoveDestination_Implementation(const FVector DestLocation)
{
	ADistancePlayerProxy* const Pawn = Cast<ADistancePlayerProxy>(GetPawn());
	if (Pawn)
	{
		
		UNavigationSystem* const NavSys = UNavigationSystem::GetCurrent(this);
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			//NavSys->SimpleMoveToLocation(this, DestLocation);
			Pawn->MoveToLocation(this, DestLocation);
			Pawn->Character->FlipForMovement_RPC(DestLocation);
		}
	}
}

bool ADistancePlayerController::ServerSetNewMoveDestination_Validate(const FVector DestLocation)
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
		if (DCharacter()->GetDistanceTo(item) < 250.0f)
		{
			PickupItem(item);
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
	else if (hitActor->IsA(ATentacle::StaticClass()))
	{
		printScreen(FColor::Red, TEXT("Clicked a Tentacle"));
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

void ADistancePlayerController::PickupItem(AItem* Item)
{
	//DCharacter()->PickupItem(Item);
	ServerPickupItem(Item);
}

void ADistancePlayerController::ServerPickupItem_Implementation(AItem* Item)
{
	if (DCharacter()->GetInventory().Num() <= 4)//Something strange might be happening where it doesnt recognize the last item pushed yet? we get the exact num within the pickup function
	{
		DCharacter()->PickupItem(Item);
	}
	else
	{
		printScreen(FColor::Red, TEXT("Inventory Full! Did Not pick up item!"));
	}
}

bool ADistancePlayerController::ServerPickupItem_Validate(AItem* Item)
{
	return true;
}

void ADistancePlayerController::OnUseItemPressed()
{
	if (DCharacter() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	if (DCharacter()->GetItem() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}

	if (DCharacter()->GetItem()->IsA(AItemCrystal::StaticClass()))
	{
		bool shrineFound = false;
		for (TActorIterator<AShrine> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			float distToShrine = DCharacter()->GetDistanceTo(*ActorItr);
			if (distToShrine <= rangeToShrine)
			{
				shrineFound = true;	
			}
		}
		if (shrineFound)
		{
			Cast<AItemCrystal>(DCharacter()->GetItem())->StartUse(false);
		}
		else
		{
			Cast<AItemCrystal>(DCharacter()->GetItem())->StartUse(true);
		}
	}
	else
	{
		DCharacter()->GetItem()->StartUse();
	}
	UE_LOG(LogTemp, Warning, TEXT("Item Pressed: isInUse: %d"), DCharacter()->GetItemEnabled());
}

//temp
void ADistancePlayerController::OnOtherUseItemPressed()
{
	if (DCharacter() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	if (DCharacter()->GetItem() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}
	if (DCharacter()->GetItem()->IsA(AItemCrystal::StaticClass()))
	{
		Cast<AItemCrystal>(DCharacter()->GetItem())->StartUse(true);
	}
	else
	{
		DCharacter()->GetItem()->StartUse();
	}
	UE_LOG(LogTemp, Warning, TEXT("Item Pressed: isInUse: %d"), DCharacter()->GetItemEnabled());
}

void ADistancePlayerController::OnUseItemReleased()
{
	if (DCharacter() == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("DistanceCharacter is null."));
		return;
	}
	AItem* item = DCharacter()->GetItem();
	if (item == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Light is null."));
		return;
	}
	item->EndUse();
	/*UE_LOG(LogTemp, Warning, TEXT("Item Released: isInUse: %d"), DCharacter()->GetItemEnabled());
	//UE_LOG(LogTemp, Warning, TEXT("Item name: %s"), *DCharacter()->GetItemName());
	if (DCharacter()->GetItemName() == "LightBeam")
	{
		if (enemyActor != NULL && enemyActor->IsA(AAIBoss_Doubt::StaticClass()))
		{
			class AItemLightBeam* lightBeam = Cast<AItemLightBeam>(item);
			Cast<AAIBoss_Doubt>(enemyActor)->ChangeHealth(DCharacter()->Attack(lightBeam->totalChargedAmount));
			
			lightBeam->totalChargedAmount = 0.0f;
			
			if (Cast<AAIBoss_Doubt>(enemyActor)->GetHealth() <= 0)
			{
				enemyActor = NULL;
			}
		}
		else if (enemyActor != NULL && enemyActor->IsA(ATentacle::StaticClass()))
		{
			//printScreen(FColor::Red, "Attacking the boss woooooooo");
			class AItemLightBeam* lightBeam = Cast<AItemLightBeam>(item);
			Cast<ATentacle>(enemyActor)->ChangeHealth(DCharacter()->Attack(lightBeam->totalChargedAmount));
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
	DCharacter()->ToggleInventory();
}

void ADistancePlayerController::OnGetLocation()//Temporary Binding, for location testing***************************************
{
	UE_LOG(LogTemp, Error, TEXT("Location is = %s"), *(DCharacter()->GetActorLocation()).ToString());
}

void ADistancePlayerController::OnConvergenceBegin()//Temporary Binding, for convergence start***************************************
{
	if (!converged)
	{
		printScreen(FColor::Red, TEXT("Beginning Convergence"));
		ConvergenceManager::StartConvergence();
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(true);
	}
	else//make sure to get rid of this code when done testing with pushing a button to converge and diverge
	{
		printScreen(FColor::Red, TEXT("Ending Convergence"));
		ConvergenceManager::EndConvergence();
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);
	}
	converged = !converged;
}

void ADistancePlayerController::OnConvergenceEnd()
{
	printScreen(FColor::Red, TEXT("Ending Convergence"));
	ConvergenceManager::EndConvergence();
}

void ADistancePlayerController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
}

void ADistancePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

ADistanceCharacter* ADistancePlayerController::DCharacter() const
{
	return Cast<ADistancePlayerProxy>(GetPawn())->Character;
}

