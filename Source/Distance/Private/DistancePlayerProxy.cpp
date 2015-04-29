// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistancePlayerProxy.h"
//#include "DistancePlayerController.h"
#include "DistanceCharacter.h"
#include "AIController.h"
#include "UnrealNetwork.h"

ADistancePlayerProxy::ADistancePlayerProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bReplicateMovement = true;
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
 
	// It seems that without a RootComponent, we can't place the Actual Character easily
	USceneComponent* RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
 
	// Create a camera boom...
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-70.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
 
	// Create a camera...
	TopDownCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
 
	if (Role == ROLE_Authority)
	{
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/2DSpriteCharacter"));
		CharacterClass = PlayerPawnBPClass.Class;
	}
 
}

void ADistancePlayerProxy::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		// Get current location of the Player Proxy
		FVector Location =  GetActorLocation();
		FRotator Rotation = GetActorRotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;
		
		// Spawn the actual player character at the same location as the Proxy
		Character = Cast<ADistanceCharacter>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, SpawnParams));
		
		// Spawn with lantern
		Character->PickupItem(GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnLantern(Character));
		Character->EquipItemComponent(0);
		Character->ItemPickedUp();
		
		// We use the PlayerAI to control the Player Character for Navigation
		PlayerAI = GetWorld()->SpawnActor<AAIController>(GetActorLocation(), GetActorRotation());
		PlayerAI->Possess(Character);
	}
 
}

void ADistancePlayerProxy::Tick(float DeltaTime)
{
 
	Super::Tick(DeltaTime);
	if (Character)
	{
		// Keep the Proxy in sync with the real character
		FTransform CharTransform = Character->GetTransform();
		FTransform MyTransform = GetTransform();
		
		FTransform Transform;
		Transform.LerpTranslationScale3D(CharTransform, MyTransform, ScalarRegister(0.5f));
		
		SetActorTransform(Transform);
		
	}
}

void ADistancePlayerProxy::MoveToLocation(const ADistancePlayerController* controller, const FVector& DestLocation)
{
	/** Looks easy - doesn't it.
	 *  What this does is to engage the AI to pathfind.
	 *  The AI will then "route" the character correctly.
	 *  The Proxy (and with it the camera), on each tick, moves to the location of the real character
	 *
	 *  And thus, we get the illusion of moving with the Player Character
	 */
	PlayerAI->MoveToLocation(DestLocation);
}

void ADistancePlayerProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
	// Replicate to Everyone
	DOREPLIFETIME(ADistancePlayerProxy, Character);
}




