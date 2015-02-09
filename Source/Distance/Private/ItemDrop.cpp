// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ItemDrop.h"

AItemDrop::AItemDrop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	ItemTrigger = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("ItemTrigger"));
	ItemTrigger->Mobility = EComponentMobility::Movable;
	ItemTrigger->SetBoxExtent(FVector(300.0f, 300.0f, 60.0f), true);
	ItemTrigger->AttachTo(RootComponent);

	ItemTrigger->OnComponentBeginOverlap.AddDynamic(this, &AItemDrop::OnOverlapBegin);

	ItemComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("ItemComponent"));
	ItemComponent->ChildActorClass = AItem::StaticClass();
	ItemComponent->OnComponentCreated();
	ItemComponent->AttachTo(RootComponent);
}

bool AItemDrop::PlayerPicksUpItem(class AActor* OtherActor)//TODO: not 100% positive im checking this correctly
{
	ACharacter *player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	ACharacter *player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	//UE_LOG(LogTemp, Warning, TEXT("Player actually is: %s"), *player1->GetActorLabel());// these are messages for checking if we collide with the player or not
	//UE_LOG(LogTemp, Warning, TEXT("Player checked is: %s"), *OtherActor->GetActorLabel());
	//UE_LOG(LogTemp, Warning, TEXT("Are they the same?: %d"), ((player1 == OtherActor)));// ? "Yes" : "No"));
	if (player1 != NULL && player1 == OtherActor)//player1->GetActorLabel() == OtherActor->GetActorLabel())
	{
		UE_LOG(LogDistance, Verbose, TEXT("Player can pick up item"));
		PickUp();
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)//player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		return true;
	}
	return false;
}

void AItemDrop::PickUp()
{
	UE_LOG(LogDistance, Verbose, TEXT("Player's pickup function called"));
	// add item to inventory here
}

void AItemDrop::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogDistance, Verbose, TEXT("Collision with item"));
		PlayerPicksUpItem(OtherActor);
		//if (CheckIfPlayer(OtherActor))
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("-----Player Entered Triggered Area"));
		//	moveToPlayer = true;//outer trigger boolean
		//	currentPlayer = Cast<ADistanceCharacter>(OtherActor);
		//	player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
		//	/*if (player->getLightAmount() > 0 && player->getLightEnabled())//This is where the enemy is supposed to run away
		//	{
		//	moveToPlayer = false;
		//	moveAway = true;
		//	UE_LOG(LogTemp, Warning, TEXT("Moving away from Player"));
		//	}*/
		//}
	}
}

AItem* AItemDrop::GetItem()
{
	return (AItem *)ItemComponent->ChildActor;
}


