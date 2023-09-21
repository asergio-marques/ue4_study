// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBoomBarrel.generated.h"



class AController;
class UStaticMeshComponent;
class URadialForceComponent;
class USAttributesComponent;



/*
 *	TODO: documentation
 */
UCLASS()
class COOPGAME_API ASBoomBarrel : public AActor {

	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ASBoomBarrel();

	// Mesh of the barrel
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// Component that handles damage input before exploding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributesComponent* StatsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when a health change has been detected by the attribute component
	UFUNCTION()
	virtual void OnHPChanged(USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP, const UDamageType* IncomingDamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Variable that signalizes if the barrel is alive (can explode)
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool bIsAlive;

	// Base damage dealt by the barrel
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 0.0, ClampMax = 250.0))
	float BaseDamage;

	// Base knockback acceleration dealt by the barrel
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 100.0, ClampMax = 100000.0))
	float BaseKnockback;

	// Damage type dealt by the barrel
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<UDamageType> DamageType;

	// Radius of the grenade's explosion (in Unreal units)
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (ClampMin = 100.0, ClampMax = 1000.0))
	float ExplosionRadius;

	// Particle effect to be emitted when grenade explodes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UParticleSystem* ExplosionParticle;

	// Scale vector to be applied to the ExplosionParticle system when grenade explodes (it is recommended this vector is uniform)
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FVector ExplosionParticleScale;




private:
	// Function to be called whenever the barrel explodes
	void Explode(AController* InstigatedBy);

	// Function that triggers the emitting of the particle effects and sound effects on explosion
	void PlayExplosionEffects();

};
