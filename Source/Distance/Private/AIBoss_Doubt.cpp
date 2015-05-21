// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIBoss_Doubt.h"
#include "UnrealNetwork.h"
#include "ItemShield.h"
#include "ConvergenceCrystal.h"
#include "Tentacle.h"

AAIBoss_Doubt::AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;//TODO: will change
	baseDamage = -1.0f;//TODO: will change along with other vars here during playtesting
	drainRate = 0.25f;
	tentacleHealth = 10.0f;
	numTentacles = 4;

	targetActor = NULL;

	distToPlayer = 1;
	tentacleYaw = 0;
	chosenTentacleIndex = 0;
	tentacleCounter = 0;
	tentacleSpriteLen = 300.0f;//maybe 600, works with 650

	swallowedPlayer = NULL;

	p1InTrigger = false;
	p2InTrigger = false;
	//capsule component 360 by 360
	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITriggerRange = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerRange"));
	AITriggerRange->Mobility = EComponentMobility::Movable;
	AITriggerRange->SetBoxExtent(FVector(1600.0f, 1600.0f, 500.0f), true);
	AITriggerRange->AttachTo(RootComponent);

	AITriggerRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBoss_Doubt::OnOverlapBegin);
	AITriggerRange->OnComponentEndOverlap.AddDynamic(this, &AAIBoss_Doubt::OnOverlapEnd);

	AITriggerAttack = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerAttack"));
	AITriggerAttack->Mobility = EComponentMobility::Movable;
	AITriggerAttack->SetBoxExtent(FVector(750.0f, 650.0f, 500.0f), true);
	AITriggerAttack->AttachTo(RootComponent);

	TentacleComponent0 = CreateTentacleComponent(0, ObjectInitializer);
	TentacleComponent1 = CreateTentacleComponent(1, ObjectInitializer);
	TentacleComponent2 = CreateTentacleComponent(2, ObjectInitializer);
	TentacleComponent3 = CreateTentacleComponent(3, ObjectInitializer);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
}

UChildActorComponent* AAIBoss_Doubt::CreateTentacleComponent(int i, const FObjectInitializer& ObjectInitializer)
{
	
	FName componentName = FName(*FString::Printf(TEXT("TentacleComponent %d"), i));
	class UChildActorComponent* TentacleComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, componentName);
	//TentacleComponent->ChildActorClass = ATentacle::StaticClass(); //not needed anymore
	TentacleComponent->OnComponentCreated();
	TentacleComponent->RelativeLocation = FVector(-465.0f, -370.0f + (i * 170.0f), -280.0f + (i * 3.0f));//add y by 170, add z by 3 each tentacle
	TentacleComponent->AttachTo(RootComponent);
	TentacleComponent->SetIsReplicated(true);
	
	// Add TC to Array of components
	TentacleComponentArray.Add(TentacleComponent);
	return TentacleComponent;
}

/*void AAIBoss_Doubt::PostLoad()
{
	Super::PostLoad();
	for (int i = 0; i < TentacleComponentArray.Num(); i++)
	{
		ATentacle* tentacle = ((ATentacle *)TentacleComponentArray[i]->ChildActor);
		UE_LOG(LogTemp, Warning, TEXT("this line is just before setting tentacle parents"));
		if (TentacleComponentArray[i]->ChildActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("TentaclesComponentArray is good to go my good sir."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TentacleComponentArray is NULL"));
		}
		if (tentacle != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tentacle parents are set"));
			tentacle->SetBossParent(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Tentacles are NULL"));
		}
	}
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
}

void AAIBoss_Doubt::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	for (int i = 0; i < TentacleComponentArray.Num(); i++)
	{
		ATentacle* tentacle = ((ATentacle *)TentacleComponentArray[i]->ChildActor);
		UE_LOG(LogTemp, Warning, TEXT("PostInitializeComponents: this line is just before setting tentacle parents"));
		if (TentacleComponentArray[i]->ChildActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("PostInitializeComponents: TentaclesComponentArray is good to go my good sir."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PostInitializeComponents: TentacleComponentArray is NULL"));
		}
		if (tentacle != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("PostInitializeComponents: Tentacle parents are set"));
			tentacle->SetBossParent(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PostInitializeComponents: Tentacles are NULL"));
		}
	}
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
}*/

/*void AAIBoss_Doubt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AAIBoss_Doubt, tentacleArray);
}*/

void AAIBoss_Doubt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//float temptentaclehealth = ((ATentacle *)TentacleComponent->ChildActor)->health;
	//UE_LOG(LogTemp, Warning, TEXT("Tentacle Health: %f"), temptentaclehealth);
	if (player != NULL)
	{
		if (Health > 0)
		{
			if (p1InTrigger && p2InTrigger)//checks and sets the current player target to the closest player
			{
				ACharacter *closestPlayer = ClosestPlayer();
				if (closestPlayer != player)
				{
					if (closestPlayer == player1)
					{
						currentPlayer = player1;
						player = Cast<ADistanceCharacter>(currentPlayer);
						playerController = Cast<ADistancePlayerController>(player->GetController());
						//printScreen(FColor::Red, "Boss targeting: Player1");
					}
					else
					{
						currentPlayer = player2;
						player = Cast<ADistanceCharacter>(currentPlayer);
						playerController = Cast<ADistancePlayerController>(player->GetController());
						//printScreen(FColor::Red, "Boss targeting: Player2");
					}
				}
			}
		}
	}
	//if swallowedPlayer == player -> then currentPlayer needs to be other player
	if (swallowedPlayer && player && !targetActor)
	{
		if (player == swallowedPlayer)
		{
			printScreen(FColor::Red, TEXT("Resetting Target, one player has been swallowed"));
			if (player == player1)
			{
				currentPlayer = player2;
				player = Cast<ADistanceCharacter>(currentPlayer);
				playerController = Cast<ADistancePlayerController>(player->GetController());
				UE_LOG(LogTemp, Warning, TEXT("current player switched to second player"));
			}
			else
			{
				currentPlayer = player1;
				player = Cast<ADistanceCharacter>(currentPlayer);
				playerController = Cast<ADistancePlayerController>(player->GetController());
				UE_LOG(LogTemp, Warning, TEXT("current player switched to first player"));
			}
			StartAttackTimer(3.0f);//attack the new target
		}
	}
	if (swallowedPlayer)
	{
		//class ADistancePlayerController* swallowedController = Cast<ADistancePlayerController>(swallowedPlayer->GetController());
		if (playerController->switchedItem)//other player, not the swallowed one
		{
			printScreen(FColor::Red, TEXT("Swallowed Player was Released by other player switching Items"));
			ReleasePlayer(swallowedPlayer);
			playerController->switchedItem = false;
		}
	}
	else
	{
		if (player1 && player2)
		{
			ADistancePlayerController *player1Controller = Cast<ADistancePlayerController>(player1->GetController());
			ADistancePlayerController *player2Controller = Cast<ADistancePlayerController>(player2->GetController());
			if (player1Controller->switchedItem)
			{
				player1Controller->switchedItem = false;
			}
			if (player2Controller->switchedItem)
			{
				player2Controller->switchedItem = false;
			}
		}
	}
}

void AAIBoss_Doubt::PullPlayer(class ACharacter* tempChar)
{
	if (tempChar == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: PullPlayer, tempChar is Null."));
		return;
	}

	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (tempPlayer->GetItem() == NULL)
	{
		printScreen(FColor::Red, TEXT("Player's item is NULL! LOOK, THIS IS BAD!"));
		UE_LOG(LogTemp, Error, TEXT("Error: PullPlayer: player's item is NULL!"));
		tempPlayer->ChangeSpeed(100);
	}
	else if (tempPlayer->GetItem()->IsA(AItemShield::StaticClass()) && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)
	{
		tempPlayer->ChangeSpeed(50);//shield is enabled
	}
	else
	{
		tempPlayer->ChangeSpeed(100);
	}

	tempPlayerController->canMove = false;
	tempPlayerController->SetNewMoveDestination(GetActorLocation());

	//GetWorld()->GetAuthGameMode<ADistanceGameMode>()->SpawnTentacleAtLocation(TentacleClass, tempPlayer->GetActorLocation() - FVector(150.0f, 0.0f, 0.0f));
}

void AAIBoss_Doubt::PullActor(class AActor* tempActor)
{
	if (tempActor == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: PullActor, tempActor is Null."));
		return;
	}
	FVector direction = tempActor->GetActorLocation() - GetActorLocation();
	direction.Normalize();
	float speed = 10.0f;
	FVector newLoc = (direction * speed) + GetActorLocation();
	//UE_LOG(LogDistance, Error, TEXT("crystal new loc: (%f, %f, %f)"), newLoc.X, newLoc.Y, newLoc.Z);
	if (GetDistanceTo(tempActor) > 150.0f)
	{
		tempActor->SetActorLocation(newLoc);
	}

}

void AAIBoss_Doubt::ActorPullTimer()
{
	if (targetActor)
	{
		PullActor(targetActor);
	}
	else
	{
		UE_LOG(LogDistance, Error, TEXT("TargetActor is NULL"));
	}
}

void AAIBoss_Doubt::ReleasePlayer(class ACharacter* tempChar)
{
	if (tempChar == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: ReleasePlayer, tempChar is Null."));
		return;
	}

	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (!tempPlayerController->canMove)//if they were under ai control, stop moving them
	{
		tempPlayerController->SetNewMoveDestination(tempPlayer->GetActorLocation() - FVector(121.0f, 0.0f, 0.0f));//for stopping the last forced move
	}
	tempPlayerController->canMove = true;
	tempPlayer->ChangeSpeed(600);
	if (tempPlayer == swallowedPlayer)//if releasing swallowed player -> stop its timer, and reset swallowed player to null so we can get a current closest player again (might happen instantly, idk)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
		swallowedPlayer = NULL;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	//printScreen(FColor::Red, "Player was Released Normally");
}

void AAIBoss_Doubt::DrainPlayer(class ADistanceCharacter* tempPlayer)
{
	if (tempPlayer == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer, tempPlayer is Null."));
		return;
	}

	if (tempPlayer->GetItem() == NULL)
	{
		printScreen(FColor::Red, TEXT("Player's item is NULL! LOOK, THIS IS BAD!"));
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer: player's item is NULL!"));
		tempPlayer->ChangeHealth(baseDamage);
	}
	else if (tempPlayer->GetItem()->IsA(AItemShield::StaticClass()) && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)
	{
		tempPlayer->ChangeItemAmount(baseDamage);//shield is enabled
	}
	else
	{
		tempPlayer->ChangeHealth(baseDamage);
	}
}

void AAIBoss_Doubt::ActorDrainTimer()
{
	if (targetActor)
	{
		DrainActor(targetActor);
	}
	else
	{
		UE_LOG(LogDistance, Error, TEXT("TargetActor is NULL"));
	}
}

void AAIBoss_Doubt::DrainActor(class AActor* tempActor)
{
	if (tempActor)
	{
		//check if crystal
		if (tempActor->IsA(AConvergenceCrystal::StaticClass()))
		{
			class AConvergenceCrystal* tempCrystal = Cast<AConvergenceCrystal>(tempActor);
			tempCrystal->drainHealthDamage = 5.0f;//change the amount the crystal normally takes damage
		}
	}
	else
	{
		UE_LOG(LogDistance, Error, TEXT("tempActor is NULL"));
	}
}

FRotator AAIBoss_Doubt::FaceActorRotation(FVector myActorLoc, class AActor* OtherActor)//gets the angle needed to turn to another actor
{
	FVector playerLoc = OtherActor->GetActorLocation();
	FVector myLoc = myActorLoc;
	FVector difference = playerLoc - myLoc;
	FRotator angleToRotate = difference.Rotation();
	//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), angleToRotate.Yaw);
	return angleToRotate;
}

void AAIBoss_Doubt::TentacleTimer()
{
	if (tentacleCounter >= 1.0f)
	{
		printScreen(FColor::Red, TEXT("Stopping tentacle timer"));
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::TentacleTimer);
		return;
	}
	if (currentPlayer->GetActorLocation().Y > GetActorLocation().Y)
	{
		//rotate //y,z,x
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw - 195), tentacleCounter), 0));//tentacleYaw + 180 works but wrong rotation wise, tentacleYaw - 217 works for rotation wise and for boss being base, 195 seems to work from tentacle itself
	}
	else
	{
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw + 180), tentacleCounter), 0));
	}
	//scale
	float maxScale = distToPlayer / tentacleSpriteLen;
	//UE_LOG(LogTemp, Error, TEXT("Max Scale: %f"), maxScale);
	TentacleComponentArray[chosenTentacleIndex]->SetRelativeScale3D(FVector(FMath::Lerp(0.5f, maxScale, tentacleCounter), 1, 1));//havnt found correct scale yet
	
	tentacleCounter += 0.1f;
	//UE_LOG(LogTemp, Error, TEXT("Tentacle Counter: %f"), tentacleCounter);
}

void AAIBoss_Doubt::StartTentacleTimer(float rate)
{
	tentacleCounter = 0.0f;
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::TentacleTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::TentacleTimer, rate, true);
}

void AAIBoss_Doubt::AttackTimer()
{
	printScreen(FColor::Red, TEXT("Boss making an attack"));
	//PullPlayer(player);//create tentacle, move tentacle
	chosenTentacleIndex = numTentacles - 1; //FMath::RandRange(0, numTentacles - 1);
	AActor* tentacleActor = TentacleComponentArray[chosenTentacleIndex]->ChildActor;
	if (tentacleActor)
	{
		distToPlayer = tentacleActor->GetDistanceTo(currentPlayer);
		tentacleYaw = FaceActorRotation(TentacleComponentArray[chosenTentacleIndex]->GetComponentLocation(), currentPlayer).Yaw;
	}
	else
	{
		distToPlayer = GetDistanceTo(currentPlayer);
		tentacleYaw = FaceActorRotation(TentacleComponentArray[chosenTentacleIndex]->GetComponentLocation(), currentPlayer).Yaw;
	}
	StartTentacleTimer(0.1f);

	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
}

void AAIBoss_Doubt::StartAttackTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::AttackTimer, rate, true);
}

void AAIBoss_Doubt::DrainTimer()
{
	//printScreen(FColor::Red, "Draining");
	DrainPlayer(player);
	if (player->Health == 0)//we "killed" the player, oops lol
	{
		//ReleasePlayer(player);
		//EndOfBoss();
	}
}

void AAIBoss_Doubt::StartDrainTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::DrainTimer, rate, true);
}

void AAIBoss_Doubt::SwallowedTimer()
{
	if (swallowedPlayer == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: SwallowedTimer, swallowedPlayer is Null."));
		return;
	}
	//printScreen(FColor::Red, "Draining Swallowed Player");
	DrainPlayer(swallowedPlayer);
	if (swallowedPlayer->Health == 0)//we "killed" the player, oops lol
	{
		//ReleasePlayer(swallowedPlayer);
	}
}

void AAIBoss_Doubt::StartSwallowedTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::SwallowedTimer, rate, true);
}

void AAIBoss_Doubt::EndOfBoss()
{
	if (swallowedPlayer != NULL)//or just realease p1 and p2
	{
		ReleasePlayer(swallowedPlayer);
	}
	if (playerController && !playerController->canMove)
	{
		ReleasePlayer(player);
		player = NULL;
	}

	ReleasePlayer(player1);
	ReleasePlayer(player2);

	GetWorldTimerManager().ClearAllTimersForObject(this);

	//GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);//if the clear all timers thing works, then we dont need these lines
	//GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
	//GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);
	printScreen(FColor::Red, TEXT("End of Boss"));

	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player = Cast<ADistanceCharacter>(currentPlayer);
	playerController = Cast<ADistancePlayerController>(player->GetController());
	if (playerController)
	{
		playerController->OnConvergenceEnd();
		GetWorld()->GetGameViewport()->SetDisableSplitscreenOverride(false);
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: End of Boss, playerController is null"));
	}
}

void AAIBoss_Doubt::ChangeHealth(float healthAmount)
{
	float tempHealth = Health + healthAmount;
	if (tempHealth <= MaxHealth)
	{
		if (player)
		{
			ReleasePlayer(player);
		}
		numTentacles--;
		//UChildActorComponent* tempTentacle = TentacleComponentArray[numTentacles];
		//TentacleComponentArray.RemoveAt(numTentacles);
		//tempTentacle->DestroyComponent();
		if (numTentacles > -1)
		{
			TentacleComponentArray[numTentacles]->DestroyComponent();
		}
		if (numTentacles > 0)
		{
			StartAttackTimer(3.0f);
		}
		else//no more tentacles
		{
			EndOfBoss();
		}

		if (tempHealth <= 0)
		{
			Health = 0.0f;//Defeated boss!
			EndOfBoss();
		}
		else
		{
			Health = tempHealth;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Boss health: %f Num of Tentacles: %d"), Health, numTentacles);
}

float AAIBoss_Doubt::GetHealth()
{
	return Health;
}

float AAIBoss_Doubt::GetMaxTentacleHealth()
{
	return tentacleHealth;
}

void AAIBoss_Doubt::Attack(float amount)
{

}

ACharacter* AAIBoss_Doubt::ClosestPlayer()//assuming player 1 and player 2
{
	float p1Dist = GetDistanceTo(player1);//AbsoluteVal((GetActorLocation() - player1->GetActorLocation()).Size());
	float p2Dist = GetDistanceTo(player2);//AbsoluteVal((GetActorLocation() - player2->GetActorLocation()).Size());
	if (p1Dist < p2Dist)
	{
		return player1;
	}
	else
	{
		return player2;
	}
}

float AAIBoss_Doubt::AbsoluteVal(float amount)//fmath::abs didnt work so...
{
	if (amount < 0)
	{
		return 0 - amount;
	}
	return amount;
}

void AAIBoss_Doubt::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			if (player1 == OtherActor)
			{
				p1InTrigger = true;
			}
			else
			{
				p2InTrigger = true;
			}
			UE_LOG(LogTemp, Warning, TEXT("****Player Entered BOSS Triggered Area"));
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			playerController = Cast<ADistancePlayerController>(player->GetController());
			StartAttackTimer(3.0f);
		}
	}
}

void AAIBoss_Doubt::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (CheckIfPlayer(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("****Player Exited BOSS Triggered Area"));
			if (player1 == OtherActor)
			{
				p1InTrigger = false;
			}
			else
			{
				p2InTrigger = false;
			}
		}
	}
}

void AAIBoss_Doubt::OnAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			if (!playerController->canMove && swallowedPlayer == NULL)//Can't move, therefore, i've dragged them in, and swallowed them
			{
				//set swallowed player
				printScreen(FColor::Red, TEXT("Beginning Draining Swallowed Player."));
				swallowedPlayer = player; // Cast<ADistanceCharacter>(OtherActor);
				StartSwallowedTimer(drainRate / 1.5f);//drainRate = 0.25f normally
			}
			else if (!playerController->canMove && swallowedPlayer != NULL)
			{
				printScreen(FColor::Red, TEXT("Beginning Draining second Swallowed Player."));
				StartDrainTimer(drainRate / 1.5f);//drainRate = 0.25f normally
				//TODO: stuff since both players are swallowed
				for (TActorIterator<AConvergenceCrystal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				{
					currentPlayer = Cast<ACharacter>(*ActorItr);
					player = NULL;
					playerController = NULL;//unless a crystal can be casted up the chain to distplayercontroller, then we set this to null
					StartAttackTimer(3.0f);
				}
			}
		}
	}
}

void AAIBoss_Doubt::OnExitAttackTrigger(class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
		}
	}
}

void AAIBoss_Doubt::OnTentacleOverlap_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("IM ANGRY FACE, HEAR ME ROAR, AND TENTACLES ENTER EVERYTHING!!!!!!"));
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("****THING Entered TENTACLE Triggered Area: %s"), *OtherActor->GetName());
		if (CheckIfPlayer(OtherActor))
		{
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			playerController = Cast<ADistancePlayerController>(player->GetController());

			PullPlayer(player);
			StartDrainTimer(drainRate);
			
			UE_LOG(LogTemp, Warning, TEXT("****Player Entered TENTACLE Triggered Area"));
		}
		else if (OtherActor->IsA(AConvergenceCrystal::StaticClass()))
		{
			if (swallowedPlayer && player)
			{
				targetActor = OtherActor;
				GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::ActorPullTimer, 0.1f, true);
				GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::ActorDrainTimer, 0.1f, true);
			}
		}
	}
}

bool AAIBoss_Doubt::CheckIfPlayer(class AActor* OtherActor)//TODO: not 100% positive im checking this correctly
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	//UE_LOG(LogTemp, Warning, TEXT("Player actually is: %s"), *player1->GetActorLabel());// these are messages for checking if we collide with the player or not
	//UE_LOG(LogTemp, Warning, TEXT("Player checked is: %s"), *OtherActor->GetActorLabel());
	//UE_LOG(LogTemp, Warning, TEXT("Are they the same?: %d"), ((player1 == OtherActor)));// ? "Yes" : "No"));
	if (player1 != NULL && player1 == OtherActor)//player1->GetActorLabel() == OtherActor->GetActorLabel())
	{
		//printScreen(FColor::Red, "Player 1 checked");
		//UE_LOG(LogTemp, Warning, TEXT("Player 1 checked"));
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)//player2->GetActorLabel() == OtherActor->GetActorLabel())
	{
		//printScreen(FColor::Red, "Player 2 checked");
		//UE_LOG(LogTemp, Warning, TEXT("Player 2 checked"));
		return true;
	}
	return false;
}

ATentacle* AAIBoss_Doubt::GetTentacle(int index)
{
	return (ATentacle *)TentacleComponentArray[index]->ChildActor;
}