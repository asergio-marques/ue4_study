// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Weapons/Helpers/WeaponUtilities.h"
#include "SAmmoSystemComponent.generated.h"



// General delegates

// Declaration of delegate type for broadcast of ammo and available reload values upon ammo consumption or successful reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, newBulletsCurrentlyAvailable, int32, newAvailableReloads);

// Declaration of delegate type for broadcast of bullets-in (used for passive and per-bullet reload), sends 1 by default
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletAddedSignature, uint8, NumBulletsAdded);


// Passive reload-specific delegates

// Declaration of delegate type for broadcast of start of passive reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPassiveReloadStartedSignature, float, FirstDelay);

// Declaration of delegate type for broadcast of start of new bullets-in procedure for passive reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPassiveReloadCycleStartedSignature, float, BulletsInDelay);

// Declaration of delegate type for broadcast of passive reload pause/cancel
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPassiveReloadCancelledSignature, UReloadCancelTrigger, CancelType);


// Magazine reload-specific delegates

// Declaration of delegate type for broadcast of start of magazine reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagazineReloadStartedSignature, float, TimeToBulletsIn);

// Declaration of delegate type for broadcast of end of bullets-in period of magazine reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagazineReloadBulletsInSignature, float, TimeToCompletion);

// Declaration of delegate type for broadcast of end of wind-down period of magazine reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagazineReloadCompleteSignature);

// Declaration of delegate type for broadcast of magazine reload pause/cancel
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagazineReloadCancelledSignature);


// Per-bullet reload-specific delegates
// Per-bullet reload-specific delegates

// Declaration of delegate type for broadcast of start of per-bullet reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerBulletReloadStartedSignature, float, FirstDelay);

// Declaration of delegate type for broadcast of start of new bullets-in procedure for per-bullet reload
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerBulletReloadCycleStartedSignature, float, BulletsInDelay);

// Declaration of delegate type for broadcast of non-instant per-bullet reload pause/cancellation
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerBulletCancelledWithTimeSignature, float, TimeToCompletion);

// Declaration of delegate type for broadcast of per-bullet reload completion
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerBulletReloadCompleteSignature);

// Declaration of delegate type for broadcast of instant per-bullet reload pause/cancellation
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerBulletCancelledInstantSignature);



/*
 *
 * Master class that handles all ammo and reload functionality with minimal exposure to the weapon. It supports three types of reload:
 *		Passive Reload - Reload is triggered after a certain time without ammo-consuming weapon actions occurring, and automatically cancelled when weapon actions start occurring anew; ammo is added one-by-one every period of time
 *		Magazine Reload - Reload is triggered by the user (be it via weapon action when current magazine is empty or via reload key). Maximum ammo capacity is replenished after a given time, with an optional "wind-down" period after the magazine has been replaced; this reload can only be cancelled by the user if configured so (via the "Reload" key), and this is instantaneous
 *		Per-Bullet Reload - Reload is triggered by the user (be it via weapon action when current magazine is empty or via reload key). Ammo is added on a per-bullet basis (the number is configurable) after a "start-up" delay, and the reload can be cancelled by the user (only via weapon action) at any time
 *
 *
 * Integration hints:
 *		Setting the reload type and the respective parameters is to be done in the final weapon blueprint, these will "rule" over the basic functionality and control it
 *		The operations necessary for use in other classes containing the component are as follows:
 *			Assignment of pHUDInfo - A struct of type FSWeaponReloadInfo, available in WeaponUtilities.h, must be available. This struct will provide information from the ammo system to the player HUD. The assignment of this member of SAmmoSystemComponent must be done in the BeginPlay() function of the containing class; not doing so may crash the game
 *			UpdateHUDInfo should be called in the containing class after any action that may or will certainly change the status of the ammo system (e.g.: reloading, firing weapon, player stun).
 *			ManageAmmoWhenUseRequested() should be called whenever an action that expends ammo is executed, it performs all ammo system operations required: it checks whether the requested ammo usage can be expended (both in partial-fire and whole-fire capabilities both), and attempts to cancel any on-going reloading actions; only if this function returns true (meaning that, as far as the ammo system goes, the weapon can fire and any ammo has been expended) should further operations be made (e.g.: spawning grenade actors, raycasts to find damage targets. Care needs to be done when using this function however, as it will automatically expend the ammo if the AmmoSystem gives the OK; if there are things aside from the weapon itself that may block weapon actions (e.g.: player being stunned), those must be checked in the containing class BEFORE calling this function.
 *			HandleReloadTrigger() should be called for any reload-triggering or reload-cancelling operations; depending on the reload type chosen, it delegates to type-specific functions which check if the weapon is already reloading and try to cancel it depending on the action that caused HandleReloadTrigger to be called.
 *			
 *
 * Planned future development and features:
 *		Fix issue in PassiveReload() (see cpp file for hint)
 *		Encapsulate HUDInfo management entirely within the AmmoSystemComponent, but also prevent issues arising in weapons without any sort of AmmoSystem (e.g.: melee weapons)
 *		Networking
 *
 *		
 *
 * //////////////////////////////////////////////
 *
 * Detailed functionalities
 * 
 * Passive Reload functionality explanation:
 *		Passive reload follows the following schema:
 *		|---A---|--1--|-2-|-2-|...|---A---|
 *		Section A represents an ongoing reload-cancelling weapon action
 *		Section 1 is the "delay period", a configurable period of time that must pass after the end of an ammo-consuming weapon action before passive reload effectively begins
 *		Section 2 is the reload proper, after each while, 1 bullet is added; this is done over and over automatically until the maximum is met; the start of a new weapon action shall cancel this cycle
 *
 *
 * Magazine Reload functionality explanation:
 *		Magazine reload follows the following schema:
 *		|--1--|--2--|
 *		Section 1 is the "magazine replacement period", in which the current magazine is discarded and a new one introduced; at the end, the ammo capacity is fully restored
 *		Section 2 is the optional "wind-down" period, during which the weapon is still unavailable despite the ammo having already been introduced
 *		This type of reload can only be cancelled via "reload" button, and ONLY if it is configured to be cancellable
 *		Available reloads are displayed as leftover magazines
 *		
 *
 * Per-Bullet Reload functionality explanation:
 *		Bullet per bullet follows the following schema
 *		|--1--|-2-|-2-|-2-|-2-|---3---|
 *		Section 1 is the "wind up", in which no bullets are added and acts as a delay before ammo begins being loaded (use first delay on NewBulletsTimer)
 *		Section 2 is the reload proper, after each while, x bullets are added; this is done over and over automatically until the maximum is met
 *		Section 3 is the "wind-down" or "reload cancel period", the time penalty accrued for the reload being cancelled (even if it's because no more bullets can be reloaded ), which is implemented via the reload cancel
 *		This type of reload can only be cancelled via weapon action ("Reload" button will not work!)
 *		If the weapon is empty, sections 1 and 2 must be gone through once before the reload can be cancelled (currently active bullets must be equal or higher than the bullets reloaded every loop of section 2
 *		The "wind-down" period can be skipped in case the reload cancel was triggered in specific ways (e.g.: Player stunned, killed, etc)
 *		
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USAmmoSystemComponent : public UActorComponent {
	
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	USAmmoSystemComponent();

	
	// Main functions to be called outside

	// Requests the cancelling of any reloading action that is in process and consumes a given number of bullets, returning true if successful in spending ammo (also returns the number of bullets truly consumed by the ammo system as the third parameter)
	bool ManageAmmoWhenUseRequested(bool CanBePartial, uint8 NumToExpend, uint8& BulletsTrulyExpended);
	
	// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadTrigger(bool WeaponIsBusy, const UReloadCancelTrigger CancelType = UReloadCancelTrigger::GenericPassiveTrigger);

	// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadCancel(const UReloadCancelTrigger& CancelType);

	// Activate the automatic properties of the weapon upon switching
	void ActivateAutomaticProperties(void);

	
	// Status extracting functions (getters that depend on reload type)

	// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
	FORCEINLINE int32 GetMaximumBullets(void) const;
	
	// Expose the reload type
	FORCEINLINE UReloadType GetReloadType(void) const;

	// Broadcasts information on generic request
	void OnBroadcastRequested(void) const;

	
	// Public class members

	
	// Delegates
	
	// Delegate to broadcast new ammo and available reload values
	FOnAmmoChangedSignature AmmoChangedDelegate;

	// Delegate to broadcast the addition of bullets into a weapon (used for passive and per-bullet reload broadcast)
	FOnBulletAddedSignature BulletsAddedDelegate;

	
	// Passive reload-specific delegates
	
	// Delegate to broadcast the start of passive reload and its parameters
	FOnPassiveReloadStartedSignature PassiveStartDelegate;

	// Delegate to broadcast the start of a passive reload cycle and its duration
	FOnPassiveReloadCycleStartedSignature PassiveReloadCycleStartDelegate;
	
	// Delegate to broadcast the cancellation of passive reload 
	FPassiveReloadCancelledSignature PassiveCancelDelegate;

	
	// Magazine reload-specific delegates
	
	// Delegate to broadcast the start of magazine reload and its parameters
	FOnMagazineReloadStartedSignature MagazineStartDelegate;

	// Delegate to broadcast bullets being inserted in magazine and the parameters for the new stage
	FOnMagazineReloadBulletsInSignature MagazineBulletsInDelegate;

	// Delegate to broadcast the proper conclusion of magazine reload
	FOnMagazineReloadCompleteSignature MagazineCompleteDelegate;

	// Delegate to broadcast the cancellation of magazine reload
	FOnMagazineReloadCancelledSignature MagazineCancelDelegate;

	
	// Per-bullet reload-specific delegates
	
	// Delegate to broadcast the start of per-bullet reload and its initial delay
	FOnPerBulletReloadStartedSignature PerBulletStartDelegate;

	// Delegate to broadcast the start of a per-bullet reload cycle and its duration
	FOnPerBulletReloadCycleStartedSignature PerBulletReloadCycleStartDelegate;
	
	// Delegate to broadcast the cancellation of per-bullet reload with winddown
	FOnPerBulletCancelledWithTimeSignature PerBulletTimedCancelDelegate;

	// Delegate to broadcast the completion of the per-bullet reload procedure
	FOnPerBulletReloadCompleteSignature PerBulletCompleteDelegate;

	// Delegate to broadcast the cancellation of per-bullet reload without winddown
	FOnPerBulletCancelledInstantSignature PerBulletInstantCancelDelegate;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Variable that states what reload rules does the ammo system follow
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Type")
	UReloadType ReloadType;

	
	// Passive reload properties
	
	// If true, reloading is done without player actively triggering it after a certain delay; otherwise, reload needs to be triggered via the "Reload" key
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	bool bIsPassiveReload;

	// Maximum number of bullets that can be restored via passive regeneration 
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Passive", meta = (EditCondition = "ReloadType == UReloadType::PassiveReload", ClampMin = 1, ClampMax = 255))
	uint8 MaximumPassiveCapacity;
	
	// Time in seconds for the delay between a weapon's most recent action and passive reload being possible
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Passive", meta = (EditCondition = "ReloadType == UReloadType::PassiveReload", ClampMin = 0.01, ClampMax = 300.0))
	float PassiveReloadDelay;

	// Time in seconds for a single bullet to regenerate passively
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Passive", meta = (EditCondition = "ReloadType == UReloadType::PassiveReload", ClampMin = 0.01, ClampMax = 300.0))
	float ReloadTimePassive;

	
	// Magazine reload properties

	// If true, reloading a magazine can be halted by pressing the "Reload" key during reloading; otherwise, the act of reloading can only be halted by switching the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload"))
	bool bEnableMagazineReloadCancel;
	
	// Total number of bullets in a single magazine
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload", ClampMin = 1, ClampMax = 1000))
	int32 BulletPerMagazine;

	// Time in seconds for the ammo to be added after reload is triggered, should be lower than FullReloadTimeMagazine
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload", ClampMin = 0.01, ClampMax = 30.0))
	float BulletsReloadTimeMagazine;
	
	// Time in seconds after the bullet reload time for the action of magazine reloading to be finalized
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload", ClampMin = 0.01, ClampMax = 30.0))
	float WinddownReloadTimeMagazine;
	
	// If true, there is a maximum of magazines (and hence, reloads) available per life, the counter is only reset upon death; otherwise, magazine reloads can be done infinitely within a single life
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload"))
	bool bCapMagazinesPerLife;

	// Total number of magazines available to a player per life (initially available magazines included)
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Magazine", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload && bCapMagazinesPerLife", ClampMin = 1, ClampMax = 100))
	uint8 MagazinesPerLife;

	
	// Active reload, bullet-per-bullet
	
	// Number of bullets restored per reload
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload", ClampMin = 1, ClampMax = 10))
	uint8 BulletPerReload;

	// Maximum number of bullets that can be restored after reload is triggered (must be a multiple of intBulletPerReload) 
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload", ClampMin = 1, ClampMax = 255))
	uint8 MaximumBulletCapacity;
	
	// Time in seconds for the initial delay before reloading starts proper on weapons with X-bullets-per-X-bullets
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload", ClampMin = 0.01, ClampMax = 10.0))
	float ReloadDelayTimeBullet;

	// Time in seconds for the action of reloading intBulletPerReload bullets on weapons with X-bullets-per-X-bullets
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload", ClampMin = 0.01, ClampMax = 10.0))
	float ReloadTimeBullet;

	// Time in seconds for the delay before weapon is available after reloading is cancelled on weapons with X-bullets-per-X-bullets
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload", ClampMin = 0.01, ClampMax = 10.0))
	float ReloadEndTimeBullet;

	// If true, there is a maximum of bullets per player life for this weapon, the counter is only reset upon death; otherwise, X-bullets-per-X-bullets reloads can be done infinitely within a single life
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload"))
	bool bCapBulletsPerLife;

	// Total number of bullets available to a player per life (initially available bullets included)
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Bullet-per-bullet", meta = (EditCondition = "ReloadType == UReloadType::ActivePerBulletReload && bCapBulletsPerLife", ClampMin = 1, ClampMax = 10000))
	int32 BulletsPerLife;
	

	// Permissions for actions while reloading

	// If true, the act of reloading stops all player movement, overriding bStopSpecialMovement
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Other Actions", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload || ReloadType == UReloadType::ActivePerBulletReload"))
	bool bStopAllMovement;
	
	// If true, the act of reloading stops all other weapon actions, primary and secondary both; movement actions stay unchanged
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Other Actions", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload || ReloadType == UReloadType::ActivePerBulletReload"))
	bool bStopWeaponActions;
	
	// If true, reloading stops only special player movement actions (dashing, walljump, etc)
	UPROPERTY(EditDefaultsOnly, Category = "Reload | Other Actions", meta = (EditCondition = "ReloadType == UReloadType::ActiveMagazineReload || ReloadType == UReloadType::ActivePerBulletReload"))
	bool bStopSpecialMovement;

	
private:
	// Internal functions
	// Status extracting functions (getters that depend on reload type)

	// Returns the number of leftover bullets/magazines depending on the active reload type; if the reload type is passive, this function returns 0
	int32 GetAvailableReloads(void) const;


	// Status helpers
	
	// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
	// Internal function called only in the scope of CanFireAndExpendBullets
	bool CanFire(bool CanBePartial, uint8 NumToExpend) const;
	
	// Returns true if the request performed and the weapon status and configuration is proper for triggering of passive-type reload
	bool CanTriggerPassiveReload(const UReloadCancelTrigger& CancelType) const;

	// Returns true if the request performed and the weapon status and configuration is proper for triggering of magazine-type reload
	bool CanTriggerMagazineReload(bool OtherWeaponActionOccurring) const;

	// Returns true if the request performed and the weapon status and configuration is proper for triggering of per-bullet-type reload
	bool CanTriggerPerBulletReload(bool OtherWeaponActionOccurring) const;
	
	// Returns true if the request performed and the weapon status and configuration is proper for cancellation of passive-type reload
	bool CanCancelPassiveReload(UReloadCancelTrigger CancelType) const;
	
	// Returns true if the request performed and the weapon status and configuration is proper for cancellation of magazine-type reload
	bool CanCancelMagazineReload(UReloadCancelTrigger CancelType) const;

	// Returns true if the request performed and the weapon status and configuration is proper for cancellation of per-bullet-type reload
	bool CanCancelPerBulletReload(UReloadCancelTrigger CancelType) const;

	
	// Handler functions

	// Function dedicated to handling passive reload, returns success
	bool TriggerPassiveReload(const UReloadCancelTrigger& CancelType);

	// Function dedicated to handling passive reload, returns success
	bool TriggerMagazineReload(bool OtherWeaponActionOccurring);
	
	// Function dedicated to handling passive reload, returns success
	bool TriggerPerBulletReload(bool OtherWeaponActionOccurring);
	
	// Inner function that handles reload cancelling in passive reload type weapons, after the conditions have been checked
	bool CancelPassiveReload(UReloadCancelTrigger CancelType);
	
	// Inner function that handles reload cancelling in active magazine reload type weapons, after the conditions have been checked
	bool CancelMagazineReload(UReloadCancelTrigger CancelType);

	// Inner function that handles reload cancelling in active per-bullet reload type weapons, after the conditions have been checked (implements step 3 of the per-bullet reload schema)
	bool CancelPerBulletReload(UReloadCancelTrigger CancelType);

	
	// Timer functions in reload
	
	// Function called once the initial delay for passive reload passes
	void OnStartDelayFinishedPassiveReload(void);

	// Function called every time ReloadTimePassive elapses in passive reload weapons
	void OnBulletsInPassiveReload(void);

	// Function called when the ammo is added to the currently available ammo
	void OnBulletsInMagazineReload(void);

	// Function called when the animation for magazine reload is complete
	void OnCompleteMagazineReload(void);

	// Function called once the initial delay for per-bullet reload passes
	void OnStartDelayFinishedPerBulletReload(void);
	
	// Function called after the initial delay for bullet-per-bullet reload has finished (section 1, wind up)
	void OnBulletsInPerBulletReload(void);

	// Inner function that handles the cancelling of the per-bullet reload proper in case it's a timed cancel
	void OnCompleteWithSuccessPerBulletReload(void);

	// Inner function that handles the cancelling of the per-bullet reload proper in case it's an instant cancel
	void OnCompleteWithCancelPerBulletReload(void);

	// Inner function that handles the cancelling of the per-bullet reload proper
	void OnCompletePerBulletReload(void);

	
	// Tracker variables
	// Indicates if the weapon is being reloaded
	bool bIsReloading;

	// Indicates if the reload cancel is already underway
	bool bReloadIsBeingCancelled;

	// Counts the bullets currently available for the player to fire, used for all types of weapons (passive and active reload both, and bullet-per-bullet and magazine reload)
	int32 BulletsCurrentlyActive;

	// Tracks the number of magazines already used in the player's life (reset upon character death and respawn)
	uint8 MagazinesExpended;

	// Tracks the number of bullets loaded into the weapon in the player's life (reset upon character death and respawn)
	int32 BulletsExpended;

	// Tracks the number of times bullets have been loaded into the weapon for the currently-active per-bullet reload cycle (0 if reloading is not happening)
	uint8 PerBulletReloadCycles;

	
	// Timers

	// Timer for handling the initial delay in passive and per-bullet reloads
	FTimerHandle TimerHandle_InitialDelayTimer;
	
	// Timer for the update of the currently available bullets
	FTimerHandle TimerHandle_NewBulletsTimer;

	// Timer for the completion of the reload animation/unavailability of the weapon
	FTimerHandle TimerHandle_ReloadCompletion;
	
};
