// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DistanceCharacter.h"
#include "DistancePlayerController.h"
#include "Classes/PaperSpriteComponent.h"
#include "AIBoss_Doubt.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIBoss_Doubt : public ACharacter
{
	GENERATED_BODY()
	
public:
	AAIBoss_Doubt(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(Category = Items, EditAnywhere)
	TSubclassOf<class ATentacle> TentacleClass;

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
	class ADistanceCharacter* player;

	UPROPERTY(EditAnywhere)
	class ADistancePlayerController* playerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Tentacle")
	TArray<class ATentacle*> tentacleArray;

	void PullPlayer(class ACharacter* tempChar);
	void ReleasePlayer(class ACharacter* tempChar);
	void DrainPlayer(class ADistanceCharacter* tempPlayer);

	void AttackTimer();
	void StartAttackTimer(float rate);
	void DrainTimer();
	void StartDrainTimer(float rate);
	void SwallowedTimer();
	void StartSwallowedTimer(float rate);

	void EndOfBoss();

	void ChangeHealth(float healthAmount);

	float GetHealth();
	float GetMaxTentacleHealth();
	
private:

	float Health;
	float MaxHealth;
	float tentacleHealth;
	int numTentacles;
	float baseDamage;
	float drainRate;

	bool p1InTrigger;
	bool p2InTrigger;

	void Attack(float amount);

	ACharacter* ClosestPlayer();

	float AbsoluteVal(float amount);
};
