// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#import "DistanceCharacter.h"

/**
 * 
 */

class DISTANCE_API ConvergenceManager
{
private:
	// Intentional, don't allow instance of Convergence Manager
	ConvergenceManager();
	static ADistanceCharacter* player1;
	static ADistanceCharacter* player2;
public:
	
	static void InitializeWithPlayers(ADistanceCharacter* p1, ADistanceCharacter* p2);
	
	static void StartConvergence();
	
	static void EndConvergence();
	
	~ConvergenceManager();
};
