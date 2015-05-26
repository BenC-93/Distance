// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ItemDataList.h"
#include "DItemPickup.h"
#include "Tentacle.h"
#include "DistanceGameMode.generated.h"

UCLASS(minimalapi)
class ADistanceGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ADistanceGameMode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Category=Items, EditAnywhere)
	TArray<TSubclassOf<ADItemPickup>> ItemTypes;
	
	UFUNCTION(BlueprintCallable, Category = "Convergence")
	void InitializeConvergence(class ADistanceCharacter* p1, class ADistanceCharacter* p2);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ADItemPickup* SpawnLantern(ACharacter* player);
	
	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ADItemPickup* SpawnItemAtLocation(TSubclassOf<class ADItemPickup> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	class ATentacle* SpawnTentacleAtLocation(TSubclassOf<class ATentacle> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Spawning)
	void SpawnBossAtLocation(TSubclassOf<class ACharacter> indexClass, FVector location);

	UFUNCTION(BlueprintCallable, Category = Items)
	void SpawnRandomItemAtLocation(FVector location);
	
	virtual void StartPlay();
	
	virtual void PostLogin(APlayerController* NewPlayer);

	TSubclassOf<class ADItemPickup> ItemFromIndex(uint32 ItemIndex);
};
