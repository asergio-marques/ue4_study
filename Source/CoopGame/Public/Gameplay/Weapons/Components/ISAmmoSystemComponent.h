// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "ISAmmoSystemComponent.generated.h"




// Delegate declarations

// Declaration of delegate type for broadcast of ammo and available reload values upon ammo consumption or successful reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangeSignature, int32, BulletsCurrentlyAvailable, int32, AvailableReloads);

//Declaration of delegate type for broadcast of bullets-in (used for passive and per-bullet reload), sends 1 by default
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletsAddedSignature, uint8, NumBulletsAdded);

// Declaration of delegate type for broadcast of start of reload delay
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStartSignature, float, StartingDelay);

// Declaration of delegate type for broadcast of new bullets-in procedure
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReloadCycleStartSignature, float, CycleDelay, int32, NewBullets);

// Declaration of delegate type for broadcast of start of reload winddown
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadWinddownStartSignature, float, WinddownDelay);

// Declaration of delegate type for broadcast of reload completion with information
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadCompleteSignature, UReloadCancelTrigger, CancelType);




/*
 * Documentation goes here
 */
UCLASS( Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API UISAmmoSystemComponent : public UActorComponent {

	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UISAmmoSystemComponent();

	// Requests the cancelling of any reloading action that is in process and consumes a given number of bullets, returning true if successful in spending ammo (also returns the number of bullets truly consumed by the ammo system as the third parameter)
	virtual bool HandleAmmoUseRequest(bool CanBePartial, uint8 NumToExpend, uint8& BulletsTrulyExpended);

	// ABSTRACT // Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	virtual bool HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType = UReloadCancelTrigger::GenericPassiveTrigger);

	// ABSTRACT // Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	virtual bool HandleReloadCancel(const UReloadCancelTrigger& CancelType);

	// ABSTRACT // Activate the automatic properties of the weapon upon switching
	virtual void ActivateAutomaticProperties(void);


	// Status extracting functions (getters that depend on reload type)

	// ABSTRACT // Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
	virtual int32 GetMaximumBullets(void) const;

	// ABSTRACT // Expose the reload type
	virtual UReloadType GetReloadType(void) const;

	// Broadcasts information on generic request
	virtual void OnBroadcastRequested(void) const;


	// Delegates

	// Delegate to broadcast changes in the amount of currently-available ammo
	FOnAmmoChangeSignature AmmoChangedDelegate;
	
	// Delegate to broadcast changes in the amount of currently-available reloads
	FOnBulletsAddedSignature BulletsAddedDelegate;
	
	// Delegate to broadcast the start of a initial reload delay (for Passive and Per-Bullet)
	FOnReloadStartSignature ReloadStartedDelegate;
	
	// Delegate to broadcast the start of the actual reload (for Passive, Magazine and Per-Bullet)
	FOnReloadCycleStartSignature ReloadDelayEndedDelegate;
	
	// Delegate to broadcast the start of the reload winddown delay (for Magazine and Per-Bullet)
	FOnReloadWinddownStartSignature ReloadStoppedDelegate;

	// Delegate to broadcast the completion of the reload procedure and whether it was forcefully cancelled or not
	FOnReloadCompleteSignature ReloadCompletedDelegate;


	protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Status extracting functions (getters that depend on reload type)

	// ABSTRACT // Returns the number of leftover bullets/magazines, returns -1 for passive reload type
	virtual int32 GetAvailableReloads(void) const;


	// Status helpers
	
	// ABSTRACT // Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon
	virtual bool CanFire(bool CanBePartial, uint8 NumToExpend) const;


	// Tracker variables

	// Indicates if the weapon is being reloaded
	bool bIsReloading;

	// Indicates if the reload cancel is already underway
	bool bReloadIsBeingCancelled;

	// Counts the bullets currently available for the player to fire, used for all types of weapons (passive and active reload both, and bullet-per-bullet and magazine reload)
	int32 BulletsCurrentlyActive;

};