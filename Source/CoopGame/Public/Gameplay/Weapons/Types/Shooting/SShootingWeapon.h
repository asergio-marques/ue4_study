// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Weapons/SWeapon.h"
#include "SShootingWeapon.generated.h"



class USAmmoSystemComponent;

/**
 *
 * Parent class for all weapons with shooting mechanics such as Ammo management (which in turn is implemented via the USAmmoSystemComponent class, ubiquitous in shooting weapons), and projectile fire.
 * It implements all logic related to control of weapon fire (frequency, damage, how it's triggered) and delegates the handling of weapon reload to the Ammo System Component. For control of weapon fire, the developer can specify whether firing mode they prefer out of the following three:
 *		Automatic Fire - Weapon fires continuously until there is no more ammo or the Weapon Action button is released; the rate of fire can be configured by the developer
 *		Manual Fire - Weapon fires only upon Weapon Action button press and after a given time has passed (rate of fire)
 *		Burst Fire - Weapon fires a configurable number of times with a configurable rate of fire; after it has fired that period, firing is disabled for a short period of time
 * 
 */
UCLASS()
class COOPGAME_API ASShootingWeapon : public ASWeapon {

	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ASShootingWeapon();

	// Execute the pressing action of this weapon's primary action
	virtual void OnPrimaryWeaponActionPressed() override;

	// Execute the releasing action of this weapon's primary action
	virtual void OnPrimaryWeaponActionReleased() override;
	
	// Check if reload is possible and call the AmmoSystemComponent reload function
	virtual void OnReloadPressed() override;

	// Implements the triggering of effects always active in shooting weapons
	virtual void TriggerAutomaticActions(void) override;

	// Requests broadcast of information from the subcomponents after this weapon's activation has been confirmed to be successful and broadcast
	virtual void RequestSubcomponentBroadcast(void) override;

	// Returns the reload type of this weapon (forwards ReloadType from AmmoSysComp)
	virtual UReloadType GetReloadType() const override;

	// Component that handles everything related to weapon ammo (bullets available to fire, reload, etc)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAmmoSystemComponent* AmmoSysComp;

	// Timer used to control the time between shots (used in automatic and burst shooting weapon types only)
	FTimerHandle TimerHandle_TimeBetweenShots;

	// Timer used to control the time between bursts (used in burst shooting weapon types only)
	FTimerHandle TimerHandle_TimeBetweenBursts;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Implements the cancelling of actions common to all shooting weapons
	virtual void CancelOngoingActions(void) override;

	// Polymorphic function that handles the specifics of the weapon being fired (e.g: raycast vs actor spawning)
	virtual bool HandleSpecificFiring(const uint8& BulletsConsumed);

	// Weapon attack parameters
	
	// Type of the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Parameters | General")
	UShootingWeaponType WeaponType;	
	
	// Amount of ammo consumed every time this weapon is fired
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | General", meta = (ClampMin = 1, ClampMax = 100))
	uint8 BulletPerAttack;

	// Amount of ammo consumed every time this weapon is fired
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | General", meta = (EditCondition = "BulletPerAttack > 1"))
	bool bCanPartialFire;

	
	// Automatic
	
	// Rate of fire variable for automatic weapons, number of bullets shot per second
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | Automatic", meta = (EditCondition = "WeaponType == UShootingWeaponType::AutomaticFire", ClampMin = 0.01, ClampMax = 50.0))
	float RateOfFireAutomatic;

	
	// Manual
	
	// Rate of fire variable, number of bullets shot per second
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | Manual", meta = (EditCondition = "WeaponType == UShootingWeaponType::ManualFire", ClampMin = 0.01, ClampMax = 10.0))
	float TimeBetweenShotsManual;

	
	// Burst
	
	// Amount of ammo consumed every time this weapon is fired
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | Burst", meta = (EditCondition = "WeaponType == UShootingWeaponType::BurstFire", ClampMin = 1, ClampMax = 10))
	uint8 ShotsInBurst;

	// Rate of fire variable for burst weapons, number of bullets shot per second
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | Burst", meta = (EditCondition = "WeaponType == UShootingWeaponType::BurstFire", ClampMin = 0.01, ClampMax = 50.0))
	float RateOfFireBurst;
	
	// Time between bursts of fire in burst-mode, in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parameters | Burst", meta = (EditCondition = "WeaponType == UShootingWeaponType::BurstFire", ClampMin = 0.10, ClampMax = 5.0))
	float TimeBetweenBursts;

	
	// Other properties
	
	// Particle effect to be emitted when the weapon is fired
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* MuzzleEffect;

	// Scale vector to be applied to the MuzzleEffect particle system when weapon is fired (it is recommended this vector is uniform)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FVector MuzzleEffectScale;	

	// Name of socket from which MuzzleParticleEffect shall be emitted
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FName MuzzleSocketName;
	
	// Dictates whether the user can trigger a reload via a button press
	UPROPERTY(EditAnywhere, Category = "User Definitions")
	bool bReloadTriggeredByButtonPress;

	// Status that represents whether a reload-blocking weapon action is being blocked or not
	bool bReloadBlockedByWeapon;
	
	// Time between shots in seconds; derived from RateOfFireAutomatic/RateOfFireManual
	float TimeBetweenFires;

	// Time in seconds since the last time this weapon fired a bullet
	float LastFireTime;


private:
	// Function dedicated to handling automatic fire
	virtual void StartFireAutomatic(float FirstDelay);

	// Function dedicated to handling manual fire with in-built delay and forced button pressing
	virtual void StartFireManual();

	// Function dedicated to handling burst fire (short bursts of automatic fire with in-built delay between them)
	virtual void StartFireBurst(float FirstDelay);
	
	// While the primary weapon action button is pressed, this function will be called continuously
	virtual void FireWeapon();

	// Resets the block for manual and burst firing
	virtual void ResetFiringBlock();
	
	// Triggers a reload via a Weapon action, always
	void TriggerReloadViaWeaponAction();
	
	// Emit the muzzle effect (happens in all shooting weapons)
	virtual void PlayMuzzleEffect();

	// flag that determines the period of time for which manual firing is blocked
	bool bBlockFiring;

	// counts the number of bullets shot in the current burst
	uint8 CurrentBurstCount;
	
};