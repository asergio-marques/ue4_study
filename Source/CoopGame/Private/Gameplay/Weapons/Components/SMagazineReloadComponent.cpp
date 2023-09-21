// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Components/SMagazineReloadComponent.h"



// Sets default values for this component's properties
USMagazineReloadComponent::USMagazineReloadComponent() :
	UISAmmoSystemComponent() {

	MagazinesExpended = 0;

}


// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USMagazineReloadComponent::HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType) {

	bool Success = false;

	if (CanTriggerReload(OtherWeaponActionOccurring)) {

		// Set timers for completion of the reload phases
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USMagazineReloadComponent::OnBulletsIn, BulletsInTime);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReloadCompletion, this, &USMagazineReloadComponent::OnReloadComplete, (BulletsInTime + WinddownDelay));

		// Update reload status and update HUDInfo, if bEnableMagazineReloadCancel then this action can be cancelled
		bIsReloading = true;

		UE_LOG(LogTemp, Log, TEXT("Set magazine reload timers, bIsReloading %u"), bIsReloading);

		Success = true;

		ReloadStartedDelegate.Broadcast(BulletsInTime);

	}

	UE_LOG(LogTemp, Log, TEXT("Weapon magazine reload trigger %u"), Success);

	return Success;

}


// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USMagazineReloadComponent::HandleReloadCancel(const UReloadCancelTrigger& CancelType) {

	bool Success = false;

	if (CanCancelReload(CancelType)) {

		// Make weapon available for other actions
		bIsReloading = false;

		// Magazine Reload cancel is instantaneous so we don't need to set bReloadIsBeingCancelled

		// Clear timers
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

		// update HUD info via broadcast
		ReloadCompletedDelegate.Broadcast(CancelType);

		Success = true;

	}

	UE_LOG(LogTemp, Log, TEXT("Weapon magazine reload cancel %u"), Success);

	return Success;

}


// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
int32 USMagazineReloadComponent::GetMaximumBullets(void) const {

	return BulletPerMagazine;

}


// Expose the reload type
UReloadType USMagazineReloadComponent::GetReloadType(void) const {

	return UReloadType::ActiveMagazineReload;

}

void USMagazineReloadComponent::BeginPlay() {

	Super::BeginPlay();

	// Set status variables to their defaults
	MagazinesExpended = 1;

	// no need to broadcast changes of ammo and whatnot here because that initial set up can be done once the weapon to which this ammo system belongs is activated

}


// Returns the number of leftover bullets/magazines depending on the active reload type; if the reload type is passive, this function returns 0
int32 USMagazineReloadComponent::GetAvailableReloads(void) const {

	if (bCapMagazinesPerLife) return (MagazinesPerLife - MagazinesExpended);
	else return -1;

}


// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
bool USMagazineReloadComponent::CanFire(bool CanBePartial, uint8 NumToExpend) const {

	// If we can fire only part of the RequiredBullets, we check only if we have 1 or more bullets
	// Otherwise, we check if we can fire all required bullets
	bool EnoughBulletsAvailable = (CanBePartial && (BulletsCurrentlyActive > 0)) || (BulletsCurrentlyActive >= NumToExpend);

	return (EnoughBulletsAvailable && !bIsReloading);

}


// Returns true if the request performed and the weapon status and configuration is proper for triggering the reload process
bool USMagazineReloadComponent::CanTriggerReload(bool OtherWeaponActionOccurring) const {

	// There is no maximum on magazines per life OR the magazines expended in this life are less than the maximum magazines available per life (there are magazines in storage)
	bool MagazinesAvailableForUse = (!bCapMagazinesPerLife || (MagazinesExpended < MagazinesPerLife));

	// If both the bullet timer and the completion timer are set, we do not execute magazine reload (just-in-case check, as this should never be reached since bIsReloading is only set to false once OnCompleteMagazineReload is called
	bool NewBulletTimerNotActive = (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_NewBulletsTimer));
	bool ReloadCompletionTimerNotActive = (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_ReloadCompletion));

	UE_LOG(LogTemp, Log, TEXT("Magazine Reload Trigger Validation, bIsReloading %u, OtherWeaponActionOccurring %u, MagazinesAvailableForUse %u, NewBulletTimerNotActive %u, ReloadCompletionTimerNotActive %u"), bIsReloading, OtherWeaponActionOccurring, MagazinesAvailableForUse, NewBulletTimerNotActive, ReloadCompletionTimerNotActive);

	// If there are magazines in storage AND no reload-blocking weapon action is happening, reload is possible	
	return (!bIsReloading && !OtherWeaponActionOccurring && MagazinesAvailableForUse && NewBulletTimerNotActive && ReloadCompletionTimerNotActive);

}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of reload process
bool USMagazineReloadComponent::CanCancelReload(UReloadCancelTrigger CancelType) const {

	// Weapon with magazine-type reload type is not reloading OR is reloading and is configured with cancellation properties
	bool ReloadKeyPressed = ((CancelType == UReloadCancelTrigger::TriggerByReload) && bEnableReloadCancel);
	bool WeaponSwitched = (CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch);
	bool OutsideFactors = (CancelType == UReloadCancelTrigger::TriggerByExternal);

	UE_LOG(LogTemp, Log, TEXT("Magazine Reload Cancel Validation, bIsReloading %u, ReloadKeyPressed %u, WeaponSwitched %u, OutsideFactors %u"), bIsReloading, ReloadKeyPressed, WeaponSwitched, OutsideFactors);

	// magazine weapon reload can only be cancelled indirectly (limit reached, weapon switch, others) or via pressing the reload key
	// no need to check if reloading is happening; cancelling may be done anyways as cancelling has no nefarious side effects
	return (bIsReloading && (ReloadKeyPressed || WeaponSwitched || OutsideFactors));

}


// Function called when the ammo is added to the currently available ammo
void USMagazineReloadComponent::OnBulletsIn() {

	// Decrement current available magazines, reset bullets available to maximum value
	++MagazinesExpended;
	BulletsCurrentlyActive = BulletPerMagazine;

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);

	// Update HUD Info via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	ReloadStoppedDelegate.Broadcast(WinddownDelay);

	UE_LOG(LogTemp, Log, TEXT("Cleared magazine bullet timer"));

}


// Function called when the animation for magazine reload is complete
void USMagazineReloadComponent::OnReloadComplete() {

	// Make weapon available for other actions
	bIsReloading = false;

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

	// Update HUD Info via broadcast
	ReloadCompletedDelegate.Broadcast(UReloadCancelTrigger::TriggerByLimitReached);

	UE_LOG(LogTemp, Log, TEXT("Cleared magazine reload timer, bIsReloading %u"), bIsReloading);

}