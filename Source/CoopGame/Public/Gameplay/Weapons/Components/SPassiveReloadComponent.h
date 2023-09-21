// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/Components/ISAmmoSystemComponent.h"
#include "SPassiveReloadComponent.generated.h"




/**
 * Documentation goes here
 */
UCLASS()
class COOPGAME_API USPassiveReloadComponent final : public UISAmmoSystemComponent {

	GENERATED_BODY()
	

public:
	// Sets default values for this component's properties
	USPassiveReloadComponent();

	// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType = UReloadCancelTrigger::GenericPassiveTrigger) override;

	// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
	bool HandleReloadCancel(const UReloadCancelTrigger& CancelType) override;

	// Activate passive reload automatically
	void ActivateAutomaticProperties(void) override;


	// Status extracting functions (getters that depend on reload type)

	// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
	int32 GetMaximumBullets(void) const override;

	// Expose the reload type
	UReloadType GetReloadType(void) const override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	// Status extracting functions (getters that depend on reload type)

	// Returns the number of leftover bullets/magazines depending on the active reload type, passive is always -1
	int32 GetAvailableReloads(void) const override;


	// Status helpers

	// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
	bool CanFire(bool CanBePartial, uint8 NumToExpend) const override;

	// Returns true if the request performed and the weapon status and configuration is proper for triggering the reload process
	bool CanTriggerReload(UReloadCancelTrigger CancelType) const;

	// Returns true if the request performed and the weapon status and configuration is proper for cancellation of reload process
	bool CanCancelReload(UReloadCancelTrigger CancelType) const;


	// Configurable properties

	// If true, reloading a magazine can be halted by pressing the "Reload" key during reloading; otherwise, the act of reloading can only be halted by switching the weapon
	// Maximum number of bullets that can be restored via passive regeneration 
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 1, ClampMax = 255))
	uint8 MaximumCapacity;

	// Time in seconds for the delay between a weapon's most recent action and passive reload being possible
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 300.0))
	float InitialDelay;

	// Time in seconds for a single bullet to regenerate passively
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 300.0))
	float ReloadCycleDuration;


private:
	// Internal functions
	// Function called once the initial delay has elapsed
	void OnInitialDelayFinished(void);

	// Function called every time the reload cycle duration elapses
	void OnBulletsIn(void);


	// Timers

	// Timer for handling the initial delay
	FTimerHandle TimerHandle_InitialDelayTimer;

	// Timer for the update of the currently available bullets
	FTimerHandle TimerHandle_NewBulletsTimer;

};
