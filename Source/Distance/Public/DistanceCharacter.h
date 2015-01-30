// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Classes/PaperSpriteComponent.h"
#include "DistanceCharacter.generated.h"

UCLASS(Blueprintable)
class ADistanceCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* SpriteComponent;




public:
	ADistanceCharacter(const FObjectInitializer& ObjectInitializer);

	/** Player's current health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float Health;
	
	/** Player's maximum allowed health */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	/** Player's base damage level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	float lightAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Damage)
	bool bIsLightEnabled;

	/** Changes player's health based on an attack or item effect */
	UFUNCTION(BlueprintCallable, Category = Health)
	void ChangeHealth(float healthAmount);

	/** Changes player's target's health based on player attack */
	UFUNCTION(BlueprintCallable, Category = Damage)
	float Attack();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

