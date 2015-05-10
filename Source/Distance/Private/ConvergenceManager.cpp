// Fill out your copyright notice in the Description page of Project Settings.

#include "Distance.h"
#include "DistanceCharacter.h"
#include "ConvergenceManager.h"
#include <map>

const float HorizontalOffset = 30000.0f;	// y-offset, should be reflected as the offset to divergence1
const float VerticalOffset = 30000.0f;		// x-offset, should be reflected as the offset to divergence2
const FVector2D CentralPoint = FVector2D(15000.0f, 15000.0f);

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
			//show animation
			player->PlayConvergenceAnim();

			FVector currentLocation = player->GetActorLocation();
			// Shift player location to convergence state then to the desired state
			FVector newLocation = currentLocation - OffsetFromConvergenceState.at(CurrentState) + OffsetFromConvergenceState.at(NewState);
			player->GetController()->StopMovement();
			player->TeleportTo(newLocation, player->GetActorRotation(),false,true);
			player->PlayerConvergenceState = NewState;
			player->BPTransitionToNewConvergenceState();
		}
	}
	else
	{
		printScreen(FColor::Red, TEXT("Null player in state transition"));
	}
}

void ConvergenceManager::InitializeWithPlayers(ADistanceCharacter *p1, ADistanceCharacter *p2)
{
	ConvergenceState state;
	if (p1)
	{
		state = StateFromLocation(p1->GetActorLocation());
		p1->PlayerConvergenceState = state;
		// Set the persistent player references based on spawn location
		if (state == ConvergenceState::DIVERGENCE1) {
			player1 = p1;
		} else if (state == ConvergenceState::DIVERGENCE2) {
			player2 = p1;
		}
	}
	if (p2)
	{
		state = StateFromLocation(p2->GetActorLocation());
		p2->PlayerConvergenceState = state;
		// Set the persistent player references based on spawn location
		if (state == ConvergenceState::DIVERGENCE1) {
			player1 = p2;
		} else if (state == ConvergenceState::DIVERGENCE2) {
			player2 = p2;
		}
	}
}

ConvergenceState ConvergenceManager::StateFromLocation(FVector location)
{
	ConvergenceState state;
	// Locations right of the center are divergence 2
	if (location.X > CentralPoint.X) {
		state = ConvergenceState::DIVERGENCE2;
	} else {
		// Locations below the center are divergence 1
		if (location.Y > CentralPoint.Y) {
			state = ConvergenceState::DIVERGENCE1;
		} else {
			state = ConvergenceState::CONVERGENCE;
		}
	}
	return state;
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
