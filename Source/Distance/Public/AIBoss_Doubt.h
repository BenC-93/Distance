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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerRange;//for approaching

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerAttack;

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
	class ADistanceCharacter* player;

	UPROPERTY(EditAnywhere)
	class ADistancePlayerController* playerController;

	void PullPlayer();
	void ReleasePlayer();
	void DrainPlayer();

	void AttackTimer();
	void StartAttackTimer(float rate);
	void DrainTimer();
	void StartDrainTimer(float rate);

	void ChangeHealth(float healthAmount);

	float GetHealth();
	float GetMaxTentacleHealth();
	
private:

	float Health;
	float MaxHealth;
	float tentacleHealth;
	int numTentacles;
	float baseDamage;

	bool p1InTrigger;
	bool p2InTrigger;

	void Attack(float amount);

	ACharacter* ClosestPlayer();

	float AbsoluteVal(float amount);
};
