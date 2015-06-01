// Copyright OvertimeGames 2015

#include "Distance.h"
#include "DistanceCharacter.h"
#include "DistancePlayerController.h"
#include "AIBoss_Betrayal.h"
#include "AIBoss_Betrayal_Minion.h"
#include "ItemDagger.h"

AItemDagger::AItemDagger(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ItemName = "Dagger";
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
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemDagger::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AItemDagger::OnOverlapEnd);
	
	LightIntensity = 100.0f * lightChargeAmount;
	
	LightSource = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "LightSource");
	LightSource->AttachTo(RootComponent);
	LightSource->Intensity = LightIntensity;
	LightSource->bVisible = false;
}

void AItemDagger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (hasAttacked == false && targetActor && GetWorldTimerManager().IsTimerActive(this, &AItemDagger::AnimationTimer))
	{
		//UE_LOG(LogDistance, Error, TEXT("timer active and targetActor is: %s"), *targetActor->GetName());
		//check for boss collision stuff
		if (targetActor->IsA(AAIBoss_Betrayal::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the betrayal boss!"));
			Cast<AAIBoss_Betrayal>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
			targetActor = NULL;
		}
		else if (targetActor->IsA(AAIBoss_Betrayal_Minion::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the betrayal minion!"));
			Cast<AAIBoss_Betrayal_Minion>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
			targetActor = NULL;
		}
		else if (targetActor->IsA(ADistanceCharacter::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Light beam hit the the other player!"));
			Cast<ADistanceCharacter>(targetActor)->ChangeHealth(-1 * (1 + totalChargedAmount));
			totalChargedAmount = 0.0f;
			hasAttacked = true;
			targetActor = NULL;
		}
	}
}

void AItemDagger::StartUse()
{
	if (!bIsInUse && ItemAmount > 0 && bCanUse == true)
	{
		lightChargeAmount = 0.0f;
		GetWorldTimerManager().SetTimer(this, &AItemDagger::Charge, chargeRate, true);
		//start charging animation
		LightSource->SetVisibility(true);
		bIsInUse = true;
		GetWorldTimerManager().ClearTimer(this, &AItemDagger::Regenerate);
	}
	
}

void AItemDagger::EndUse()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	if (bIsInUse)
	{
		GetWorldTimerManager().ClearTimer(this, &AItemDagger::Charge);
		bIsInUse = false;
		bCanUse = false;
		//start attack animation and sound
		BPPlaySound();
		playerController->canMove = false;
		GetOwningPawn()->GetMesh()->PlayAnimation(UseAnimation, false);
		GetWorldTimerManager().SetTimer(this, &AItemDagger::AnimationTimer, 0.85f, false);
		
		LightSource->SetVisibility(false);
		GetWorldTimerManager().SetTimer(this, &AItemDagger::Regenerate, RegenRate, true);
	}
}

void AItemDagger::AnimationTimer()
{
	class ADistancePlayerController* playerController = Cast<ADistancePlayerController>(GetOwningPawn()->GetController());
	playerController->canMove = true;
	bCanUse = true;
	hasAttacked = false;
	GetOwningPawn()->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void AItemDagger::Charge()
{
	ChangeAmount(-chargeAmount);
	totalChargedAmount += chargeAmount;//might need to multiply by some factor for more or less damage dealing
	lightChargeAmount += chargeAmount;
	
	// light increases while charging
	LightIntensity = 100.0f * lightChargeAmount;
	LightSource->SetIntensity(LightIntensity);
}

void AItemDagger::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor != GetOwningPawn())
	{
		targetActor = OtherActor;
		//UE_LOG(LogDistance, Warning, TEXT("Overlaped with: %s"), *targetActor->GetName());
	}
}

void AItemDagger::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		//UE_LOG(LogDistance, Warning, TEXT("Exit Overlap with: %s"), *OtherActor->GetName());
		targetActor = NULL;
		//UE_LOG(LogDistance, Warning, TEXT("TargetActor is NULL"));
	}
}


