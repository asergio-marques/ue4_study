// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Helpers/DamagingActor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Gameplay/Characters/TargetDummy.h"
#include "Gameplay/Characters/SPlayerCharacter.h"
#include "CoopGame/CoopGame.h"




// Sets default values
ADamagingActor::ADamagingActor() {

	// Initialize mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionProfileName("Projectile");
	MeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	RootComponent = MeshComp;
	
	// Initialize projectile movement component
	ProjMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjMoveComp"));
	ProjMoveComp->UpdatedComponent = MeshComp;
	ProjMoveComp->bRotationFollowsVelocity = true;
	ProjMoveComp->bShouldBounce = true;

	// Configure speeds (must be done here at construction!)
	ProjMoveComp->InitialSpeed = 3000.0f;
	ProjMoveComp->MaxSpeed = ProjMoveComp->InitialSpeed;
	
	// Set base parameters of grenade (overriden at BeginPlay)
	ExplosionRadius = 500.0f;
	GrenadeLifetime = 3.0f;
	Bounciness = 0.5f;
	
	ExplosionParticleScale = FVector(10.0f);
	BaseDamage = 100.0f;
	
}


// Allows the ThrowingWeapon to set the grenade's damage
void ADamagingActor::SetBaseDamage(float newShotDamage) {
	
	BaseDamage = newShotDamage;	
	
}


// Allows the ThrowingWeapon to set the grenade's damage type
void ADamagingActor::SetDamageType(TSubclassOf<UDamageType> newDamageType) {

	DamageType = newDamageType;
	
}


// Called when the game starts or when spawned
void ADamagingActor::BeginPlay() {
	
	Super::BeginPlay();

	MeshComp->OnComponentHit.AddDynamic(this, &ADamagingActor::OnHit);
	GetWorldTimerManager().SetTimer(TimerHandle_ExplosionFuse, this, &ADamagingActor::Explode, GrenadeLifetime);

	ProjMoveComp->Bounciness = Bounciness;
	
}


// Called when this damaging actor hits another surface, used to check if the hit component is of a type that should trigger an explosion
void ADamagingActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp
	, FVector NormalImpulse, const FHitResult& Hit) {

	ATargetDummy* DummyActor = Cast<ATargetDummy>(OtherActor);
	ASPlayerCharacter* PlayerActor = Cast<ASPlayerCharacter>(OtherActor); 

	// todo implement friendly fire configuration for weapon
	// Explode only if the hit actor is a target dummy or a player character
	if (DummyActor || PlayerActor) {

		Explode();
		
	}
	
}


// Function to be called whenever the grenade explodes
void ADamagingActor::Explode() {

	IConsoleVariable* debugDrawVariable =
		IConsoleManager::Get().FindConsoleVariable(TEXT("DebugWeaponDrawing"));

	if (debugDrawVariable && debugDrawVariable->GetInt() > 0) {

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 0);
		
	}
	
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), ExplosionRadius, DamageType, TArray<AActor*>(), this, GetOwner()->GetInstigatorController(), true, COLLISION_WEAPON);

	PlayExplosionEffects();
	
	this->Destroy();
	
}


// Function that triggers the emitting of the particle effects and sound effects on explosion
void ADamagingActor::PlayExplosionEffects() {

	if (ExplosionParticle) {

		UParticleSystemComponent* ExplosionVFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation());
		ExplosionVFX->SetWorldScale3D(ExplosionParticleScale);
		
	}

}