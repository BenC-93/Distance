// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "AIEnemy.h"

AAIEnemy::AAIEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	health = 100.0f;
	maxHealth = 100.0f;
	baseDamage = 20.0f;

	/*
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;*/

	/*
	Creaete a radius field for checking if player is nearby*/
}

void AAIEnemy::ChangeHealth(float amount)
{

}

void AAIEnemy::Attack(float amount)
{

}


