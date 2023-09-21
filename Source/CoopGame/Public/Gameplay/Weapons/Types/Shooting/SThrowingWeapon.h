// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SShootingWeapon.h"
#include "SThrowingWeapon.generated.h"



class ADamagingActor;


/**
 *
 * This class implements the specifics of shooting weapons that use projectiles rather than raycast to deal damage.
 * For this purpose, it only overrides the HandleSpecificFiring function declared in its parent class ASShootingWeapon to spawn a configurable actor that deals damage.
 * 
 */
UCLASS()
class COOPGAME_API ASThrowingWeapon : public ASShootingWeapon {
	
	GENERATED_BODY()

	
protected:
	// Implements the logic specific to this subclass of shooting weapon; in this case, it implements projectiles sent flying at a given speed which themselves deal the damage
	// the overload allows us to eject more than one projectile
	virtual bool HandleSpecificFiring(const uint8& BulletsConsumed) override;

	// Category of grenade to be used
	UPROPERTY(EditDefaultsOnly, Category = "Damage | Damaging Actor")
	TSubclassOf<ADamagingActor> GrenadeClass;
	
};