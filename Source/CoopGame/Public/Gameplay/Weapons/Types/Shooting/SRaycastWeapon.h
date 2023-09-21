// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SShootingWeapon.h"
#include "SRaycastWeapon.generated.h"



/*
 *
* This class implements the specifics of shooting weapons that use raycasts to determine if there are any damageable actors in a pseudo-projectile's path, for weapons that are designed to have fast-moving projectiles.
 * For this purpose, it overrides the HandleSpecificFiring function to do raycast-specific operations, and implements helper functions that deal with firing aspects specific to ShootingWeapons (bullet path, impact effects VFX) and extra hit effects.
 * For the moment, this class also implements a zoom-in/zoom-out functionality for the Secondary Weapon Action button (Right Mouse Button).
 * 
 */
UCLASS()
class COOPGAME_API ASRaycastWeapon : public ASShootingWeapon {
	
	GENERATED_BODY()

	
public:
	// Sets default values for this actor's properties
	ASRaycastWeapon();

	// Execute the pressing action of this weapon's secondary action
	virtual void OnSecondaryWeaponActionPressed() override;

	// Execute the releasing action of this weapon's secondary action
	virtual void OnSecondaryWeaponActionReleased() override;

	
protected:
	// Implements the cancelling of actions common to all raycast weapons
	virtual void CancelOngoingActions(void) override;

	// Implements the logic specific to this subclass of shooting weapon; in this case, it implements the raycast hit detection
	// the overload allows us to trace more than one ray at a time
	virtual bool HandleSpecificFiring(const uint8& BulletsConsumed) override;
	
	// Particle effect to be emitted to trace the raycast in the world, simulating a bullet's path
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* TracerEffect;

	// Name of particle system property for the target of the TracerEffect emitter
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FName TracerTargetName;

	// Particle effect to be emitted if the raycast hits SurfaceType1 (FleshDefault)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* DefaultImpactEffect;

	// Particle effect to be emitted if the raycast hits SurfaceType2 (FleshVulnerable)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* FleshImpactEffect;

	
private:
	// Casts a single ray in the direction the player character is looking towards and checks if it is blocked by anything
	// Returns true if the raycast was successfully executed; also returns the hit information and whether it has collided with anything on the COLLISION_WEAPON channel, trace direction and information about the player character (where it's looking towards)
	bool ExecuteRaycast(bool& BlockingHit, FHitResult& Hit, FVector& EyeLocation, FRotator& EyeRotation, FVector& TraceEnd);	

	// Function that determine the damage and special effects that will effect the hit component (only doing damage rn)
	virtual float CalculateHitEffect(const EPhysicalSurface& HitSurfaceType);

	// Function that triggers the emitting of the particle effects and sound effects on raycast hit
	virtual void PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector& HitLocation, const FRotator& HitRotation);

	// Emit the tracer effect with the muzzle socket name as source location, then set target location via parameter setting
	virtual void PlayTraceEffect(const FVector& ShotTraceEnd);
	
};
