// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AIBoss_Betrayal_Minion.generated.h"

enum MoveState { STATIC, COPY, FOLLOW, RANDOM };

enum MinionOrientation : uint8 { LEFT, RIGHT };

UCLASS()
class DISTANCE_API AAIBoss_Betrayal_Minion : public ACharacter
{
	GENERATED_BODY()
	
public:
	AAIBoss_Betrayal_Minion(const FObjectInitializer& ObjectInitializer);

	void EquipCorrectClassItem(ADItem *blah);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	class ADItem* CurrentItem;
	class ADItem* PlayerItem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> LanternClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> StaffClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> ShieldClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> CrystalClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADItem> SwordClass;
	
	FName ItemSocket;

	virtual void Tick(float DeltaTime) override;

	virtual FVector GetVelocity() const override;

	//~~~ Health ~~~//
	float Health;
	float MaxHealth;

	void ChangeHealth(float h);
	float GetHealth();
	float GetMaxHealth();
	
	//~~~ Targeting  ~~~//
	AActor* TargetActor;
	FVector TargetLocation;
	float TargetSpeed;

	FVector CopyLength;

	void SetTargetActor(AActor* t);
	AActor* GetTargetActor();

	//~~~ Move states(States) and functions ~~~//
	MoveState ActiveMoveState;

	void SetMoveState(MoveState m, ACharacter* c);
	MoveState GetMoveState();

	void MoveStatic();
	void MoveCopy();
	void MoveFollow();
	void MoveRandom();

	void StartMoveRandomTimer();

	//~~~ Owning Monster ~~~//
	class AAIBoss_Betrayal* OwningPawn;

	float currentSpeed;

	void SetOwner(class AAIBoss_Betrayal* b);

	bool canMove;

	void AnimationTimer();

	void AttackTimer();

	UPROPERTY(Category = Animation, EditAnywhere)
	UAnimationAsset* AttackAnimation;

	void StartAttackRandomTimer();
	FName GetItemAttachPoint();

	MinionOrientation CurrDirection;

	void FlipForDirection(float MovementInputDirection, bool bUseCurrentDirection = false);
};
