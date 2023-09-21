// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Types/Shooting/SRaycastWeapon.h"
#include "CoopGame/CoopGame.h"
#include "Gameplay/Characters/SPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"



// Sets default values for this actor's properties
ASRaycastWeapon::ASRaycastWeapon() {
	
	TracerTargetName = FName("BeamEnd");

}


// Execute the pressing action of this weapon's secondary action
void ASRaycastWeapon::OnSecondaryWeaponActionPressed() {
	
	Super::OnSecondaryWeaponActionPressed();

	// Zooming in is done in the player class so we cast it then set the variable accordingly
	if (IsActive) {
		
		if (WeaponOwner) {

			WeaponOwner->bWantsToZoomIn = true;
		
		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Secondary weapon action can't be performed, weapon inactive"));
		
	}
}


// Execute the releasing action of this weapon's secondary action
void ASRaycastWeapon::OnSecondaryWeaponActionReleased() {

	Super::OnSecondaryWeaponActionReleased();

	// Zooming out is done in the player class so we cast it then set the variable accordingly
	if (WeaponOwner) {

		WeaponOwner->bWantsToZoomIn = false;
		
	}
	
}


// Implements the cancelling of actions common to all raycast weapons
void ASRaycastWeapon::CancelOngoingActions() {

	// reload request cancel and primary action termination is already handled in the parent class
	Super::CancelOngoingActions();

	// Simulate weapon action release
	OnSecondaryWeaponActionReleased();
}


// Implements the logic specific to this subclass of shooting weapon; in this case, it implements the raycast hit detection
// the overload allows us to trace more than one ray at a time
bool ASRaycastWeapon::HandleSpecificFiring(const uint8& BulletsConsumed) {

	bool BlockingHit = false;
	FHitResult Hit;
	FVector EyeLocation;
	FRotator EyeRotation;
	FVector TraceEnd;

	// todo LOW PRIORITY: implement single shot multiple bullet scattering		
	bool Success = ExecuteRaycast(BlockingHit,Hit, EyeLocation, EyeRotation, TraceEnd);
		
	// Default value for the smoke trail
	FVector TracerParticleEnd = TraceEnd;

	// If the ray cast was blocked by any object, update the tracer particle target and apply damage 
	if (BlockingHit) {

		// Apply damage to actor
		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		AActor* HitActor = Hit.GetActor();
		UGameplayStatics::ApplyPointDamage(HitActor, CalculateHitEffect(SurfaceType), EyeRotation.Vector(), Hit, GetInstigatorController(), this, DamageType);

		// Determine impact effect
		TracerParticleEnd = Hit.ImpactPoint;

		// Play impact VFX
		PlayImpactEffects(SurfaceType, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			
	}
			
	// Play specific VFX, sounds, etc
	PlayTraceEffect(TracerParticleEnd);	

	return Success;
	
}


// Casts a single ray in the direction the player character is looking towards and checks if it is blocked by anything
// Returns true if the raycast has collided with anything on the COLLISION_WEAPON channel; also returns the hit information, trace direction and information about the player character (where it's looking towards)
bool ASRaycastWeapon::ExecuteRaycast(bool& BlockingHit, FHitResult& Hit, FVector& EyeLocation, FRotator& EyeRotation, FVector& TraceEnd) {

	bool Success = false;
	
	// Get the point of view from the actor and determine the location where the player character is looking at
	if (WeaponOwner) {

		// Calculate the direction of the trace
		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000.0f);
		
		// Disable the line trace from detecting the weapon itself and its owner, get exact result of collision against a mesh
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(WeaponOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// Check if the ray cast has been blocked by any object and get the hit information
		BlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams);

		Success = true;
		
	}

	return Success;
	
}


// Function that determine the damage and special effects that will effect the hit component (only doing damage rn)
float ASRaycastWeapon::CalculateHitEffect(const EPhysicalSurface& HitSurfaceType) {

	float FinalDamage = 0.0f;
	
	switch (HitSurfaceType) {
				
	case SURFACE_FLESHDEFAULT:
		FinalDamage = ShotDamage * 1.0f;
		break;
				
	case SURFACE_FLESHVULNERABLE:
		FinalDamage = ShotDamage * 2.0f;
		break;
				
	default:
		FinalDamage = ShotDamage * 0.0f;
		break;
				
	}

	return FinalDamage;
	
}


// Function that triggers the emitting of the particle effects and sound effects on raycast hit
void ASRaycastWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector& HitLocation, const FRotator& HitRotation) {

	// Determine impact effect to play and play it
	UParticleSystem* ImpactToPlay = DefaultImpactEffect;
	switch (SurfaceType) {
				
	case SURFACE_FLESHDEFAULT:
		ImpactToPlay = FleshImpactEffect;
		break;
				
	case SURFACE_FLESHVULNERABLE:
		ImpactToPlay = FleshImpactEffect;
		break;
				
	default:
		ImpactToPlay = DefaultImpactEffect;
		break;
				
	}
	
	// Emit particle effect on hit
	if (ImpactToPlay) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactToPlay, HitLocation, HitRotation);
				
	}
	
}


// Emit the tracer effect with the muzzle socket name as source location, then set target location via parameter setting
void ASRaycastWeapon::PlayTraceEffect(const FVector& ShotTraceEnd) {

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	
	if (TracerEffect) {
			
		UParticleSystemComponent* TracerParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerParticle) {

			TracerParticle->SetVectorParameter(TracerTargetName, ShotTraceEnd);
				
		}
		    
	}
	
}