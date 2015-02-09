// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Item.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/Components/ChildActorComponent.h"
#include "ItemDrop.generated.h"

/**
 * 
 */
UCLASS()
class DISTANCE_API AItemDrop : public AActor
{
	GENERATED_BODY()
	
public:
	AItemDrop(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Root, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* ItemComponent;

	// to enable player to pick up item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
	class UBoxComponent* ItemTrigger;

	UFUNCTION(BlueprintCallable, Category = "Trigger Functions")
	bool PlayerPicksUpItem(class AActor* OtherActor);

	void PickUp();

	UFUNCTION(BlueprintNativeEvent, Category = "Trigger Functions")
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



	AItem* GetItem();
	
};
