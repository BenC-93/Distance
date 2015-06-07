// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"
#include "AIBoss.h"
#include "ConvergenceCrystal.h"

AConvergenceCrystal::AConvergenceCrystal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	health = 300.0f;
	maxHealth = 300.0f;

	healthLossRate = 0.5f;
	drainHealthDamage = 1.0f;

	movementSpeed = 450.0f; //speed in cm/s

	zflip = 1.f;
	zmin = 0.f;
	zmax = 25.f;

	p1Near = p2Near = false;

	LightIntensity = 10.0f * health;

	inDoubtBoss = false;
	doubtTriggers = false;
	oneDoubtTrigger = false;

	cameraOffset = FVector(100.0f, 0.0f, 0.0f);//used becuase all the clutter on the top of the hud, we move the focal point up more

	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	FocusPoint = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("FocusPoint"));
	FocusPoint->AttachTo(RootComponent);

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(0.f, 90.f, -70.f);
	SpriteComponent->AttachTo(RootComponent);
	SpriteComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(FocusPoint);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-70.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	ConvergenceCam = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("ConvergenceCam"));
	ConvergenceCam->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	ConvergenceCam->bUsePawnControlRotation = false;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->SourceRadius = 5.f;
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = true;
	LightSource->RelativeLocation.Z += 145.f;

	CaptureBox1 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CaptureBox1"));
	CaptureBox1->Mobility = EComponentMobility::Movable;
	CaptureBox1->bGenerateOverlapEvents = false;
	CaptureBox1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CaptureBox1->SetBoxExtent(FVector(1000.f, 10.f, 200.f), true);
	CaptureBox1->AttachTo(FocusPoint);
	CaptureBox1->SetRelativeLocation(FVector(0.f, 750.f, 0.f));
	//CaptureBox1->bVisible = true;
	//CaptureBox1->bHiddenInGame = false;
	

	CaptureBox2 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CaptureBox2"));
	CaptureBox2->Mobility = EComponentMobility::Movable;
	CaptureBox2->bGenerateOverlapEvents = false;
	CaptureBox2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CaptureBox2->SetBoxExtent(FVector(1000.f, 10.f, 200.f), true);
	CaptureBox2->AttachTo(FocusPoint);
	CaptureBox2->SetRelativeLocation(FVector(0.f, -750.f, 0.f));
	//CaptureBox2->bVisible = true;
	//CaptureBox2->bHiddenInGame = false;
	

	CaptureBox3 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CaptureBox3"));
	CaptureBox3->Mobility = EComponentMobility::Movable;
	CaptureBox3->bGenerateOverlapEvents = false;
	CaptureBox3->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CaptureBox3->SetBoxExtent(FVector(10.f, 1000.f, 200.f), true);
	CaptureBox3->AttachTo(FocusPoint);
	CaptureBox3->SetRelativeLocation(FVector(750.f, 0.f, 0.f));
	//CaptureBox3->bVisible = true;
	//CaptureBox3->bHiddenInGame = false;
	

	CaptureBox4 = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CaptureBox4"));
	CaptureBox4->Mobility = EComponentMobility::Movable;
	CaptureBox4->bGenerateOverlapEvents = false;
	CaptureBox4->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CaptureBox4->SetBoxExtent(FVector(10.f, 1000.f, 200.f), true);
	CaptureBox4->AttachTo(FocusPoint);
	CaptureBox4->SetRelativeLocation(FVector(-750.f, 0.f, 0.f));
	//CaptureBox4->bVisible = true;
	//CaptureBox4->bHiddenInGame = false;
	
}

void AConvergenceCrystal::BeginPlay()
{
	ConvergenceCam->SetActive(true);
	if (player1) { Cast<APlayerController>(player1->GetController())->SetViewTarget(this); }
	if (player2) { Cast<APlayerController>(player2->GetController())->SetViewTarget(this); }
	zmin += SpriteComponent->GetRelativeTransform().GetLocation().Z;
	zmax += SpriteComponent->GetRelativeTransform().GetLocation().Z;
}

void AConvergenceCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//move to inbetween players
	float tempH = (DeltaTime * zflip * 75.f) + SpriteComponent->RelativeLocation.Z;
	SpriteComponent->SetRelativeLocation(FVector(SpriteComponent->RelativeLocation.X, SpriteComponent->RelativeLocation.Y, fmax(zmin, fmin(zmax, tempH))));
	if (SpriteComponent->RelativeLocation.Z == zmax || SpriteComponent->RelativeLocation.Z == zmin)
	{
		zflip *= -1.f;
	}

	if (player1 && player2)
	{
		CameraBoom->TargetArmLength = fmax(900.f, (0.65f * FVector::Dist(player1->GetActorLocation(), player2->GetActorLocation())) + 650.f);
		doubtTriggers = false;
		oneDoubtTrigger = false;
		for (TActorIterator<AAIBoss_Doubt> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->p1InTrigger && ActorItr->p2InTrigger)//special conditions if the boss is doubt
			{
				doubtTriggers = true;
				oneDoubtTrigger = true;
			}
			else
			{
				//FocusPoint->SetWorldLocation(GetActorLocation());
			}
			if (ActorItr->p1InTrigger || ActorItr->p2InTrigger)
			{
				oneDoubtTrigger = true;
				inDoubtBoss = true;
			}
		}

		for (TActorIterator<AAIBoss> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			boss = Cast<AAIBoss>(*ActorItr);
		}
		
		
		FVector TargetLocation = (player1->GetActorLocation() + player2->GetActorLocation()) / 2;
		if (inDoubtBoss)
		{
			TargetLocation = (TargetLocation + boss->GetActorLocation()) / 2;
		}
		TargetLocation = TargetLocation + cameraOffset;
		FVector temp = TargetLocation - FocusPoint->GetComponentLocation();
		temp.Normalize();
		temp = (DeltaTime * temp * movementSpeed) + FocusPoint->GetComponentLocation();
		if (GetDistanceTo(player1) > 150.0f || GetDistanceTo(player2) > 150.0f)
		{
			if (FVector::Dist(TargetLocation, FocusPoint->GetComponentLocation()) > 50.0f)
			{
				FocusPoint->SetWorldLocation(temp);
			}
		}
		if (!p1Near && (FocusPoint->GetComponentLocation() - player1->GetActorLocation()).Size() < 400.f) { p1Near = true; }
		else { p1Near = false; }
		if (!p2Near && (FocusPoint->GetComponentLocation() - player2->GetActorLocation()).Size() < 400.f) { p2Near = true; }
		else { p2Near = false; }
		
		
		if (!doubtTriggers)
		{
			FVector TargetLocation = (player1->GetActorLocation() + player2->GetActorLocation()) / 2;
			FVector temp = TargetLocation - GetActorLocation();
			temp.Normalize();
			temp = (DeltaTime * temp * movementSpeed) + GetActorLocation();
			if (GetDistanceTo(player1) > 150.0f || GetDistanceTo(player2) > 150.0f)
			{
				if (FVector::Dist(TargetLocation, GetActorLocation()) > 50.0f)
				{
					SetActorLocation(temp);
				}
			}
		}
		
		if (p1Near && p2Near && !oneDoubtTrigger) 
		{ 
			TurnCaptureBoxesOn(); 
		}
		else
		{
			TurnCaptureBoxesOff();
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
		// play death sound
		BPPlayDeathSound();

		health = 0;
		boss->EndOfBoss();
		Destroy();
	}
	else
	{
		health -= drainHealthDamage;
		LightIntensity = 10.0f * health;
		LightSource->SetIntensity(LightIntensity);


		if (remainder(health, 25) == 0) {
			// play damage sound
			BPPlayDamageSound();
		}
	}
	//UE_LOG(LogDistance, Warning, TEXT("crystal health: %f"), health);
}

void AConvergenceCrystal::TurnCaptureBoxesOn()
{
	//UE_LOG(LogTemp, Warning, TEXT("TurnCaptureBoxOn() was called"));
	if (CaptureBox1->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Block)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Turning on capture box"));
		CaptureBox1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		CaptureBox2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		CaptureBox3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		CaptureBox4->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	}
}

void AConvergenceCrystal::TurnCaptureBoxesOff()
{
	//UE_LOG(LogTemp, Warning, TEXT("TurnCaptureBoxOn() was called"));
	if (CaptureBox1->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Ignore)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Turning on capture box"));
		CaptureBox1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CaptureBox2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CaptureBox3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CaptureBox4->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AConvergenceCrystal::Destroyed()
{
	
	GetWorldTimerManager().ClearAllTimersForObject(this);
	//convergence crystal just died, blow shit up, do stuff, idk...wait i found it out, end the boss
	
	ConvergenceCam->SetActive(false);
	GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);
	if (player1) { Cast<APlayerController>(player1->GetController())->SetViewTarget(player1); }
	if (player2) { Cast<APlayerController>(player2->GetController())->SetViewTarget(player2); }
}

