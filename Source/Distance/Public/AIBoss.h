// Copyright OvertimeGames 2015

#pragma once

#include "GameFramework/Character.h"
#include "AIBoss.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AAIBoss : public ACharacter
{
	GENERATED_BODY()
	
public:

	AAIBoss(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	virtual void EndOfBoss(bool KilledBoss = false);
	
};
