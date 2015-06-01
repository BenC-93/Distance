// Copyright OvertimeGames 2015

#include "Distance.h"
#include "AIBoss.h"

AAIBoss::AAIBoss(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Health = 100.0f;
	MaxHealth = 100.0f;
}

void AAIBoss::EndOfBoss(bool KilledBoss)
{
	GetWorld()->GetAuthGameMode<ADistanceGameMode>()->OnEndConvergence(KilledBoss);
}