// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIEnemy.h"
#include "DistanceCharacter.h"
#include "Spirit.h"

ASpirit::ASpirit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	lifeTime = 10.0f; //how long its alive, in seconds

	movementSpeed = 360.0f; //speed in cm/s

	OwningPawn = NULL;

	fadeOut = false;
	alphaVal = 0.0f; //in this case, 0 is not transparent, 1 is fully transparent

	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);

	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("TriggerBox"));
	TriggerBox->Mobility = EComponentMobility::Movable;
	TriggerBox->SetBoxExtent(FVector(400.0f, 400.0f, 100.0f), true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpirit::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASpirit::OnOverlapEnd);
	TriggerBox->AttachTo(RootComponent);
}

void ASpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//move to player
	if (OwningPawn)
	{
		FVector direction = OwningPawn->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		float speed = DeltaTime * movementSpeed;
		FVector newLoc = (direction * speed) + GetActorLocation();
		//UE_LOG(LogDistance, Error, TEXT("Spirit new loc: (%f, %f, %f)"), newLoc.X, newLoc.Y, newLoc.Z);
		if (GetDistanceTo(OwningPawn) > 150.0f)
		{
			SetActorLocation(newLoc);
		}
	}
	if (fadeOut)
	{
		alphaVal += (DeltaTime / 8);
		UE_LOG(LogDistance, Warning, TEXT("Alpha color: %f"), alphaVal);
		if (alphaVal >= 1.0f)
		{
			alphaVal = 1.0f;
			Destroy();
		}
		ChangeSpriteAlpha(alphaVal);
	}
}

void ASpirit::SetOwner(ADistanceCharacter* owningPlayer)
{
	// start playing spirit sound
	BPStartPlaySound();
	UE_LOG(LogDistance, Verbose, TEXT("Spirit's owner is set!"));
	//set owner for reference to move to player
	OwningPawn = owningPlayer;
	//begin deathtimer to countdown to death
	GetWorldTimerManager().SetTimer(this, &ASpirit::DeathTimer, lifeTime, false);
}

void ASpirit::DeathTimer()
{
	// stop playing spirit sound
	BPStopPlaySound();
	//destroy, or i could make a fade away
	fadeOut = true;
	//Destroy();
}

void ASpirit::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		//UE_LOG(LogDistance, Warning, TEXT("Spirit Overlaped with: %s"), *OtherActor->GetName());
		if (OtherActor->IsA(AAIEnemy::StaticClass()))
		{
			UE_LOG(LogDistance, Error, TEXT("Warded off enemy!"));
			//call function on enemy to ward them off
			Cast<AAIEnemy>(OtherActor)->RunAway();
		}
	}
}

void ASpirit::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//UE_LOG(LogDistance, Warning, TEXT("Spirit Exit Overlap with: %s"), *OtherActor->GetName());
	}
}