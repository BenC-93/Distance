// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "ConvergenceManager.h"
#include <map>

const float HorizontalOffset = 30000.0f;	// y-offset, should be reflected as the offset to divergence1
const float VerticalOffset = 30000.0f;		// x-offset, should be reflected as the offset to divergence2

std::map<ConvergenceState,FVector> OffsetFromConvergenceState = {
	{ConvergenceState::CONVERGENCE, FVector(0.0, 0.0, 0.0)},
	{ConvergenceState::DIVERGENCE1, FVector(0.0, HorizontalOffset, 0.0)},
	{ConvergenceState::DIVERGENCE2, FVector(VerticalOffset, 0.0, 0.0)}
};

ConvergenceManager::ConvergenceManager()
{
}

ConvergenceManager::~ConvergenceManager()
{
}

// Initialization of static member variables so they can be used
ADistanceCharacter* ConvergenceManager::player1 = NULL;
ADistanceCharacter* ConvergenceManager::player2 = NULL;

void TransitionPlayerToState(ADistanceCharacter* player, ConvergenceState NewState)
{
	if (player)
	{
		ConvergenceState CurrentState = player->PlayerConvergenceState;
		if (CurrentState == NewState)
		{
			// No state transition just log a message and stop
			printScreen(FColor::Red, TEXT("Attempted to transition player to current state"));
			// Temporary for debugging transition back to divergence
			ConvergenceManager::EndConvergence();
		}
		else
		{
			FVector currentLocation = player->GetActorLocation();
			// Shift player location to convergence state then to the desired state
			FVector newLocation = currentLocation - OffsetFromConvergenceState.at(CurrentState) + OffsetFromConvergenceState.at(NewState);
			player->GetController()->StopMovement();
			player->TeleportTo(newLocation, player->GetActorRotation());
			player->PlayerConvergenceState = NewState;
			if (player->PlayerConvergenceState == ConvergenceState::CONVERGENCE)
			{
				player->BPTransitionToConvergenceState();
			}
			else
			{
				player->BPTransitionToDivergenceState();
			}
		}
	}
	else
	{
		printScreen(FColor::Red, TEXT("Null player in state transition"));
	}
}

void ConvergenceManager::InitializeWithPlayers(ADistanceCharacter *p1, ADistanceCharacter *p2)
{
	player1 = p1;
	if (player1)
	{
		player1->PlayerConvergenceState = ConvergenceState::DIVERGENCE1;
	}
	player2 = p2;
	if (player2)
	{
		player2->PlayerConvergenceState = ConvergenceState::DIVERGENCE2;
	}
}

void ConvergenceManager::StartConvergence()
{
	printScreen(FColor::Red, TEXT("Start for player 1"));
	TransitionPlayerToState(player1, ConvergenceState::CONVERGENCE);
	printScreen(FColor::Red, TEXT("Start for player 2"));
	TransitionPlayerToState(player2, ConvergenceState::CONVERGENCE);
}

void ConvergenceManager::EndConvergence()
{
	printScreen(FColor::Red, TEXT("End for player 1"));
	TransitionPlayerToState(player1, ConvergenceState::DIVERGENCE1);
	printScreen(FColor::Red, TEXT("End for player 2"));
	TransitionPlayerToState(player2, ConvergenceState::DIVERGENCE2);
}
