// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AIEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIEnemy : public ACharacter
{
	GENERATED_BODY()
	
public:
	AAIEnemy(const FObjectInitializer& ObjectInitializer);

private:
	float health;
	float maxHealth;
	float baseDamage;

	void ChangeHealth(float amount);

	void Attack(float amount);
	
	
};
