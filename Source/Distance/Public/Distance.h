// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#ifndef __DISTANCE_H__
#define __DISTANCE_H__

#include "EngineMinimal.h"
// We probably shouldn't be importing the entire engine here, but fast debugging.
// TODO: remove for real builds
#include "Engine.h"
#include "DistanceGameMode.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDistance, Log, All);

const float DEFAULT_SPRITE_PITCH = 0.0f;
const float DEFAULT_SPRITE_YAW = 90.0f;
const float DEFAULT_SPRITE_ROLL = -65.0f;

#define printScreen(c, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0, c, FString::Printf(__VA_ARGS__));

#define printLog(...) UE_LOG(LogDistance, Warning, __VA_ARGS__);

#endif