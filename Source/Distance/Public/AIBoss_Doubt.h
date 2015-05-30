// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIBoss.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DistanceCharacter.h"
#include "DistancePlayerController.h"
#include "Classes/PaperSpriteComponent.h"
#include "AIBoss_Doubt.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIBoss_Doubt : public AAIBoss
{
	GENERATED_BODY()
	
public:
	AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	//virtual void PostLoad() override;
	//virtual void PostInitializeComponents() override;

	bool p1InTrigger;
	bool p2InTrigger;

	bool bossHasBegun;

	//UPROPERTY(Category = Items, EditAnywhere)
	//TSubclassOf<class ATentacle> TentacleClass;

	ATentacle* GetTentacle(int index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerRange;//for approaching

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerAttack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	TArray<class UChildActorComponent*> TentacleComponentArray;

	UChildActorComponent* CreateTentacleComponent(int i, const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnTentacleOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	bool CheckIfPlayer(class AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	void OnAttackTrigger(class AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	void OnExitAttackTrigger(class AActor* OtherActor);

	UPROPERTY(EditAnywhere)
	class ACharacter* player1;

	UPROPERTY(EditAnywhere)
	class ACharacter* player2;

	UPROPERTY(EditAnywhere)
	class ACharacter* currentPlayer;

	UPROPERTY(EditAnywhere)
	class ADistanceCharacter* swallowedPlayer;

	UPROPERTY(EditAnywhere)
	class ADistanceCharacter* secondSwallowedPlayer;

	UPROPERTY(EditAnywhere)
	class ADistanceCharacter* player;

	UPROPERTY(EditAnywhere)
	class ADistancePlayerController* playerController;

	class ACharacter* FindTarget();

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Tentacle")
	//TArray<class ATentacle*> tentacleArray;

	UFUNCTION(BlueprintCallable, Category = "Rotations")
	FRotator FaceActorRotation(FVector myActorLoc, class AActor* OtherActor);

	float tentacleYaw;
	float distToPlayer;
	float chosenTentacleIndex;
	float tentacleCounter;
	float tentacleSpriteLen;

	bool isPullingObject;//whether or not an object is being pulled
	class AActor* pullingObject;//object being pulled

	class AActor* targetActor;
	void PullActor(class AActor* tempActor);

	void PullPlayer(class ACharacter* tempChar);
	void ReleasePlayer(class ACharacter* tempChar);
	void DrainPlayer(class ADistanceCharacter* tempPlayer);

	class ADistanceCharacter* drainTarget1;
	class ADistanceCharacter* drainTarget2;

	void ActorPullTimer();
	void DrainActor(class AActor* tempActor);
	void ActorDrainTimer();

	void CrystalTentacleTimer();
	void StartCrystalTentacleTimer(float rate);
	void TentacleTimer();
	void StartTentacleTimer(float rate);
	void AttackTimer();
	void StartAttackTimer(float rate);
	void CrystalAttackTimer();
	void StartCrystalAttackTimer(float rate);
	void DrainTimer();
	void StartDrainTimer(float rate);
	void AnotherDrainTimer();
	void StartAnotherDrainTimer(float rate);
	void SwallowedTimer();
	void StartSwallowedTimer(float rate);
	void SecondSwallowedTimer();
	void StartSecondSwallowedTimer(float rate);

	virtual void EndOfBoss(bool KilledBoss = false) override;

	bool CheckIfBothPlayersSwallowed();

	void ChangeHealth(float healthAmount);

	float GetHealth();
	float GetMaxTentacleHealth();
	
private:

	float tentacleHealth;
	int numTentacles;
	float baseDamage;
	float drainRate;

	void Attack(float amount);

	ACharacter* ClosestPlayer();

	float AbsoluteVal(float amount);
};
