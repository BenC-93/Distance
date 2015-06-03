// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "DItemPickup.h"
#include "AIBoss.h"
#include "Tentacle.h"
#include "DistanceGameMode.generated.h"

UCLASS(minimalapi)
class ADistanceGameMode : public AGameMode
{
	GENERATED_BODY()
	
	uint32 BossIndex;
	
	uint32 CrystalsUsed;

public:
	ADistanceGameMode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category=Items, EditAnywhere)
	TArray<TSubclassOf<ADItemPickup>> ItemTypes;
	
	UPROPERTY(Category=Items, BlueprintReadWrite, EditAnywhere)
	uint8 TotalAvailableCrystals;
	
	// Transform location for the boss
	UPROPERTY(Category=Bosses, BlueprintReadWrite)
	FTransform BossSpawnTransform;
	
	// List of classes to spawn as Convergence bosses, spawned in order as players clear the previous boss
	UPROPERTY(Category=Bosses, EditAnywhere)
	TArray<TSubclassOf<AAIBoss>> BossClasses;
	
	UFUNCTION(BlueprintCallable, Category = "Convergence")
	void InitializeConvergence(class ADistanceCharacter* p1, class ADistanceCharacter* p2);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ADItemPickup* SpawnLantern(ACharacter* player);
	
	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ADItemPickup* SpawnItemAtLocation(TSubclassOf<class ADItemPickup> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ATentacle* SpawnTentacleAtLocation(TSubclassOf<class ATentacle> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	void SpawnBossAtLocation(TSubclassOf<class AAIBoss> indexClass, FVector location);
	
	UFUNCTION(BlueprintCallable, Category = Bosses)
	void SpawnBossForConvergence();
	
	UFUNCTION(BlueprintCallable, Category = Bosses)
	void AdvanceToNextBoss();
	
	void OnCrystalUsed(bool TriggeredConvergence);
	
	void OnEndConvergence(bool DefeatedBoss);

	UFUNCTION(BlueprintCallable, Category = Items)
	void SpawnRandomItemAtLocation(FVector location);
	
	virtual void StartPlay();
	
	virtual void PostLogin(APlayerController* NewPlayer);

	TSubclassOf<class ADItemPickup> ItemFromIndex(uint32 ItemIndex);
};
