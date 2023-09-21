// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Types/Shooting/SThrowingWeapon.h"
#include "Gameplay/Weapons/Helpers/DamagingActor.h"
#include "Gameplay/Characters/SPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"



// Implements the logic specific to this subclass of shooting weapon; in this case, it implements projectiles sent flying at a given speed which themselves deal the damage
// the overload allows us to eject more than one projectile
bool ASThrowingWeapon::HandleSpecificFiring(const uint8& BulletsConsumed) {

	bool Success = false;
	
	// Trace the world from pawn camera to crosshair location
	if (WeaponOwner) {
		
		// Get the point of view from the actor and determine the location where the player character is looking at
		FVector EyeLocation;
		FRotator EyeRotation;
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Get the location of the muzzle
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
		FVector SpawnLocation = MuzzleLocation + (MuzzleRotation.Vector() * 50.0f);

		// Spawn damage-dealing actor at location of muzzle and set damage and owner params
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Instigator = WeaponOwner;
		ADamagingActor* Grenade = GetWorld()->SpawnActor<ADamagingActor>(GrenadeClass, SpawnLocation, EyeRotation, SpawnParameters);

		// Set grenade parameters if it has been spawned correctly
		if (Grenade) {
			
			Grenade->SetOwner(this->GetOwner());
			Grenade->SetBaseDamage(ShotDamage);
			Grenade->SetDamageType(DamageType);

			// If grenade was successfully spawned, set success to true
			Success = true;
			
		}
		
	}

	return Success;
	
}
