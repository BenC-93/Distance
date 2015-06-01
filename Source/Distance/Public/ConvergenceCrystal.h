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
	float maxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float healthLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float drainHealthDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crystal)
	float movementSpeed;

	// used for sprite bobbing
	float zflip;
	float zmin;
	float zmax;

	bool p1Near;
	bool p2Near;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	float LightIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
	class UPointLightComponent* LightSource;

	UPROPERTY(EditAnywhere)
	class ACharacter* player1;

	UPROPERTY(EditAnywhere)
	class ACharacter* player2;

	UPROPERTY(EditAnywhere)
	class AAIBoss* boss;

	bool inDoubtBoss;
	bool doubtTriggers;

	AConvergenceCrystal(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *FocusPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* ConvergenceCam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UBoxComponent* CaptureBox1;

	class UBoxComponent* CaptureBox2;
	class UBoxComponent* CaptureBox3;
	class UBoxComponent* CaptureBox4;

	void TurnCaptureBoxesOn();

	void TurnCaptureBoxesOff();

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void LoseHealthTimer();
	
	virtual void Destroyed() override;
};
