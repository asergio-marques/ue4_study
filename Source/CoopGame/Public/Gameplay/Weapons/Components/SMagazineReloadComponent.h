// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/Components/ISAmmoSystemComponent.h"
#include "SMagazineReloadComponent.generated.h"




/**
 * Documentation goes here
 */
UCLASS()
class COOPGAME_API USMagazineReloadComponent final : public UISAmmoSystemComponent {

	GENERATED_BODY()
	

public:
	// Sets default values for this component's properties
	USMagazineReloadComponent();

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


	// Configurable properties

	// If true, reloading a magazine can be halted by pressing the "Reload" key during reloading; otherwise, the act of reloading can only be halted by switching the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	bool bEnableReloadCancel;

	// Total number of bullets in a single magazine
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 1, ClampMax = 1000))
	int32 BulletPerMagazine;

	// Time in seconds for the ammo to be added after reload is triggered
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 30.0))
	float BulletsInTime;

	// Time in seconds after the bullet reload time for the action of magazine reloading to be finalized
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (ClampMin = 0.01, ClampMax = 30.0))
	float WinddownDelay;

	// If true, there is a maximum of magazines (and hence, reloads) available per life, the counter is only reset upon death; otherwise, magazine reloads can be done infinitely within a single life
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	bool bCapMagazinesPerLife;

	// Total number of magazines available to a player per life (initially available magazines included)
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (EditCondition = "bCapMagazinesPerLife", ClampMin = 1, ClampMax = 100))
	uint8 MagazinesPerLife;

private:
	// Internal functions
	// Function called when the ammo is added to the currently available ammo
	void OnBulletsIn(void);

	// Function called when the animation for magazine reload is complete
	void OnReloadComplete(void);
	
	
	// Tracker variables
	
	// Tracks the number of magazines already used in the player's life (reset upon character death and respawn)
	uint8 MagazinesExpended;


	// Timers

	// Timer for the update of the currently available bullets
	FTimerHandle TimerHandle_NewBulletsTimer;

	// Timer for the completion of the reload animation/unavailability of the weapon
	FTimerHandle TimerHandle_ReloadCompletion;

};
