// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Classes/PaperSpriteComponent.h"
#include "Spirit.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DISTANCE_API ASpirit : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spirit)
	float lifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spirit)
	float movementSpeed;

	UPROPERTY(EditAnywhere, Category = Spirit)
	ADistanceCharacter* OwningPawn;

	ASpirit(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent *SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Triggers)
	class UBoxComponent *TriggerBox;

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	ADistanceCharacter* GetOwningPawn();

	UFUNCTION(BlueprintCallable, Category = Spirit)
	void SetOwner(ADistanceCharacter* owningPlayer);

	UFUNCTION(BlueprintCallable, Category = "Timers")
	void DeathTimer();
	
};
