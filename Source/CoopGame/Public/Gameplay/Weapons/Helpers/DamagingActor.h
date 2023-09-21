// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DamagingActor.generated.h"



class UStaticMeshComponent;
class UProjectileMovementComponent;


/*
 *
 * This class represents an actor that can be projected from a ThrowingWeapon, which explodes upon contact with another player character/damage dummy or after a given configurable time is set
 * 
 */
UCLASS()
class COOPGAME_API ADamagingActor : public AActor {
	
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ADamagingActor();

	// Allows the ThrowingWeapon to set the grenade's damage
	FORCEINLINE void SetBaseDamage(float newShotDamage);

	// Allows the ThrowingWeapon to set the grenade's damage type
	FORCEINLINE void SetDamageType(TSubclassOf<UDamageType> newDamageType);

	
protected:

	// Mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// Dictates movement rules for this actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjMoveComp;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when this damaging actor hits another surface, used to check if the hit component is of a type that should trigger an explosion
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Function to be called whenever the grenade explodes
	void Explode();
	
	// Function that triggers the emitting of the particle effects and sound effects on explosion 
	virtual void PlayExplosionEffects();

	// Radius of the grenade's explosion (in Unreal units)
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 100.0, ClampMax = 1000.0))
	float ExplosionRadius;
	
	// Total lifespan of damaging actor (in seconds)
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 1.0, ClampMax = 5.0))
	float GrenadeLifetime;
	
	// Percentage of velocity maintained after this projectile hits a surface
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 0.0, ClampMax = 1.0))
	float Bounciness;
	
	// Particle effect to be emitted when grenade explodes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* ExplosionParticle;

	// Scale vector to be applied to the ExplosionParticle system when grenade explodes (it is recommended this vector is uniform)
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FVector ExplosionParticleScale;

	// Base damage dealt by the grenade, assigned from the ThrowingWeapon which spawns this actor
	float BaseDamage;
	
	// Damage type dealt by the grenade, assigned from the ThrowingWeapon which spawns this actor
	TSubclassOf<UDamageType> DamageType;


private:
	// Member timer for the damaging actor's lifetime before exploding
	FTimerHandle TimerHandle_ExplosionFuse;
};