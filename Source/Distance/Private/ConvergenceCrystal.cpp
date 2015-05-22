// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"
#include "AIBoss_Betrayal.h"
#include "ConvergenceCrystal.h"

AConvergenceCrystal::AConvergenceCrystal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 300.0f;

	healthLossRate = 0.5f;
	drainHealthDamage = 1.0f;

	movementSpeed = 390.0f; //speed in cm/s

	bossDoubt = NULL;

	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);

	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-70.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	ConvergenceCam = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ConvergenceCam"));
	ConvergenceCam->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	ConvergenceCam->bUsePawnControlRotation = false;
}

void AConvergenceCrystal::BeginPlay()
{
	ConvergenceCam->SetActive(true);
	if (player1) { Cast<APlayerController>(player1->GetController())->SetViewTarget(this); }
	if (player2) { Cast<APlayerController>(player2->GetController())->SetViewTarget(this); }
}

void AConvergenceCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//move to inbetween players
	if (player1 && player2)
	{
		for (TActorIterator<AAIBoss_Doubt> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			bossDoubt = Cast<AAIBoss_Doubt>(*ActorItr);
			if (ActorItr->p1InTrigger && ActorItr->p2InTrigger)
			{
				return;
			}
		}

		for (TActorIterator<AAIBoss_Betrayal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			bossBetrayal = Cast<AAIBoss_Betrayal>(*ActorItr);
			if (ActorItr->p1InTrigger || ActorItr->p2InTrigger)
			{
				return;
			}
		}

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
	if (health - drainHealthDamage <= 0.0f)
	{
		health = 0;
		GetWorldTimerManager().ClearTimer(this, &AConvergenceCrystal::LoseHealthTimer);
		//convergence crystal just died, blow shit up, do stuff, idk...wait i found it out, end the boss
		ConvergenceCam->SetActive(false);
		if (player1) { Cast<APlayerController>(player1->GetController())->SetViewTarget(player1); }
		if (player2) { Cast<APlayerController>(player2->GetController())->SetViewTarget(player2); }
		//bossDoubt->EndOfBoss();//check if player controller is null
		bossBetrayal->EndOfBoss();
	}
	else
	{
		health -= drainHealthDamage;
	}
	UE_LOG(LogDistance, Warning, TEXT("crystal health: %f"), health);
}