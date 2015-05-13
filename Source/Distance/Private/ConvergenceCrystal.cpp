// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "ConvergenceCrystal.h"

AConvergenceCrystal::AConvergenceCrystal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 100.0f;

	healthLossRate = 0.5f;

	movementSpeed = 390.0f; //speed in cm/s

	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);
}

void AConvergenceCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//move to inbetween players
	if (player1 && player2)
	{
		FVector midpointVec = (player1->GetActorLocation() + player2->GetActorLocation()) / 2;
		FVector direction = midpointVec - GetActorLocation();
		float distToMidPoint = direction.Size();
		direction.Normalize();
		float speed = DeltaTime * movementSpeed;
		FVector newLoc = (direction * speed) + GetActorLocation();
		//UE_LOG(LogDistance, Error, TEXT("crystal new loc: (%f, %f, %f)"), newLoc.X, newLoc.Y, newLoc.Z);
		if (GetDistanceTo(player1) > 150.0f && GetDistanceTo(player2) > 150.0f)
		{
			if (distToMidPoint > 50.0f)
			{
				SetActorLocation(newLoc);
			}
		}
	}
}

void AConvergenceCrystal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimer(this, &AConvergenceCrystal::LoseHealthTimer, healthLossRate, true);
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
}

void AConvergenceCrystal::LoseHealthTimer()
{
	//lose health over time
	if (health - 1.0f <= 0.0f)
	{
		health = 0;
		GetWorldTimerManager().ClearTimer(this, &AConvergenceCrystal::LoseHealthTimer);
	}
	else
	{
		health -= 1.0f;
	}
	UE_LOG(LogDistance, Warning, TEXT("crystal health: %f"), health);
}