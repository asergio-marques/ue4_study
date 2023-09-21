// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Types/Shooting/SShootingWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Weapons/Components/SAmmoSystemComponent.h"
#include "Particles/ParticleSystemComponent.h"



// Sets default values for this actor's properties
ASShootingWeapon::ASShootingWeapon() {

	// Create and setup components
	AmmoSysComp = CreateDefaultSubobject<USAmmoSystemComponent>(TEXT("AmmoSysComp"));

	// General shooting weapon properties
	MuzzleEffectScale = FVector(1.0f);
	MuzzleSocketName = FName("default_muzzle_socket");
	BulletPerAttack = 1;
	bCanPartialFire = false;

	// Automatic shooting weapon properties
	RateOfFireAutomatic = 10.0f;
	
	// Manual shooting weapon properties
	TimeBetweenShotsManual = 2.0f;
	
	// Burst shooting weapon properties
	ShotsInBurst = 5.0f;
	RateOfFireBurst = 10.0f;
	TimeBetweenBursts = 1.0f;

	// Others
	bReloadTriggeredByButtonPress = true;
	bReloadBlockedByWeapon = false;
	bBlockFiring = false;
	CurrentBurstCount = 0;
	
}

// Execute the pressing action of this weapon's primary action
void ASShootingWeapon::OnPrimaryWeaponActionPressed() {

	Super::OnPrimaryWeaponActionPressed();
	
	if (IsActive) {
		
		// Determine if it's too soon to fire this weapon again and calculate the delay necessary so the RateOfFire isn't changed
		// If the result is negative, then we have no delay
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenFires - GetWorld()->TimeSeconds, 0.0f);
	
		switch (WeaponType) {
		
		case UShootingWeaponType::AutomaticFire:
			StartFireAutomatic(FirstDelay);
			break;
		
		case UShootingWeaponType::ManualFire:
			StartFireManual();
			break;
		
		case UShootingWeaponType::BurstFire:
			StartFireBurst(FirstDelay);
			break;
		
		default:
			UE_LOG(LogTemp, Error, TEXT("Invalid mode selected!"));
			break;

		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Primary weapon action can't be performed, weapon inactive"));
		
	}

}


// Execute the releasing action of this weapon's primary action
void ASShootingWeapon::OnPrimaryWeaponActionReleased() {

	Super::OnPrimaryWeaponActionReleased();

	switch (WeaponType) {
		
	case UShootingWeaponType::AutomaticFire:
		// Clear timer for automatic fire and weapon "busy" status
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
		bReloadBlockedByWeapon = false;
		break;
		
	case UShootingWeaponType::ManualFire:
		// Clear weapon "busy" status
		bReloadBlockedByWeapon = false;
		break;
		
	case UShootingWeaponType::BurstFire:
		// Clear timer for automatic fire and weapon "busy" status
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
		bReloadBlockedByWeapon = false;
		break;
		
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid mode selected!"));
		break;

	}
	
}


// Check if reload is possible and call the AmmoSystemComponent reload function
void ASShootingWeapon::OnReloadPressed() {
	
	Super::OnReloadPressed();

	// Reload only possible if the weapon is not being fired
	if (AmmoSysComp) {
		
		bool ReloadTriggerSuccess = AmmoSysComp->HandleReloadTrigger(bReloadBlockedByWeapon, UReloadCancelTrigger::TriggerByReload);

		// If the reload trigger was not successful (possibly because there was no on-going reload), then we cancel any on-going reload
		if (!ReloadTriggerSuccess) {

			AmmoSysComp->HandleReloadCancel(UReloadCancelTrigger::TriggerByReload);
			
		}
		
	}
	
}


// Implements the triggering of effects always active in shooting weapons
void ASShootingWeapon::TriggerAutomaticActions() {
	
	Super::TriggerAutomaticActions();

	// Trigger automatic properties of the ammo system
	if (AmmoSysComp) {

		AmmoSysComp->ActivateAutomaticProperties();
		
	}
	
}


// Requests broadcast of information from the subcomponents after this weapon's activation has been confirmed to be successful and broadcast
void ASShootingWeapon::RequestSubcomponentBroadcast() {
	
	Super::RequestSubcomponentBroadcast();

	if (AmmoSysComp) {

		AmmoSysComp->OnBroadcastRequested();
		
	}
	
}


// Returns the reload type of this weapon (NoReload by default, should be overwritten by child classes if they have a reload system)
UReloadType ASShootingWeapon::GetReloadType() const {

	if (AmmoSysComp) {

		return AmmoSysComp->GetReloadType();
		
	}
	else {
		
		return UReloadType::NoReload;
		
	}
	
}


// Called when the game starts or when spawned
void ASShootingWeapon::BeginPlay() {

	Super::BeginPlay();

	// Calculate time between fires off the ROF
	switch (WeaponType) {
	case UShootingWeaponType::AutomaticFire:
		TimeBetweenFires = 1 / RateOfFireAutomatic;
		break;
		
	case UShootingWeaponType::ManualFire:
		TimeBetweenFires = 1E7;			// Large number intentionally, TimeBetweenFires isn't used in manual weapons
		break;
		
	case UShootingWeaponType::BurstFire:
		TimeBetweenFires = 1 / RateOfFireBurst;
		break;
		
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid mode selected!"));
		break;
		
	}

	// First set on LastFireTime (TimeBetweenFires subtracted so firing is possible immediately)
	LastFireTime = GetWorld()->TimeSeconds - TimeBetweenFires;
	
}


// Implements the cancelling of actions common to all shooting weapons
void ASShootingWeapon::CancelOngoingActions() {
	
	Super::CancelOngoingActions();

	if (AmmoSysComp) {
		
		// Cancel current reload (consider weapon busy status as false because we want to cancel ALWAYS)
		AmmoSysComp->HandleReloadCancel(UReloadCancelTrigger::TriggerByWeaponSwitch);
		
	}

	// Simulate weapon action release
	OnPrimaryWeaponActionReleased();
	
}


// Polymorphic function that handles the specifics of the weapon being fired (e.g: raycast vs actor spawning)
bool ASShootingWeapon::HandleSpecificFiring(const uint8& BulletsConsumed) {

	UE_LOG(LogTemp, Log, TEXT("Gun banged %u times"), BulletsConsumed);
	
	return false;
}


// Function dedicated to handling automatic fire
void ASShootingWeapon::StartFireAutomatic(float FirstDelay) {

	// Start the auto-repeating timer for automatic fire
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASShootingWeapon::FireWeapon, TimeBetweenFires, true, FirstDelay);
	
}


// Function dedicated to handling manual fire with in-built delay and forced button pressing
void ASShootingWeapon::StartFireManual() {

	// Fire the weapon instantly and only once, then block firing immediately
	FireWeapon();
	bBlockFiring = true;

	// Check if the timer is already running; if not, start the timer for clearing the block
	// As the function here is only used here, there is no problem with this design; bBlockFiring will be set to true just before the timer is started, and as this is the only place where this variable is set to false, FireWeapon will be blocked successfully
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_TimeBetweenShots)) {
			
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASShootingWeapon::ResetFiringBlock, TimeBetweenShotsManual);
		
	}
	
}


// Function dedicated to handling burst fire (short bursts of automatic fire with in-built delay between them)
void ASShootingWeapon::StartFireBurst(float FirstDelay) {

	// Start the auto-repeating timer for automatic fire
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASShootingWeapon::FireWeapon, TimeBetweenFires, true, FirstDelay);
	
}


// While the primary weapon action button is pressed, this function will be called continuously
void ASShootingWeapon::FireWeapon() {

	uint8 BulletsConsumed = 0;
	
	if (!bBlockFiring && AmmoSysComp && AmmoSysComp->ManageAmmoWhenUseRequested(bCanPartialFire, BulletPerAttack, BulletsConsumed)) {
			
		// Set the weapon state to "busy"
		bReloadBlockedByWeapon = true;

		// Polymorphic!
		bool FiringSuccess = HandleSpecificFiring(BulletsConsumed);
			
		ShakePlayerCamera();
		
		LastFireTime = GetWorld()->TimeSeconds;
		
		PlayMuzzleEffect();

		if (FiringSuccess && WeaponType == UShootingWeaponType::BurstFire) {

			++CurrentBurstCount;
			
		}
		// If the burst limit of bullets has been reached, then block firing and start timer to reset firing block
		// We can already reset the burst count back to 0 because the firing block is in place already
		if (CurrentBurstCount >= ShotsInBurst) {
		
			bBlockFiring = true;
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenBursts, this, &ASShootingWeapon::ResetFiringBlock, TimeBetweenBursts);
			CurrentBurstCount = 0;
		
		}
		
	}
	// In the case the weapon/player settings are configured to be triggerable via weapon action, trigger reload
	else if (bReloadTriggeredByButtonPress) {
		
		// Reset weapon busy state (no longer firing although button still being pressed) and try to reload the weapon
		bReloadBlockedByWeapon = false;
		TriggerReloadViaWeaponAction();
			
	}
	else if (bBlockFiring) {

		// do nothing
		UE_LOG(LogTemp, Log, TEXT("Weapon fire is blocked by the weapon itself"));
		
	}
	
}


// Resets the block for manual and burst firing
void ASShootingWeapon::ResetFiringBlock() {

	bBlockFiring = false;

	UE_LOG(LogTemp, Log, TEXT("Reset firing block"));
	
}


// Triggers a reload via a Weapon action, always
void ASShootingWeapon::TriggerReloadViaWeaponAction() {

	UE_LOG(LogTemp, Log, TEXT("Triggering reload via weapon action"));
	
	// Reload only possible if the weapon is not being fired
	if (AmmoSysComp) {

		bool ReloadTriggerSuccess = AmmoSysComp->HandleReloadTrigger(bReloadBlockedByWeapon);

		// If the reload trigger was not successful (possibly because there was no on-going reload), then we cancel any on-going reload
		if (!ReloadTriggerSuccess) {

			AmmoSysComp->HandleReloadCancel(UReloadCancelTrigger::TriggerByWeaponAction);
			
		}
		
	}
	
}


// Emit the muzzle effect (happens in all shooting weapons)
void ASShootingWeapon::PlayMuzzleEffect() {

	if (MuzzleEffect) {
			
		UParticleSystemComponent* MuzzleVFX = UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		MuzzleVFX->SetWorldScale3D(MuzzleEffectScale);
			
	}
	
}
