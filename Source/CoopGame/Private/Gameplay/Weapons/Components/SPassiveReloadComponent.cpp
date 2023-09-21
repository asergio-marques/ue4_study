// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Components/SPassiveReloadComponent.h"



// Sets default values for this component's properties
USPassiveReloadComponent::USPassiveReloadComponent() {

	

}


// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USPassiveReloadComponent::HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType) {

	bool Success = false;

	if (CanTriggerReload(CancelType)) {

		// Immediately set up the timer afterwards
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitialDelayTimer, this, &USPassiveReloadComponent::OnInitialDelayFinished, InitialDelay);

		// Update reload status and update HUDInfo via broadcast
		ReloadCompletedDelegate.Broadcast(CancelType);
		ReloadStartedDelegate.Broadcast(InitialDelay);
		bIsReloading = true;

		UE_LOG(LogTemp, Log, TEXT("Set passive reload timers, with delay of %f seconds and per bullet tick of %f seconds"), InitialDelay, ReloadCycleDuration);

		Success = true;


	}

	UE_LOG(LogTemp, Log, TEXT("Weapon passive reload trigger %u"), Success);

	return Success;

}


// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USPassiveReloadComponent::HandleReloadCancel(const UReloadCancelTrigger& CancelType) {

	bool Success = false;

	if (CanCancelReload(CancelType)) {

		// Reset IsReloading and retrigger protection flags, clear timer
		bIsReloading = false;
		bReloadIsBeingCancelled = false;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);

		// Update HUD info via broadcast
		ReloadCompletedDelegate.Broadcast(CancelType);

		Success = true;

	}

	// if the passive reload cancel operation was successful and the weapon has not been switched out, we want to trigger the reload anew immediately, so we can do it here
	bool CanRetriggerImmediately = ((CancelType != UReloadCancelTrigger::TriggerByWeaponSwitch) && (CancelType != UReloadCancelTrigger::TriggerByLimitReached));

	if (!bIsReloading && CanRetriggerImmediately) {

		HandleReloadTrigger(false);

	}

	// no need to update HUD info

	UE_LOG(LogTemp, Log, TEXT("Weapon passive reload cancel %u"), Success);

	return Success;

}


// Activate passive reload automatically
void USPassiveReloadComponent::ActivateAutomaticProperties(void) {

	HandleReloadTrigger(false, UReloadCancelTrigger::GenericPassiveTrigger);

}


// Status extracting functions (getters that depend on reload type)

// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
int32 USPassiveReloadComponent::GetMaximumBullets(void) const {

	return MaximumCapacity;

}


// Expose the reload type
UReloadType USPassiveReloadComponent::GetReloadType(void) const {

	return UReloadType::PassiveReload;

}


// Called when the game starts
void USPassiveReloadComponent::BeginPlay() {

	Super::BeginPlay();

}


// Returns the number of leftover bullets/magazines depending on the active reload type, passive is always -1
int32 USPassiveReloadComponent::GetAvailableReloads(void) const {

	return -1;

}


// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
bool USPassiveReloadComponent::CanFire(bool CanBePartial, uint8 NumToExpend) const {

	// If we can fire only part of the RequiredBullets, we check only if we have 1 or more bullets
	// Otherwise, we check if we can fire all required bullets
	return (CanBePartial && (BulletsCurrentlyActive > 0)) || (BulletsCurrentlyActive >= NumToExpend);

}


// Returns true if the request performed and the weapon status and configuration is proper for triggering the reload process
bool USPassiveReloadComponent::CanTriggerReload(UReloadCancelTrigger CancelType) const {

	// Bullets currently active are less than the maximum bullet capacity (there is space in weapon for more bullets)
	bool SpaceAvailableInWeapon = (BulletsCurrentlyActive < MaximumCapacity);

	bool ReloadKeyPressed = (CancelType == UReloadCancelTrigger::TriggerByReload);

	UE_LOG(LogTemp, Log, TEXT("Passive Reload Trigger Validation, SpaceAvailableInWeapon %u, ReloadKeyPressed %u"), SpaceAvailableInWeapon, ReloadKeyPressed);

	// If there is space in the weapon and the reload key is not requesting passive reload, we can trigger passive reload
	return (!bIsReloading && SpaceAvailableInWeapon && !ReloadKeyPressed);

}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of reload process
bool USPassiveReloadComponent::CanCancelReload(UReloadCancelTrigger CancelType) const {

	bool WeaponActionTriggered = (CancelType == UReloadCancelTrigger::TriggerByWeaponAction) && (BulletsCurrentlyActive > 0);

	// TODO: investigate this further
	// needed to prevent some multithreading fuckery going on with WeaponActionTriggered
	bool temp = WeaponActionTriggered;

	bool LimitReached = (CancelType == UReloadCancelTrigger::TriggerByLimitReached);
	bool WeaponSwitched = (CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch);
	bool OutsideFactors = (CancelType == UReloadCancelTrigger::TriggerByExternal);

	UE_LOG(LogTemp, Log, TEXT("Passive Reload Cancel Validation, bIsReloading %u, WeaponActionTriggered %u, LimitReached %u, WeaponSwitched %u, OutsideFactors %u"), bIsReloading, WeaponActionTriggered, LimitReached, WeaponSwitched, OutsideFactors);

	// passive weapon reload can only be cancelled indirectly (limit reached, weapon switch, others) or by a weapon action
	// no need to check if reloading is happening; cancelling may be done anyways as cancelling has no nefarious side effects
	return (bIsReloading && (WeaponActionTriggered || LimitReached || WeaponSwitched || OutsideFactors));


}


// Function called once the initial delay has elapsed
void USPassiveReloadComponent::OnInitialDelayFinished(void) {

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);

	// Set bullets-in retriggerable timer
	// Set looping timer for the introduction of ammo in the weapon
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USPassiveReloadComponent::OnBulletsIn, ReloadCycleDuration, true);

	// Update HUD Info via broadcast
	ReloadStartedDelegate.Broadcast(ReloadCycleDuration);

	UE_LOG(LogTemp, Log, TEXT("Cleared passive initial delay timer, started passive bullets-in looping timer with duration %f"), ReloadCycleDuration);

}


// Function called every time the reload cycle duration elapses
void USPassiveReloadComponent::OnBulletsIn(void) {

	// Increment one-by-one always
	BulletsCurrentlyActive += 1;

	// Update HUD Info via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	BulletsAddedDelegate.Broadcast(1);	/*this is always 1 */

	UE_LOG(LogTemp, Log, TEXT("Loaded 1 bullet, current capacity %u out of %u maximum"), BulletsCurrentlyActive, MaximumCapacity);

	// If we have reached the maximum capacity, we cancel the reload automatically
	if (BulletsCurrentlyActive == MaximumCapacity) {

		HandleReloadCancel(UReloadCancelTrigger::TriggerByLimitReached);

	}
	// if there is no reason to cancel the reload, broadcast the start of a new reload cycle
	else {

		ReloadStartedDelegate.Broadcast(ReloadCycleDuration);

	}

}