// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "DistancePlayerController.h"
#include "AIBoss_Betrayal.h"
#include "AIBoss_Betrayal_Minion.h"
#include "ItemLightBeam.h"

AItemLightBeam::AItemLightBeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemName = "LightBeam";
	chargeRate = 0.1f;
	chargeAmount = 0.5f;
	totalChargedAmount = 0.0f;
	lightChargeAmount = 0.0f;

	targetActor = NULL;
	hasAttacked = false;//so the tick if statement only happens once per attack

	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("TriggerBox"));
	TriggerBox->Mobility = EComponentMobility::Movable;
	TriggerBox->AttachTo(RootComponent);
	TriggerBox->SetBoxExtent(FVector(70.0f, 50.0f, 125.0f), true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemLightBeam::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AItemLightBeam::OnOverlapEnd);

	LightIntensity = 100.0f * lightChargeAmount;

	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;
}

void AItemLightBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!hasAttacked && targetActor && GetWorldTimerManager().IsTimerActive(this, &AItemLightBeam::AnimationTimer))
	{
		//UE_LOG(LogDistance, Error, TEXT("timer active and targetActor is: %s"), *targetActor->GetName());
		//check for boss collision stuff
		if (targetActor->IsA(ATentacle::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the tentacle!"));
			Cast<ATentacle>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
		}
		else if (targetActor->IsA(AAIBoss_Betrayal::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the betrayal boss!"));
			Cast<AAIBoss_Betrayal>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
		}
		else if (targetActor->IsA(AAIBoss_Betrayal_Minion::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the betrayal minion!"));
			Cast<AAIBoss_Betrayal_Minion>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
		}
		else if (targetActor->IsA(ADistanceCharacter::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the the other player!"));
			Cast<ADistanceCharacter>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
		}
	}
}

void AItemLightBeam::StartUse()
{
	if (!bIsInUse && ItemAmount > 0 && bCanUse == true)
	{
		lightChargeAmount = 0.0f;
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::Charge, chargeRate, true);
		//start charging animation
		LightSource->SetVisibility(true);
		bIsInUse = true;
		GetWorldTimerManager().ClearTimer(this, &AItemLightBeam::Regenerate);
	}
	
}

void AItemLightBeam::EndUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (bIsInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemLightBeam::Charge);
		bIsInUse = false;
		bCanUse = false;
		//start attack animation
		
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::AnimationTimer, 0.85f, false);
		LightSource->SetVisibility(false);
		GetWorldTimerManager().SetTimer(this, &AItemLightBeam::Regenerate, RegenRate, true);
	}
}

void AItemLightBeam::AnimationTimer()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	playerController->canMove = true;
	bCanUse = true;
	hasAttacked = false;
	GetOwningPawn()->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void AItemLightBeam::Charge()
{
	ChangeAmount(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
	lightChargeAmount += chargeAmount;

	// light increases while charging
	LightIntensity = 100.0f * lightChargeAmount;
	LightSource->SetIntensity(LightIntensity);
}

void AItemLightBeam::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		targetActor = OtherActor;
		UE_LOG(LogDistance, Warning, TEXT("Overlaped with: %s"), *targetActor->GetName());
	}
}

void AItemLightBeam::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//UE_LOG(LogDistance, Warning, TEXT("Exit Overlap with: %s"), *OtherActor->GetName());
		targetActor = NULL;
		//UE_LOG(LogDistance, Warning, TEXT("TargetActor is NULL"));
	}
}