// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/Components/ISAmmoSystemComponent.h"
#include "SPerBulletReloadComponent.generated.h"




/**
 * Documentation goes here.
 */
UCLASS()
class COOPGAME_API USPerBulletReloadComponent : public UISAmmoSystemComponent {
	
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USPerBulletReloadComponent();

	// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType = UReloadCancelTrigger::GenericPassiveTrigger) override;

	// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadCancel(const UReloadCancelTrigger& CancelType) override;


	// Status extracting functions (getters that depend on reload type)

	// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
	int32 GetMaximumBullets(void) const override;

	// Expose the reload type
	UReloadType GetReloadType(void) const override;


protected:
	// Called when the game starts
	void BeginPlay() override;


	// Status extracting functions (getters that depend on reload type)

	// Returns the number of leftover bullets/magazines depending on the active reload type; if the reload type is passive, this function returns 0
	int32 GetAvailableReloads(void) const override;


	// Status helpers

	// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
	bool CanFire(bool CanBePartial, uint8 NumToExpend) const override;

	// Returns true if the request performed and the weapon status and configuration is proper for triggering the reload process
	bool CanTriggerReload(bool OtherWeaponActionOccurring) const;

	// Returns true if the request performed and the weapon status and configuration is proper for cancellation of reload process
	bool CanCancelReload(UReloadCancelTrigger CancelType) const;


protected:
	// Number of bullets restored per reload
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 1, ClampMax = 10))
	uint8 BulletPerReload;

	// Maximum number of bullets that can be held simultaneously
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 1, ClampMax = 255))
	uint8 MaximumCapacity;

	// Time in seconds for the initial delay before reloading starts proper
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 10.0))
	float InitialDelay;

	// Time in seconds for the action of reloading BulletPerReload bullets
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 10.0))
	float ReloadCycleDuration;

	// Time in seconds for the delay before weapon is available after reloading is cancelled
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 10.0))
	float WinddownDelay;

	// If true, there is a maximum of bullets per player life for this weapon, the counter is only reset upon death; otherwise, reloads can be done infinitely within a single life
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	bool bCapBulletsPerLife;

	// Total number of bullets available to a player per life (initially available bullets included)
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (EditCondition = "bCapBulletsPerLife", ClampMin = 1, ClampMax = 10000))
	int32 BulletsPerLife;


private:
	// Internal functions

	// Function called once the initial delay has elapsed
	void OnInitialDelayFinished(void);

	// Function called every time the reload cycle duration elapses
	void OnBulletsIn(void);

	// Function called once the winddown period has passed (the winddown period is set in motion by the HandleReloadCancel function)
	void OnReloadComplete(UReloadCancelTrigger CancelType);


	// Tracker variables

	// Tracks the number of bullets loaded into the weapon in the player's life (reset upon character death and respawn)
	int32 BulletsExpended;

	// Tracks the number of times bullets have been loaded into the weapon for the currently-active per-bullet reload cycle (0 if reloading is not happening)
	uint8 PerBulletReloadCycles;


	// Timers

	// Timer for handling the initial delay
	FTimerHandle TimerHandle_InitialDelayTimer;

	// Timer for the update of the currently available bullets
	FTimerHandle TimerHandle_NewBulletsTimer;

	// Timer for the completion of the reload animation/unavailability of the weapon
	FTimerHandle TimerHandle_ReloadCompletion;

};
