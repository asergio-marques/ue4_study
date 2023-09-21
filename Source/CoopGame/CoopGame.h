// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define SURFACE_FLESHDEFAULT			SurfaceType1
#define SURFACE_FLESHVULNERABLE			SurfaceType2
#define COLLISION_WEAPON				ECC_GameTraceChannel1

// Console debug command to display weapon-related debug lines
//static int32 DebugWeaponDrawing = 0;

#ifndef _DEBUGVAR
#define _DEBUGVAR

/*FAutoConsoleVariableRef DebugWeapons(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draws debug lines for weapons if > 0"),
	ECVF_Cheat);*/

static TAutoConsoleVariable<int32> DebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	0,
	TEXT("Draws debug lines for weapons if > 0"),
	ECVF_Cheat);

#endif // _DEBUGVAR