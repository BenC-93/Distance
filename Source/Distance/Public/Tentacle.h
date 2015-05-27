// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Tentacle.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API ATentacle : public AActor
{
	GENERATED_BODY()
	
public:

	ATentacle(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	//void PostLoad() override;
	//void PostActorCreated() override;

	float health;
	float maxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers)
	class UBoxComponent *TriggerBox;

	class AAIBoss_Doubt* closestBoss;

	class AAIBoss_Doubt* doubtParent;

	void FindParent();

	void SetBossParent(class AAIBoss_Doubt* parent);

	void ChangeHealth(float amount);

	void DestroyTentacle();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroyTentacle();
};
