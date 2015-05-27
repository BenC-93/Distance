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
	tentacleSpriteLen = 290.0f;//actual sprite len is 616, cut in half is 308 because the bp is scaled to .5, i go less so that it overreaches by a little

	swallowedPlayer = NULL;
	secondSwallowedPlayer = NULL;

	drainTarget1 = NULL;
	drainTarget2 = NULL;

	isPullingObject = false;

	p1InTrigger = false;
	p2InTrigger = false;

	bossHasBegun = false;

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
	TentacleComponent->OnComponentCreated();
	TentacleComponent->RelativeLocation = FVector(-465.0f, -370.0f + (i * 170.0f), -280.0f + (i * 3.0f));//add y by 170, add z by 3 each tentacle
	TentacleComponent->AttachTo(RootComponent);
	TentacleComponent->SetIsReplicated(true);
	
	// Add TC to Array of components
	TentacleComponentArray.Add(TentacleComponent);
	return TentacleComponent;
}

void AAIBoss_Doubt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (player != NULL)
	{
		if (!isPullingObject && swallowedPlayer == NULL)
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
					}
					else
					{
						currentPlayer = player2;
						player = Cast<ADistanceCharacter>(currentPlayer);
						playerController = Cast<ADistancePlayerController>(player->GetController());
					}
				}
			}
		}
	}
	if (swallowedPlayer)//player switching items to release other
	{
		if (playerController)
		{
			if (playerController->switchedItem)//other player, not the swallowed one
			{
				//printScreen(FColor::Red, TEXT("Swallowed Player was Released by other player switching Items"));
				UE_LOG(LogTemp, Error, TEXT("Swallowed Player was Released by other player switching Items"));
				ReleasePlayer(swallowedPlayer);
				StartAttackTimer(3.0f);
				playerController->switchedItem = false;
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Tick: checking if player switched items: current playercontroller target is null, might have both players swallowed"));
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
	if (isPullingObject && pullingObject && !GetWorldTimerManager().IsTimerActive(this, &AAIBoss_Doubt::TentacleTimer) && TentacleComponentArray.IsValidIndex(chosenTentacleIndex))//shrink and rotate tentacle that is pulling the player to match the player getting pulled in
	{
		//UE_LOG(LogTemp, Error, TEXT("shrinking tentalces"));
		AActor* tentacleActor = TentacleComponentArray[chosenTentacleIndex]->ChildActor;
		float distToObject = GetDistanceTo(pullingObject);
		if (tentacleActor)
		{
			distToObject = tentacleActor->GetDistanceTo(pullingObject);
		}

		float myYaw = FaceActorRotation(TentacleComponentArray[chosenTentacleIndex]->GetComponentLocation(), pullingObject).Yaw;
		if (pullingObject->GetActorLocation().Y > GetActorLocation().Y)
		{
			//rotate //y,z,x
			TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, (myYaw - 180), 0));//tentacleYaw + 180 works but wrong rotation wise, tentacleYaw - 217 works for rotation wise and for boss being base, 195 seems to work from tentacle itself
		}
		else
		{
			TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, (myYaw + 185), 0));
		}
		float maxScale = distToObject / tentacleSpriteLen;
		//UE_LOG(LogTemp, Warning, TEXT("scale: %f dist: %f spritelen: %f"), maxScale, distToObject, tentacleSpriteLen);
		TentacleComponentArray[chosenTentacleIndex]->SetRelativeScale3D(FVector(maxScale, 1, 1));
		FVector currentScale = TentacleComponentArray[chosenTentacleIndex]->GetComponentScale();
		//UE_LOG(LogTemp, Warning, TEXT("actual scale: %f, %f, %f"), currentScale.X, currentScale.Y, currentScale.Z);
	}
	else if (!isPullingObject && !GetWorldTimerManager().IsTimerActive(this, &AAIBoss_Doubt::TentacleTimer) && TentacleComponentArray.IsValidIndex(chosenTentacleIndex))
	{
		//TentacleComponentArray[chosenTentacleIndex]->SetRelativeScale3D(FVector(1, 1, 1));
	}
	if (isPullingObject && pullingObject && !CheckIfBothPlayersSwallowed())//for constant checking while pulling a player to set their speed accordingly
	{
		class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(pullingObject);
		
		if (tempPlayer->GetItem() == NULL)
		{
			//printScreen(FColor::Red, TEXT("Player's item is NULL! LOOK, THIS IS BAD!"));
			UE_LOG(LogTemp, Error, TEXT("Error: PullPlayer: player's item is NULL!"));
			tempPlayer->ChangeSpeed(100);
		}
		else if (tempPlayer->GetItem()->IsA(AItemShield::StaticClass()) && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)
		{
			tempPlayer->ChangeSpeed(10);//shield is enabled
		}
		else
		{
			tempPlayer->ChangeSpeed(100);
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
	isPullingObject = true;
	pullingObject = Cast<AActor>(tempChar);
	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (tempPlayer->GetItem() == NULL)
	{
		//printScreen(FColor::Red, TEXT("Player's item is NULL! LOOK, THIS IS BAD!"));
		UE_LOG(LogTemp, Error, TEXT("Error: PullPlayer: player's item is NULL!"));
		tempPlayer->ChangeSpeed(100);
	}
	else if (tempPlayer->GetItem()->IsA(AItemShield::StaticClass()) && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)
	{
		tempPlayer->ChangeSpeed(10);//shield is enabled
	}
	else
	{
		tempPlayer->ChangeSpeed(100);
	}

	/*player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	if (tempChar == player1)
	{
		Cast<ADistancePlayerController>(player1->GetController())->notTrappedByEnemy = false;
		//UE_LOG(LogTemp, Error, TEXT("pulling player1's notTrappedByEnemy: %d"), Cast<ADistancePlayerController>(player1->GetController())->notTrappedByEnemy);
	}
	else if (tempChar == player2)
	{
		Cast<ADistancePlayerController>(player2->GetController())->notTrappedByEnemy = false;
		//UE_LOG(LogTemp, Error, TEXT("pulling player2's notTrappedByEnemy: %d"), Cast<ADistancePlayerController>(player2->GetController())->notTrappedByEnemy);
	}*/
	tempPlayerController->notTrappedByEnemy = false;
	UE_LOG(LogTemp, Error, TEXT("pulling tmepplayer's notTrappedByEnemy: %d"), tempPlayerController->notTrappedByEnemy);


	tempPlayerController->SetNewMoveDestination(GetActorLocation());
}

void AAIBoss_Doubt::PullActor(class AActor* tempActor)
{
	if (tempActor == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: PullActor, tempActor is Null."));
		return;
	}
	isPullingObject = true;
	pullingObject = tempActor;
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
	isPullingObject = false;
	pullingObject = NULL;

	class ADistanceCharacter* tempPlayer = Cast<ADistanceCharacter>(tempChar);
	class ADistancePlayerController* tempPlayerController = Cast<ADistancePlayerController>(tempPlayer->GetController());

	if (!tempPlayerController->notTrappedByEnemy)//if they were under ai control, stop moving them
	{
		tempPlayerController->SetNewMoveDestination(tempPlayer->GetActorLocation() - FVector(121.0f, 0.0f, 0.0f));//for stopping the last forced move
	}
	tempPlayerController->notTrappedByEnemy = true;
	tempPlayerController->canUseItem = true;
	tempPlayer->ChangeSpeed(600);
	if (tempPlayer == swallowedPlayer)//if releasing swallowed player -> stop its timer, and reset swallowed player to null so we can get a current closest player again (might happen instantly, idk)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
		swallowedPlayer = NULL;
	}
	else if (tempPlayer == secondSwallowedPlayer)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SecondSwallowedTimer);
		secondSwallowedPlayer = NULL;
	}
	else
	{
		if (drainTarget1 && tempPlayer == drainTarget1)
		{
			GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
			drainTarget1 = NULL;
		}
		else if(drainTarget2 && tempPlayer == drainTarget2)
		{
			GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AnotherDrainTimer);
			drainTarget2 = NULL;
		}
		
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AttackTimer);//TODO: do we really want this, its been there forever, but now i dont know
	//printScreen(FColor::Red, "Player was Released Normally");
	UE_LOG(LogTemp, Error, TEXT("Player was Released Normally."));
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
		//printScreen(FColor::Red, TEXT("Player's item is NULL! LOOK, THIS IS BAD!"));
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer: player's item is NULL!"));
		tempPlayer->ChangeHealth(baseDamage);
	}
	else if (tempPlayer->GetItem()->IsA(AItemShield::StaticClass()) && tempPlayer->GetItemEnabled() && tempPlayer->GetItemAmount() > 0)
	{
		tempPlayer->ChangeItemAmount(baseDamage);//shield is enabled TODO: do we want to drain shield, we used to want to, do we still?
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
	if (tentacleCounter >= 1.0f)//End of the tentacle scaling
	{
		//printScreen(FColor::Red, TEXT("Stopping tentacle timer"));
		//UE_LOG(LogTemp, Warning, TEXT("Stopping tentacle timer"));
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::TentacleTimer);

		//check if we didnt hit a player, try again with a new target, or same, depending
		if (!isPullingObject && !CheckIfBothPlayersSwallowed())
		{
			currentPlayer = Cast<ADistanceCharacter>(FindTarget());
			player = Cast<ADistanceCharacter>(currentPlayer);
			playerController = Cast<ADistancePlayerController>(player->GetController());

			StartAttackTimer(3.0f);
		}

		return;
	}
	if (currentPlayer->GetActorLocation().Y > GetActorLocation().Y)
	{
		//rotate //y,z,x
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw - 180), tentacleCounter), 0));//tentacleYaw + 180 works but wrong rotation wise, tentacleYaw - 217 works for rotation wise and for boss being base, 195 seems to work from tentacle itself
	}
	else
	{
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw + 185), tentacleCounter), 0));
	}
	//scale
	float maxScale = distToPlayer / tentacleSpriteLen;
	//UE_LOG(LogTemp, Error, TEXT("Max Scale: %f"), maxScale);
	TentacleComponentArray[chosenTentacleIndex]->SetRelativeScale3D(FVector(FMath::Lerp(0.5f, maxScale, tentacleCounter), 1, 1));
	
	tentacleCounter += 0.1f;
	//UE_LOG(LogTemp, Error, TEXT("Tentacle Counter: %f"), tentacleCounter);
}

void AAIBoss_Doubt::CrystalTentacleTimer()
{
	if (tentacleCounter >= 1.0f)//End of the tentacle scaling
	{
		//printScreen(FColor::Red, TEXT("Stopping tentacle timer"));
		UE_LOG(LogTemp, Warning, TEXT("Stopping tentacle timer"));
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::TentacleTimer);

		//check if we didnt hit a player, try again with a new target, or same, depending
		if (!isPullingObject)
		{
			StartCrystalAttackTimer(3.0f);
		}

		return;
	}
	if (targetActor&& targetActor->GetActorLocation().Y > GetActorLocation().Y)
	{
		//rotate //y,z,x
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw - 180), tentacleCounter), 0));//tentacleYaw + 180 works but wrong rotation wise, tentacleYaw - 217 works for rotation wise and for boss being base, 195 seems to work from tentacle itself
	}
	else
	{
		TentacleComponentArray[chosenTentacleIndex]->SetWorldRotation(FRotator(0, FMath::Lerp(0.0f, (tentacleYaw + 185), tentacleCounter), 0));
	}
	//scale
	float maxScale = distToPlayer / tentacleSpriteLen;
	//UE_LOG(LogTemp, Error, TEXT("Max Scale: %f"), maxScale);
	TentacleComponentArray[chosenTentacleIndex]->SetRelativeScale3D(FVector(FMath::Lerp(0.5f, maxScale, tentacleCounter), 1, 1));

	tentacleCounter += 0.1f;
	//UE_LOG(LogTemp, Error, TEXT("Tentacle Counter: %f"), tentacleCounter);
}

void AAIBoss_Doubt::StartCrystalTentacleTimer(float rate)
{
	tentacleCounter = 0.0f;
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::CrystalTentacleTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::CrystalTentacleTimer, rate, true);
}

class ACharacter* AAIBoss_Doubt::FindTarget()
{
	if (swallowedPlayer)
	{
		if (swallowedPlayer == player1)
		{
			UE_LOG(LogTemp, Error, TEXT("Target found: player2"));
			return player2;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Target found: player1"));
			return player1;
		}
	}
	else if (!swallowedPlayer && !CheckIfBothPlayersSwallowed())//no swallowed players
	{
		UE_LOG(LogTemp, Error, TEXT("Target found: closest player"));
		return ClosestPlayer();
	}
	UE_LOG(LogTemp, Error, TEXT("No Target found: using player1 as default"));
	return player1;//default return if something goes wrong
}

void AAIBoss_Doubt::StartTentacleTimer(float rate)
{
	tentacleCounter = 0.0f;
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::TentacleTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::TentacleTimer, rate, true);
}

void AAIBoss_Doubt::AttackTimer()
{
	//printScreen(FColor::Red, TEXT("Boss making an attack"));
	UE_LOG(LogTemp, Warning, TEXT("Boss making an attack"));
	chosenTentacleIndex = numTentacles - 1; //FMath::RandRange(0, numTentacles - 1);
	AActor* tentacleActor = TentacleComponentArray[chosenTentacleIndex]->ChildActor;
	
	if (!currentPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: attack timer, something is terrible wrong, we dont know what to attack"));
		return;
	}
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

void AAIBoss_Doubt::CrystalAttackTimer()
{
	//printScreen(FColor::Red, TEXT("Boss making an attack"));
	UE_LOG(LogTemp, Warning, TEXT("Boss making an attack on crystal"));
	chosenTentacleIndex = numTentacles - 1; //FMath::RandRange(0, numTentacles - 1);
	AActor* tentacleActor = TentacleComponentArray[chosenTentacleIndex]->ChildActor;
	
	if (targetActor)
	{
		if (tentacleActor)
		{
			distToPlayer = tentacleActor->GetDistanceTo(targetActor);
			tentacleYaw = FaceActorRotation(TentacleComponentArray[chosenTentacleIndex]->GetComponentLocation(), targetActor).Yaw;
		}
		else
		{
			distToPlayer = GetDistanceTo(targetActor);
			tentacleYaw = FaceActorRotation(TentacleComponentArray[chosenTentacleIndex]->GetComponentLocation(), targetActor).Yaw;
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::CrystalAttackTimer);
		UE_LOG(LogTemp, Error, TEXT("Boss has no target to attack the crystal"));
		return;
	}

	StartCrystalTentacleTimer(0.1f);

	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::CrystalAttackTimer);
}

void AAIBoss_Doubt::StartCrystalAttackTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::CrystalAttackTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::CrystalAttackTimer, rate, true);
}

void AAIBoss_Doubt::DrainTimer()
{
	//printScreen(FColor::Red, "Draining");
	if (!drainTarget1)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: DrainTimer, draintarget1 is null."));
		return;
	}
	DrainPlayer(drainTarget1);
	if (drainTarget1->Health == 0)//we "killed" the player, oops lol
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

void AAIBoss_Doubt::AnotherDrainTimer()
{
	//printScreen(FColor::Red, "Draining");
	if (!drainTarget2)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: DrainTimer, draintarget2 is null."));
		return;
	}
	DrainPlayer(drainTarget2);
	if (drainTarget2->Health == 0)//we "killed" the player, oops lol
	{
		//ReleasePlayer(player);
		//EndOfBoss();
	}
}

void AAIBoss_Doubt::StartAnotherDrainTimer(float rate)
{
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AnotherDrainTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::AnotherDrainTimer, rate, true);
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
	//check if swallowedplayer is draintarget 1 or 2 and clear that timer and set the target to null
	if (drainTarget1 && secondSwallowedPlayer == drainTarget1)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
		drainTarget1 = NULL;
	}
	if (drainTarget2 && secondSwallowedPlayer == drainTarget2)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AnotherDrainTimer);
		drainTarget2 = NULL;
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SwallowedTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::SwallowedTimer, rate, true);
}

void AAIBoss_Doubt::SecondSwallowedTimer()
{
	if (secondSwallowedPlayer == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Error: SecondSwallowedTimer, swallowedPlayer is Null."));
		return;
	}
	//printScreen(FColor::Red, "Draining Swallowed Player");
	DrainPlayer(secondSwallowedPlayer);
	if (secondSwallowedPlayer->Health == 0)//we "killed" the player, oops lol
	{
		//ReleasePlayer(swallowedPlayer);
	}
}

void AAIBoss_Doubt::StartSecondSwallowedTimer(float rate)
{
	//check if swallowedplayer is draintarget 1 or 2 and clear that timer and set the target to null
	if (drainTarget1 && secondSwallowedPlayer == drainTarget1)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::DrainTimer);
		drainTarget1 = NULL;
	}
	if (drainTarget2 && secondSwallowedPlayer == drainTarget2)
	{
		GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::AnotherDrainTimer);
		drainTarget2 = NULL;
	}
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Doubt::SecondSwallowedTimer);
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::SecondSwallowedTimer, rate, true);
}

void AAIBoss_Doubt::EndOfBoss()
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);

	ReleasePlayer(player1);
	ReleasePlayer(player2);

	GetWorldTimerManager().ClearAllTimersForObject(this);

	for (TActorIterator<AConvergenceCrystal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->Destroy();
	}

	//Set everything to null here if we decide not to destroy boss at end

	playerController = Cast<ADistancePlayerController>(player1->GetController());
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
		if (isPullingObject && pullingObject)
		{
			ReleasePlayer(Cast<ACharacter>(pullingObject));//might give error if crystal can ever be the pullingObject while a player is killing a tentacle, so it should give problems
		}
		numTentacles--;
		//UChildActorComponent* tempTentacle = TentacleComponentArray[numTentacles];
		//TentacleComponentArray.RemoveAt(numTentacles);
		//tempTentacle->DestroyComponent();
		if (numTentacles > -1)
		{
			TentacleComponentArray[numTentacles]->DestroyComponent();
			UE_LOG(LogTemp, Error, TEXT("Boss kknows that it destroied its tentacle****************"));
		}
		if (numTentacles > 0 && !CheckIfBothPlayersSwallowed())
		{
			StartAttackTimer(3.0f);
		}
		else if (numTentacles > 0 && CheckIfBothPlayersSwallowed())
		{
			StartCrystalAttackTimer(3.0f);
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
			if (!bossHasBegun)
			{
				UE_LOG(LogTemp, Warning, TEXT("****Player Entered BOSS Triggered Area"));
				currentPlayer = Cast<ADistanceCharacter>(OtherActor);
				player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
				playerController = Cast<ADistancePlayerController>(player->GetController());
				StartAttackTimer(3.0f);//created the case where the player dodges the tentacle and we should start attack again, so we only have to do this once
				bossHasBegun = true;
			}
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
	UE_LOG(LogTemp, Warning, TEXT("Something entered swallowed trigger."));
	if (OtherActor && (OtherActor != this))
	{
		if (CheckIfPlayer(OtherActor))
		{
			if (OtherActor == pullingObject && !Cast<ADistancePlayerController>(Cast<ACharacter>(OtherActor)->GetController())->notTrappedByEnemy && swallowedPlayer == NULL)//Can't move, therefore, i've dragged them in, and swallowed them
			{
				//set swallowed player
				//printScreen(FColor::Red, TEXT("Beginning Draining Swallowed Player."));
				UE_LOG(LogTemp, Warning, TEXT("Beginning Draining Swallowed Player."));
				swallowedPlayer = player; // Cast<ADistanceCharacter>(OtherActor);
				Cast<ADistancePlayerController>(swallowedPlayer->GetController())->canUseItem = false;
				StartSwallowedTimer(drainRate / 1.5f);//drainRate = 0.25f normally
				isPullingObject = false;
				pullingObject = NULL;

				//if swallowedPlayer == player -> then currentPlayer needs to be other player
				if (swallowedPlayer == player1)
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
				UE_LOG(LogTemp, Warning, TEXT("Resetting Target, one player has been swallowed, beginning new attack"));
				StartAttackTimer(3.0f);//attack the new target
				
			}
			else if (!playerController->notTrappedByEnemy && swallowedPlayer != NULL)//previously was !playerController->notTrappedByEnemy && swallowedPlayer != NULL
			{
				secondSwallowedPlayer = player;
				Cast<ADistancePlayerController>(secondSwallowedPlayer->GetController())->canUseItem = false;
				//printScreen(FColor::Red, TEXT("Beginning Draining second Swallowed Player."));
				UE_LOG(LogTemp, Warning, TEXT("Beginning Draining second Swallowed Player."));
				StartSecondSwallowedTimer(drainRate / 1.5f);
				isPullingObject = false;
				pullingObject = NULL;
				for (TActorIterator<AConvergenceCrystal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				{
					targetActor = Cast<AActor>(*ActorItr);
					currentPlayer = NULL;
					player = NULL;
					playerController = NULL;
					StartCrystalAttackTimer(3.0f);//begin attack on crystal
					UE_LOG(LogTemp, Warning, TEXT("Starting attack on crystal."));
				}
			}
		}
		else if (CheckIfBothPlayersSwallowed() && targetActor && OtherActor->IsA(AConvergenceCrystal::StaticClass()))//we swallowed the crystal
		{
			isPullingObject = false;
			EndOfBoss();
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
	if (OtherActor && (OtherActor != this) && OtherComp && !isPullingObject)
	{
		//UE_LOG(LogTemp, Warning, TEXT("****THING Entered TENTACLE Triggered Area: %s"), *OtherActor->GetName());
		if (CheckIfPlayer(OtherActor))
		{
			currentPlayer = Cast<ADistanceCharacter>(OtherActor);
			player = Cast<ADistanceCharacter>(currentPlayer);//added for use of player methods
			playerController = Cast<ADistancePlayerController>(player->GetController());

			PullPlayer(player);
			if (drainTarget2 == NULL)
			{
				drainTarget2 = player;
				StartAnotherDrainTimer(drainRate);
				UE_LOG(LogTemp, Warning, TEXT("starting drain using another timer"));
			}
			else if (drainTarget1 == NULL)
			{
				drainTarget1 = player;
				StartDrainTimer(drainRate);
				UE_LOG(LogTemp, Warning, TEXT("starting drain using timer"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("WTF, cant decide on a drain target so no draining happens"));
			}
			
			UE_LOG(LogTemp, Warning, TEXT("****Player Entered TENTACLE Triggered Area"));
		}
		else if (OtherActor->IsA(AConvergenceCrystal::StaticClass()))
		{
			if (CheckIfBothPlayersSwallowed())
			{
				UE_LOG(LogTemp, Warning, TEXT("TargetActor is set"));
				targetActor = OtherActor;
				GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::ActorPullTimer, 0.1f, true);//pull the crystal
				GetWorldTimerManager().SetTimer(this, &AAIBoss_Doubt::ActorDrainTimer, 0.1f, true);//drain the crystal faster
			}
		}
	}
}

bool AAIBoss_Doubt::CheckIfPlayer(class AActor* OtherActor)
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

bool AAIBoss_Doubt::CheckIfBothPlayersSwallowed()
{
	if (player1 && player2)//needed becuase adjusting the blueprints in editor calls code and will crash if I dont check everything.
	{
		class ADistancePlayerController* tempController1 = Cast<ADistancePlayerController>(player1->GetController());
		class ADistancePlayerController* tempController2 = Cast<ADistancePlayerController>(player2->GetController());
		if (!tempController1->notTrappedByEnemy && !tempController2->notTrappedByEnemy && swallowedPlayer && secondSwallowedPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Both players are swallowed."));
			return true;
		}
	}
	return false;
}

ATentacle* AAIBoss_Doubt::GetTentacle(int index)
{
	return (ATentacle *)TentacleComponentArray[index]->ChildActor;
}