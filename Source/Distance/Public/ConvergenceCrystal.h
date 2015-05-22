// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "ConvergenceCrystal.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DISTANCE_API AConvergenceCrystal : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float healthLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float drainHealthDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float movementSpeed;

	UPROPERTY(EditAnywhere)
	class ACharacter* player1;

	UPROPERTY(EditAnywhere)
	class ACharacter* player2;

	UPROPERTY(EditAnywhere)
	class AAIBoss_Doubt* bossDoubt;

	UPROPERTY(EditAnywhere)
	class AAIBoss_Betrayal* bossBetrayal;

	AConvergenceCrystal(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* ConvergenceCam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void LoseHealthTimer();
};
