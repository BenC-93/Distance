// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "KismetMathLibrary.generated.h"
#include "ItemLantern.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	moveToPlayer = false;
	moveAway = false;
	player1 = NULL;
	player2 = NULL;

	prepareToDie = false;

	health = 0.0f;
	maxHealth = 100.0f;
	baseDamage = -1.0f;
	drainRate = 0.1f;//half or tenth of a second

	deathCounter = 10;

	scaleCounter = 4.0f;
	scaleLimit = 7.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITriggerRange = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerRange"));
	AITriggerRange->Mobility = EComponentMobility::Movable;
	AITriggerRange->SetBoxExtent(FVector(900.0f, 900.0f, 60.0f), true);
	AITriggerRange->AttachTo(RootComponent);

	AITriggerRange->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBegin);
	AITriggerRange->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEnd);

	ShadowSpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("ShadowSpriteComponent"));
	ShadowSpriteComponent->RelativeRotation = FRotator(0, 90, -90);//y,z,x
	ShadowSpriteComponent->RelativeLocation = FVector(-58.0f, -5.0f, -42.0f);
	ShadowSpriteComponent->SetRelativeScale3D(FVector(1, 1, 5));
	ShadowSpriteComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	ShadowSpriteComponent->AttachTo(RootComponent);

	DrainParticleSys = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("DrainParticleSys"));
	//Reference to particle system asset
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem
		(TEXT("ParticleSystem'/Game/MyParticleSystem/Sprite/P_Drain'"));

	//Set Particle system's template
	DrainParticleSys->SetTemplate(ParticleSystem.Object);
	DrainParticleSys->bAutoDestroy = false;
	DrainParticleSys->bAutoActivate = false;
	DrainParticleSys->bIsActive = false;
	DrainParticleSys->RelativeLocation = FVector(0, 0, 100);
	DrainParticleSys->RelativeRotation = FRotator(-90, 0, 0);//y,z,x
	DrainParticleSys->AttachTo(ShadowSpriteComponent);

	AITriggerAttack = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerAttack"));
	AITriggerAttack->Mobility = EComponentMobility::Movable;
	AITriggerAttack->SetBoxExtent(FVector(88.99f,128.65f, 50.0f), true);//was 300, 300, 60
	AITriggerAttack->RelativeLocation = FVector(5.0f, 26.0f, 120.0f);
	AITriggerAttack->AttachTo(ShadowSpriteComponent);

	//AITriggerAttack->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnOverlapBeginAttack);
	//AITriggerAttack->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnOverlapEndAttack);
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Delta Time, %f"), DeltaTime);
	
	//UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), GetVelocity().Size());
	if (prepareToDie && deathCounter == 0 && GetVelocity().Size() == 0)//after running away
	{
		//player->GetCharacterMovement()->MaxWalkSpeed = 600;
		Destroy();
	}
	if (prepareToDie)//used for buffer time so the ai can run away
	{
		if (deathCounter == 10)//right before running away
		{
			player->ChangeSpeed(600);
			ShadowSpriteComponent->SetRelativeScale3D(FVector(1, 1, 1));
			DrainParticleSys->Deactivate();
			DrainParticleSys->DestroyComponent();
			//DrainParticleSys->bIsActive = false;
		}
		if (deathCounter - 1 >= 0)//used to decrement the death counter and so it doesnt go below 0
		{
			deathCounter--;
		}
	}

	if (player != NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Distance to Player: %f"), GetDistanceTo(player));
		if (moveToPlayer && !GetWorldTimerManager().IsTimerActive(this, &AAIEnemy::DrainTimer))//Determines if ai moves faster towards player or player moves slower trying to escape
		{
			float distToPlayer = GetDistanceTo(player);
			//orient shadow towards player//y,z,x
			float myYaw = FaceActorRotation(player).Yaw;
			ShadowSpriteComponent->SetWorldRotation(FRotator(0, myYaw + 90, -90));
			//scale shadow to the player

			FRotator playerDirection = player->GetVelocity().Rotation();

			float playerYaw = playerDirection.Yaw;

			playerYaw = ConvertToUnitCircle(playerYaw);
			myYaw = ConvertToUnitCircle(myYaw);
			
			//UE_LOG(LogTemp, Warning, TEXT("Shadow Yaw: %f, Player Yaw: %f"), myYaw, playerYaw);

			float range = 10.0f;
			if (myYaw > playerYaw - range && myYaw < playerYaw + range)//going in same direction
			{//grow shadow even faster
				//UE_LOG(LogTemp, Warning, TEXT("Same direction"));//speed up ai
				if (scaleCounter + DeltaTime < scaleLimit)
				{
					scaleCounter += 5 * DeltaTime;
					//UE_LOG(LogTemp, Warning, TEXT("AI speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
				}
			}
			else if (myYaw > playerYaw + 180 - range && myYaw < playerYaw + 180 + range)//going in opposite directions
			{//shrink shadow
				//UE_LOG(LogTemp, Warning, TEXT("Opposite direction"));
				if (scaleCounter > 0.8 && scaleCounter - DeltaTime > 0)
				{
					scaleCounter -= 1 * DeltaTime;
					//health += 2;//make enemy full when approaching?
					//slow down player?
					float playerSpeed = player->GetCharacterMovement()->MaxWalkSpeed;
					player->GetCharacterMovement()->MaxWalkSpeed =  FMath::Lerp(200.0f, playerSpeed, (scaleCounter/scaleLimit));
					//UE_LOG(LogTemp, Warning, TEXT("Player speed: %f"), player->GetCharacterMovement()->MaxWalkSpeed);
				}
				/*GetCharacterMovement()->MaxWalkSpeed = runAwaySpeed;
				speedCounter = 1.0f;
				RunAway();*/
			}
			else
			{//not needed, just for testing purposes
				//UE_LOG(LogTemp, Warning, TEXT("some other direction"));
				if (scaleCounter + DeltaTime < scaleLimit)//in general, grow
				{
					scaleCounter += 2 * DeltaTime;
					//UE_LOG(LogTemp, Warning, TEXT("AI speed: %f"), GetCharacterMovement()->MaxWalkSpeed);
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("ShadowLen: %f, DistToPlayer: %f"), (scaleCounter * spriteLen), distToPlayer);
			if (spriteLen * scaleCounter > distToPlayer)//if the player is within the shadow becuase the shadow is too big and doesnt follow the player,
			{
				if (scaleCounter > 0.8 && scaleCounter - DeltaTime > 0)
				{
					scaleCounter -= 4 * DeltaTime;
				}
			}

			ShadowSpriteComponent->SetRelativeScale3D(FVector(1, 1, scaleCounter));
			//AITriggerAttack->AddLocalTransform(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1, 1, scaleCounter - 1)));
			
			if (scaleCounter <= 0.8)//we have shrunk too far, now we run away
			{
				RunAway();
			}
			//UE_LOG(LogTemp, Warning, TEXT("scaleCounter: %f"), scaleCounter);
			
		}
	}
}

void AAIEnemy::RunAway()
{
	GetCharacterMovement()->MaxWalkSpeed = runAwaySpeed;
	moveToPlayer = false;
	moveAway = true;
	GetWorldTimerManager().ClearTimer(this, &AAIEnemy::DrainTimer);
	DrainParticleSys->Deactivate();
}

void AAIEnemy::DrainTimer()
{
	if (player != NULL)
	{
		if (health >= maxHealth)//Ai is full, move away
		{
			RunAway();
			return;
		}
		if (player->Health == 0)//player health has depleted, move away
		{
			RunAway();
			return;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Equipped item name, %s"), *player->GetItemName());
		if (moveToPlayer && player->GetItem()->IsA(AItemLantern::StaticClass()) && player->GetItemAmount() > 0.0f && player->GetItemEnabled())
		{
			//drain light
			if (health < maxHealth)
			{
				health += 1;
				player->ChangeItemAmount(baseDamage);
				//UE_LOG(LogTemp, Warning, TEXT("Light decremented, %f"), player->GetItemAmount());
			}
		}
		else//drain health
		{
			if (moveToPlayer && health < maxHealth)
			{
				//UE_LOG(LogTemp, Error, TEXT("Equipped item name, %s"), *player->GetItemName());
				health += 1;
				player->ChangeHealth(baseDamage);
				//UE_LOG(LogTemp, Warning, TEXT("Health decremented, %f"), player->Health);
			}
		}
	}
}

void AAIEnemy::StartDrainTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIEnemy::DrainTimer);
	GetWorldTimerManager().SetTimer(this, &AAIEnemy::DrainTimer, rate, true);
}

void AAIEnemy::ChangeHealth(float amount)
{

}

void AAIEnemy::Attack(float amount)
{

}

float AAIEnemy::ConvertToUnitCircle(float degrees)//convert to 0 - 360
{
	float unrealDeg = degrees;
	if (unrealDeg > 0)//positive
	{
		if (unrealDeg <= 90)
		{
			unrealDeg = 90 - unrealDeg;
		}
		else
		{
			unrealDeg = 360 - (unrealDeg - 90);
		}
	}
	else if (unrealDeg <= 0)//negative
	{
		unrealDeg = FMath::Abs(unrealDeg) + 90;
	}
	return unrealDeg;	//it has been converted to normal unit circle lingo
}

FRotator AAIEnemy::FaceActorRotation(class AActor* OtherActor)//gets the angle needed to turn to another actor
{
	FVector playerLoc = OtherActor->GetActorLocation();
	FVector myLoc = GetActorLocation();
	FVector difference = playerLoc - myLoc;
	FRotator angleToRotate = difference.Rotation();
	//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), angleToRotate.Yaw);
	return angleToRotate;

	/*float distToPlayer = GetDistanceTo(OtherActor);//hyp
	float deltaX = playerLoc.X - GetActorLocation().X;//opp
	float deltaY = playerLoc.Y - GetActorLocation().Y;//adj
	float angle = 0;
	//angle = FMath::RadiansToDegrees(FMath::Atan(deltaX / deltaY));
	//angle = FMath::RadiansToDegrees(FMath::Asin(deltaX / distToPlayer));

	if (deltaX >= 0)
	{
		//angle = FMath::RadiansToDegrees(FMath::Atan(deltaX / deltaY));
		angle = FMath::RadiansToDegrees(FMath::Asin(deltaX / distToPlayer));
	}
	else//do i make deltaY * -1 here?
	{
		//angle = FMath::RadiansToDegrees(FMath::Atan(FMath::Abs(deltaY / deltaX)));
		angle = FMath::RadiansToDegrees(FMath::Acos(deltaX / distToPlayer));
	}
	UE_LOG(LogTemp, Warning, TEXT("DeltaX: %f, DeltaY: %f, Angle: %f"), deltaX, deltaY, angle);
	return FRotator(0, angle, 0);*/
}

void AAIEnemy::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("-----Player Entered Triggered Area"));
			moveToPlayer = true;//outer trigger boolean
			moveAway = false;
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			//orient shadow towards player//y,z,x
			ShadowSpriteComponent->SetWorldRotation(FRotator(0, FaceActorRotation(player).Yaw + 90, -90));
		}
	}
}

void AAIEnemy::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("-------Player Exited Triggered Area fool!"));
		}
	}
}

void AAIEnemy::OnAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Entered drain trigger"));
			//UE_LOG(LogTemp, Warning, TEXT("Slowed Player1 and beginning drain"));
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);
			player->ChangeSpeed(400);//Works, but when do we set it back to normal??
			//GetWorldTimerManager().SetTimer(this, &AAIEnemy::Drain, 1.0f, true);

			StartDrainTimer(drainRate);

			DrainParticleSys->Activate();
		}
	}
}

void AAIEnemy::OnExitAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Exited drain trigger."));
			GetWorldTimerManager().ClearTimer(this, &AAIEnemy::DrainTimer);
			DrainParticleSys->Deactivate();
		}
	}
}

bool AAIEnemy::CheckIfPlayer(class AActor* OtherActor)
{
	// Just do a simple check to see if it can cast properly
	return Cast<ADistanceCharacter>(OtherActor) != NULL;
}

