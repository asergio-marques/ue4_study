// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "WeaponUtilities.generated.h"



/*
 *
 * This file contains a number of auxiliary classes, structs and enums for better definition of weapon behavior
 * Currently, it implements:
 *		UShootingWeaponType - Enum that signals the way in which a shooting weapon executes its firing
 *		UReloadType - Enum that signals the way in which a shooting weapon executes its reload
 *		FSWeaponReloadInfo - Struct containing information from the ReloadSystem to be displayed on the HUD
 *		UReloadCancelTrigger - Enum that allows us to better identify the trigger that has led to a request to cancel the current reloading action
 *		
 */


/*
 *
 *	Variable type that specifies the way this weapon shoots
 *		AutomaticFire = weapon action can be held continuously and the weapon will shoot as long as bullets are available
 *		SingleShotFire = weapon action is only executed at the moment the respective button is pressed, holding it will yield no further action (however, there still is a maximum rate of fire to stop spam)
 *		BurstFire = weapon action causes a given number of bullets to be fired in quick succession much as if the weapon were of AutomaticFire type, but stops for a moment after that time is fulfilled before firing again; this continues until ammo is deplated
 *	
 */
UENUM(BlueprintType)
enum class UShootingWeaponType : uint8 {

	Invalid = 0,
	AutomaticFire = 1,
	ManualFire = 2,
	BurstFire = 3
	
};



/*
 *
 *	Variable type that specifies the Reload type applicable to a weapon
 *	Note that "X > UReloadType::PassiveReload" means "X == UReloadType::ActiveMagazineReload || X == UReloadType::ActivePerBulletReload"
 *	
 */
UENUM(BlueprintType)
enum class UReloadType : uint8 {

	NoReload = 0,
	PassiveReload = 1,
	ActiveMagazineReload = 2,
	ActivePerBulletReload = 3
	
};


/*
 *
 *	Struct containing all the info relative to the player loadout (weapons and skill)
 *
 */
USTRUCT(Blueprintable)
struct FSEquipmentInfoHUD {
	
	GENERATED_USTRUCT_BODY()

	
public:
	// Signals if this slot has a weapon equipped
	bool IsEquipped;
	
	// Name of the weapon/skill
	FName EquipmentShortName;

	// Number of the slot for this item (0 if skill)
	uint8 EquipmentSlot;

	
	// Constructor
	FSEquipmentInfoHUD() :
		IsEquipped(false),
		EquipmentShortName(TEXT("Generic")),
		EquipmentSlot(0) {
		
	}
	
};


/*
 *
 *	Struct containing the all info necessary from a weapon for the player HUD
 *	As this is only necessary at BeginPlay and weapons cannot be switched out of the loadout during the game, an instance of this struct should never be modified.
 *
 */
USTRUCT(Blueprintable)
struct FLoadoutInfo {
	
	GENERATED_USTRUCT_BODY()

	
public:
	// Information for Weapon in slot 1
	FSEquipmentInfoHUD Weapon1Info;

	// Information for Weapon in slot 2
	FSEquipmentInfoHUD Weapon2Info;

	// Information for Weapon in slot 3
	FSEquipmentInfoHUD Weapon3Info;

	// Information for Skill
	FSEquipmentInfoHUD SkillInfo;
	
	// Constructor
	FLoadoutInfo() :
		Weapon1Info(),
		Weapon2Info(),
		Weapon3Info(),
		SkillInfo() {
		
	}
	
};


/*
 *
 *	Variable type that allows the developer to specify the cause of a reload cancel request and process it accordingly
 *	
 */
UENUM(BlueprintType)
enum class UReloadCancelTrigger : uint8 {

	// Values with which reload cancel isn't executed, ever
	Invalid = 0,						// Default invalid value
	TriggerByWeaponAction = 1,			// When Ammo = 0 and player fires again

	// Values with which reload cancel is only executed if bEnableMagazineReloadCancel == true
	TriggerByReload = 2,				// When "Reload" button is pressed

	// Value with which reload cancel is always executed
	TriggerByLimitReached = 3,			// When per-bullet reload reaches the maximum
	TriggerByWeaponSwitch = 4,			// When Weapon is switched out by players
	TriggerByExternal = 5,				// When the cancel is caused by outside factors
	GenericPassiveTrigger = 6			// Default value for triggering passive reload (necessary for filtering out reload key triggers)
	
};