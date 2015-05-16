// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */

// changed to uint8 to make blueprintable
enum class ConvergenceState : uint8;

class DISTANCE_API ConvergenceManager
{
private:
	// Intentional, don't allow instance of Convergence Manager
	ConvergenceManager();
	static class ADistanceCharacter* player1;
	static class ADistanceCharacter* player2;
public:
	
	static void InitializeWithPlayers(class ADistanceCharacter* p1, class ADistanceCharacter* p2);
	
	static ConvergenceState StateFromLocation(FVector location);

	static void StartConvergence();
	
	static void EndConvergence();
	
	~ConvergenceManager();
};
