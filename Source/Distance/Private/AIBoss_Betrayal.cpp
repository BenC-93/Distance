// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistancePlayerController.h"
#include "ConvergenceCrystal.h"
#include "AIBoss_Betrayal.h"

AAIBoss_Betrayal::AAIBoss_Betrayal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;

	baseDamage = -1.0f;
	drainRate = 0.5f;

	timeBetweenCycles = 15.0f;//in seconds

	bossBattleBegun = false;

	p1InTrigger = false;
	p2InTrigger = false;

	numOfMinions = 0.0f;
	numOfCycles = 3.0f;

	SpriteComponent = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("SpriteComponent"));
	SpriteComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	SpriteComponent->AttachTo(RootComponent);

	AITriggerRange = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("AITriggerRange"));
	AITriggerRange->Mobility = EComponentMobility::Movable;
	AITriggerRange->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f), true);
	AITriggerRange->AttachTo(RootComponent);

	AITriggerRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBoss_Betrayal::OnOverlapBegin);
	AITriggerRange->OnComponentEndOverlap.AddDynamic(this, &AAIBoss_Betrayal::OnOverlapEnd);

	TentacleComponent0 = CreateTentacleComponent(0, ObjectInitializer);
	TentacleComponent1 = CreateTentacleComponent(1, ObjectInitializer);
	TentacleComponent2 = CreateTentacleComponent(2, ObjectInitializer);
	TentacleComponent3 = CreateTentacleComponent(3, ObjectInitializer);
}

UChildActorComponent* AAIBoss_Betrayal::CreateTentacleComponent(int i, const FObjectInitializer& ObjectInitializer)
{
	FName componentName = FName(*FString::Printf(TEXT("TentacleComponent %d"), i));
	class UChildActorComponent* TentacleComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, componentName);
	TentacleComponent->OnComponentCreated();
	//TentacleComponent->RelativeRotation = FRotator(DEFAULT_SPRITE_PITCH, DEFAULT_SPRITE_YAW, DEFAULT_SPRITE_ROLL);//y,z,x
	TentacleComponent->RelativeLocation = FVector(0.0f, 0.0f + (i * 170.0f), -1.0f);//add y by 170
	TentacleComponent->AttachTo(RootComponent);
	//TentacleComponent->SetIsReplicated(true);

	// Add TC to Array of components
	TentacleComponentArray.Add(TentacleComponent);
	return TentacleComponent;
}

void AAIBoss_Betrayal::PostInitializeComponents()
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	Super::PostInitializeComponents();
}

/*void AAIBoss_Betrayal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

void AAIBoss_Betrayal::BeginCycle()
{
	numOfMinions = 6.0f;
	//summon monsters
	float widthOfMonsters = 150.0f;
	for (int i = 0; i < numOfMinions; i++)
	{
		//y: half of the #ofminions so that I can align them spawning with the 3rd monster in the middle of the boss
		//x: 150.0f below the boss
		FVector offset = FVector(150.0f, ((numOfMinions / 2) * widthOfMonsters * -1) + (widthOfMonsters * i), 0.0f);
		switch (i)
		{//usual order is copy, follow, random
			case 0:
				SpawnMonster(offset, MoveState::COPY, player1);
				break;
			case 1:
				SpawnMonster(offset, MoveState::FOLLOW, player1);
				break;
			case 2:
				SpawnMonster(offset, MoveState::RANDOM, player1);
				break;
			case 3:
				SpawnMonster(offset, MoveState::COPY, player2);
				break;
			case 4:
				SpawnMonster(offset, MoveState::FOLLOW, player2);
				break;
			case 5:
				SpawnMonster(offset, MoveState::RANDOM, player2);
				break;
			default:
				SpawnMonster(offset, MoveState::STATIC, NULL);
				break;
		}
	}
	//constantly make players lose health
	GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal::DrainTimer, drainRate, true);
}

void AAIBoss_Betrayal::EndCycle()
{
	//reduce num of cycles
	numOfCycles--;
	//destroy a tentacle thing to show loss of health representation
	if (numOfCycles > -1)
	{
		TentacleComponentArray[numOfCycles]->DestroyComponent();
	}
	//make boss lose health
	ChangeHealth(-25.0f);
	//stop constantly makingn players lose health
	GetWorldTimerManager().ClearTimer(this, &AAIBoss_Betrayal::DrainTimer);
	UE_LOG(LogTemp, Error, TEXT("End of Cycle: %f cycles left"), numOfCycles);
}

void AAIBoss_Betrayal::SpawnMonster(FVector offset, MoveState movestate, class ACharacter* target)
{
	AAIBoss_Betrayal_Minion* monster = GetWorld()->SpawnActor<AAIBoss_Betrayal_Minion>(MonsterClass, FVector(this->GetActorLocation()) - offset, FRotator(this->GetActorRotation()));
	monster->SetOwner(this);
	monster->SetMoveState(movestate, target);
}

void AAIBoss_Betrayal::DrainTimer()
{
	class ADistanceCharacter* tempPlayer1 = NULL;
	class ADistanceCharacter* tempPlayer2 = NULL;
	if (player1)
	{
		tempPlayer1 = Cast<ADistanceCharacter>(player1);
	}
	else
	{
		//error
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer, player1 is Null."));
		return;
	}
	if (player2)
	{
		tempPlayer2 = Cast<ADistanceCharacter>(player2);
	}
	else
	{
		//error
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer, player2 is Null."));
		return;
	}
	if (!tempPlayer1 || !tempPlayer2)
	{
		//cast error
		UE_LOG(LogTemp, Error, TEXT("Error: DrainPlayer, Cast to distancecharacter failed."));
		return;
	}

	tempPlayer1->ChangeHealth(baseDamage);
	tempPlayer2->ChangeHealth(baseDamage);
	if (tempPlayer1->Health == 0 || tempPlayer2->Health == 0)//we "killed" a player, oops lol
	{
		EndOfBoss();
	}
}

void AAIBoss_Betrayal::BeginCycleTimer()
{
	BeginCycle();
}

void AAIBoss_Betrayal::NotifyDeath()//notifies the boss when one of its minion dies
{
	if (numOfMinions > 0)//minion died, reduce number of minions
	{
		numOfMinions--;
		UE_LOG(LogTemp, Warning, TEXT("A minion died, %f minions left."), numOfMinions);
	}
	if (numOfMinions <= 0)//no more minions
	{
		if (numOfCycles > 0)//cycle has ended, begin next cycle
		{
			//end cycle cleanup
			EndCycle();
			//start wait timer to start next cycle
			GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal::BeginCycleTimer, timeBetweenCycles, false);
		}
		else//no more cycles, end of boss
		{
			EndOfBoss();
		}
	}
}

void AAIBoss_Betrayal::ChangeHealth(float healthAmount)
{
	float tempHealth = Health + healthAmount;
	if (tempHealth <= MaxHealth)
	{
		if (tempHealth <= 0)
		{
			Health = 0.0f;//Defeated boss!
			EndOfBoss(true);
		}
		else
		{
			Health = tempHealth;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Boss health: %f"), Health);
}

void AAIBoss_Betrayal::EndOfBoss(bool KilledBoss)
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	GetWorldTimerManager().ClearAllTimersForObject(this);

	printScreen(FColor::Red, TEXT("End of Boss"));

	//destroy all minions
	for (TActorIterator<AAIBoss_Betrayal_Minion> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->Destroy();
	}
	
	Super::EndOfBoss(KilledBoss);
	Destroy();
}

void AAIBoss_Betrayal::OnOverlapBegin_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
			if (!bossBattleBegun)
			{
				bossBattleBegun = true;
				//starttimer that begins cycle
				GetWorldTimerManager().SetTimer(this, &AAIBoss_Betrayal::BeginCycle, 3.0f, false);//give initial spawn wait time
			}
		}
	}
}

void AAIBoss_Betrayal::OnOverlapEnd_Implementation(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

bool AAIBoss_Betrayal::CheckIfPlayer(class AActor* OtherActor)
{
	player1 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	player2 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	if (player1 != NULL && player1 == OtherActor)
	{
		return true;
	}
	else if (player2 != NULL && player2 == OtherActor)
	{
		return true;
	}
	return false;
}