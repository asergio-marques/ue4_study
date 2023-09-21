// Fill out your copyright notice in the Description page of Project Settings.




#include "Gameplay/EnvHazards/SBoomBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Gameplay/Characters/Components/SAttributesComponent.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"



// Sets default values
ASBoomBarrel::ASBoomBarrel() {

	// Create and setup components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->bCastHiddenShadow = false;
	RootComponent = MeshComp;

	// Create radial force component
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;

	// Create attributes component
	StatsComp = CreateDefaultSubobject<USAttributesComponent>(TEXT("StatsComp"));

	bIsAlive = true;

	// Set base parameters of barrel (overridden at BeginPlay)
	ExplosionRadius = 500.0f;
	ExplosionParticleScale = FVector(10.0f);
	BaseDamage = 100.0f;
	BaseKnockback = 50000.0f;

}

// Called when the game starts or when spawned
void ASBoomBarrel::BeginPlay() {

	Super::BeginPlay();
	
	if (StatsComp) {

		StatsComp->HPChangedDelegate.AddDynamic(this, &ASBoomBarrel::OnHPChanged);

	}

	if (RadialForceComp) {

		RadialForceComp->Radius = ExplosionRadius;

	}

}


// Called when a health change has been detected by the attribute component
void ASBoomBarrel::OnHPChanged(USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP
	, const UDamageType* IncomingDamageType, AController* InstigatedBy, AActor* DamageCauser) {

	// if the character is still alive and the current HP reaches zero, kill it!
	if (bIsAlive && (CurrentHP <= 0.0f)) {

		bIsAlive = false;
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

		SetLifeSpan(10.0f);

		Explode(InstigatedBy);

	}

}


// Function to be called whenever the barrel explodes
void ASBoomBarrel::Explode(AController* InstigatedBy) {

	IConsoleVariable* debugDrawVariable =
		IConsoleManager::Get().FindConsoleVariable(TEXT("COOP.DebugWeapons"));

	if (debugDrawVariable && debugDrawVariable->GetInt() > 0) {

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 0);

	}

	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), ExplosionRadius, DamageType, TArray<AActor*>(), this, InstigatedBy, true, COLLISION_WEAPON);
	
	if (RadialForceComp){

		RadialForceComp->FireImpulse();

	}

	PlayExplosionEffects();

}


// Function that triggers the emitting of the particle effects and sound effects on explosion
void ASBoomBarrel::PlayExplosionEffects() {

	if (ExplosionParticle) {

		UParticleSystemComponent* ExplosionVFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation());
		ExplosionVFX->SetWorldScale3D(ExplosionParticleScale);

	}

}