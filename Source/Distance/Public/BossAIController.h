// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Engine.h"
#include "AIBoss_Doubt.h"
#include "BossAIController.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API ABossAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABossAIController(const FObjectInitializer& ObjectInitializer);
	
	virtual void Possess(class APawn *InPawn) override;

private:

	UPROPERTY(transient)
	class AAIBoss_Doubt* AIBoss_DoubtClass;
	
};
