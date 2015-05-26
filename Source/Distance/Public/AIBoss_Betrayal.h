// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIBoss.h"
#include "Classes/PaperSpriteComponent.h"
#include "AIBoss_Betrayal_Minion.h"
#include "AIBoss_Betrayal.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIBoss_Betrayal : public AAIBoss
{
	GENERATED_BODY()
	
public:

	AAIBoss_Betrayal(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	//virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerRange;//for approaching

	UPROPERTY(EditAnywhere)
	class ACharacter* player1;

	UPROPERTY(EditAnywhere)
	class ACharacter* player2;

	bool bossBattleBegun;

	bool p1InTrigger;
	bool p2InTrigger;

	float numOfMinions;
	float numOfCycles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	TArray<class UChildActorComponent*> TentacleComponentArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tentacle", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* TentacleComponent2;

	UChildActorComponent* CreateTentacleComponent(int i, const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category = Monster, EditAnywhere)
	TSubclassOf<class AAIBoss_Betrayal_Minion> MonsterClass;

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Cycle")
	void BeginCycle();

	UFUNCTION(BlueprintCallable, Category = "Cycle")
	void EndCycle();

	UFUNCTION(BlueprintCallable, Category = "Cycle")
	void TransformPlayers(bool toMonster);

	void SpawnMonster(FVector offset, MoveState movestate, class ACharacter* target);

	void DrainTimer();

	void BeginCycleTimer();

	virtual void EndOfBoss() override;

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	bool CheckIfPlayer(class AActor* OtherActor);

	void NotifyDeath();

	void ChangeHealth(float healthAmount);

private:

	float baseDamage;
	float drainRate;
	float timeBetweenCycles;
};
