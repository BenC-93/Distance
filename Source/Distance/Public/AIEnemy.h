// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Classes/PaperSpriteComponent.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite")
	class UPaperSpriteComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerRange;//for approaching

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* AITriggerAttack;

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	void OnAttackTrigger(class AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	bool CheckIfPlayer(class AActor* OtherActor);

	/*UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBeginAttack(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapEndAttack(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

	UPROPERTY(EditAnywhere)
	bool moveToPlayer;

	UPROPERTY(EditAnywhere)
	bool moveAway;

	UPROPERTY(EditAnywhere)
	class ACharacter* player1;

	UPROPERTY(EditAnywhere)
	class ACharacter* player2;

	UPROPERTY(EditAnywhere)
	class ACharacter* currentPlayer;

private:
	
	float health;
	float maxHealth;
	float baseDamage;
	
	void ChangeHealth(float amount);

	void Attack(float amount);
	
};
