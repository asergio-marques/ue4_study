// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Components/SPerBulletReloadComponent.h"



// Sets default values for this component's properties
USPerBulletReloadComponent::USPerBulletReloadComponent() {

	BulletsExpended = 0;
	PerBulletReloadCycles = 0;

}


// Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USPerBulletReloadComponent::HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType) {

	bool Success = false;

	if (CanTriggerReload(OtherWeaponActionOccurring)) {

		// Set timer for the initial delay
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitialDelayTimer, this, &USPerBulletReloadComponent::OnInitialDelayFinished, InitialDelay);

		// Update reload status and update HUDInfo, reload cancelling always possible in Per-Bullet types
		bIsReloading = true;

		// Start "wind up" animation here too - see https://forums.unrealengine.com/t/elegant-solution-to-play-certain-animations-depending-on-what-weapon-a-player-is-using/27939/7 on how to handle

		UE_LOG(LogTemp, Log, TEXT("Set per-bullet initial delay timer with duration %f, bIsReloading %u"), InitialDelay, bIsReloading);

		Success = true;

		ReloadStartedDelegate.Broadcast(InitialDelay);

	}

	UE_LOG(LogTemp, Log, TEXT("Weapon per-bullet reload trigger %u"), Success);

	return Success;

}


// Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool USPerBulletReloadComponent::HandleReloadCancel(const UReloadCancelTrigger& CancelType) {

	bool Success = false;

	if (CanCancelReload(CancelType)) {

		// If the reload cancel is not caused directly by the player (cancel via weapon action or reload key) or not caused by limit reaching, then cancel must be instantaneous, also stopping retriggering of reload cancel
		bool bSkipCancelProcess = ((CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch) || (CancelType == UReloadCancelTrigger::TriggerByExternal));

		UE_LOG(LogTemp, Log, TEXT("Per-bullet reload status, bSkipCancelProcess %u, bReloadIsBeingCancelled %u"), bSkipCancelProcess, bReloadIsBeingCancelled);

		// todo LOW PRIORITY: perhaps this could be nerfed as otherwise, weapon switching will yield an advantage vs letting the reload time finish?
		if (bSkipCancelProcess) {

			// No need to set flag against retrigger because cancel is automatic

			// Clear timer for reloading and complete cancel instantly
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);
			OnReloadComplete(CancelType);

			UE_LOG(LogTemp, Log, TEXT("Per-bullet reload cancelled without delay"));

			Success = true;
		}
		else {

			if (PerBulletReloadCycles < 1) {

				// do nothing, intentional
				// we want to guarantee that section 2 of the reload is executed at least once before a reload cancel triggered by the weapon owner is accepted
				// this is to prevent constant trigger and cancel of reloading when holding the weapon attack button endlessly

				UE_LOG(LogTemp, Log, TEXT("Reload cancel request rejected, only %u reload cycles have been completed out of 1 minimum"), PerBulletReloadCycles);

			}
			else if (!bReloadIsBeingCancelled) {

				// Set protective flag against retrigger
				bReloadIsBeingCancelled = true;

				// Clear timer for reloading and set timer for "freeing" up this weapon
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);
				
				// overloaded functions need timer delegates
				FTimerDelegate delegate;
				delegate.BindUFunction(this, "OnReloadComplete", CancelType);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReloadCompletion, delegate, WinddownDelay, false);

				// update HUD via broadcast (timed cancel)
				ReloadStoppedDelegate.Broadcast(WinddownDelay);

				// start playing reload end animation
				UE_LOG(LogTemp, Log, TEXT("Per-bullet reload cancelled with delay"));

				Success = true;

			}

		}

	}

	UE_LOG(LogTemp, Log, TEXT("Weapon per-bullet reload cancel %u"), Success);

	return Success;

}


// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
int32 USPerBulletReloadComponent::GetMaximumBullets(void) const {

	return MaximumCapacity;

}


// Expose the reload type
UReloadType USPerBulletReloadComponent::GetReloadType(void) const {

	return UReloadType::ActivePerBulletReload;

}


// Called when the game starts
void USPerBulletReloadComponent::BeginPlay() {

	Super::BeginPlay();

	// Always the currently available bullets at start
	BulletsExpended = BulletsCurrentlyActive;

}


// Returns the number of leftover bullets/magazines depending on the active reload type; if the reload type is passive, this function returns 0
int32 USPerBulletReloadComponent::GetAvailableReloads(void) const {

	if (bCapBulletsPerLife) {
		
		// Guard against bad configuration in which MaximumBulletCapacity > BulletsPerLife (recall that at BeginGame, BulletsExpended = MaximumBulletCapacity
		return ((BulletsExpended > BulletsPerLife) ? 0 : (BulletsPerLife - BulletsExpended));

	}
	else {

		return -1;
		
	}

}


// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon.
bool USPerBulletReloadComponent::CanFire(bool CanBePartial, uint8 NumToExpend) const {

	// If we can fire only part of the RequiredBullets, we check only if we have 1 or more bullets
	// Otherwise, we check if we can fire all required bullets
	bool EnoughBulletsAvailable = (CanBePartial && (BulletsCurrentlyActive > 0)) || (BulletsCurrentlyActive >= NumToExpend);

	return (EnoughBulletsAvailable && !bIsReloading);

}


// Returns true if the request performed and the weapon status and configuration is proper for triggering the reload process
bool USPerBulletReloadComponent::CanTriggerReload(bool OtherWeaponActionOccurring) const {

	// Bullets currently active are less than the maximum bullet capacity (there is space in weapon for more bullets)
	bool SpaceAvailableInWeapon = (BulletsCurrentlyActive < MaximumCapacity);

	// There is no maximum on bullets per life OR the required bullets for a reload are equal or less than the bullets expended in this life (bullets available per life minus the bullets already loaded into the weapon)
	bool EnoughBulletsAvailableInStorage = (!bCapBulletsPerLife || (BulletsPerLife - BulletsExpended) >= BulletPerReload);

	// If the bullet timer is set, we do not execute per-bullet reload (just-in-case check, as this should never be reached since bIsReloading is only set to false once OnCompletePerBulletReload is called
	bool NewBulletTimerNotActive = !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_NewBulletsTimer);

	UE_LOG(LogTemp, Log, TEXT("Per-Bullet Reload Trigger Validation, bIsReloading %u, SpaceAvailableInWeapon %u, EnoughBulletsAvailableInStorage %u, NewBulletTimerNotActive %u"), bIsReloading, SpaceAvailableInWeapon, EnoughBulletsAvailableInStorage, NewBulletTimerNotActive);

	// If the above conditions are true AND no reload-blocking weapon action is happening, reload is possible
	return (!bIsReloading && !OtherWeaponActionOccurring && SpaceAvailableInWeapon && EnoughBulletsAvailableInStorage && NewBulletTimerNotActive);

}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of reload process
bool USPerBulletReloadComponent::CanCancelReload(UReloadCancelTrigger CancelType) const {

	// Weapon with magazine-type reload type is not reloading OR is reloading and is configured with cancellation properties
	bool WeaponActionTriggered = (CancelType == UReloadCancelTrigger::TriggerByWeaponAction);
	bool LimitReached = (CancelType == UReloadCancelTrigger::TriggerByLimitReached);
	bool WeaponSwitched = (CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch);
	bool OutsideFactors = (CancelType == UReloadCancelTrigger::TriggerByExternal);

	UE_LOG(LogTemp, Log, TEXT("Per-bullet Reload Cancel Validation, bIsReloading $u, WeaponActionTriggered %u, LimitReached %u, WeaponSwitched %u, OutsideFactors %u"), bIsReloading, WeaponActionTriggered, LimitReached, WeaponSwitched, OutsideFactors);

	// magazine weapon reload can only be cancelled indirectly (limit reached, weapon switch, others) or via pressing the reload key
	// no need to check if reloading is happening; cancelling may be done anyways as cancelling has no nefarious side effects
	return (bIsReloading && (WeaponActionTriggered || LimitReached || WeaponSwitched || OutsideFactors));

}


// Function called once the initial delay has elapsed
void USPerBulletReloadComponent::OnInitialDelayFinished(void) {

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);

	// Set bullets-in retriggerable timer
	// Set looping timer for the introduction of ammo in the weapon
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USPerBulletReloadComponent::OnBulletsIn, ReloadCycleDuration, true);

	// Update HUD Info via broadcast
	ReloadStartedDelegate.Broadcast(ReloadCycleDuration);

	UE_LOG(LogTemp, Log, TEXT("Cleared per-bullet initial delay timer, started per-bullet bullets-in looping timer with duration %f"), ReloadCycleDuration);

}


// Function called every time the reload cycle duration elapses
void USPerBulletReloadComponent::OnBulletsIn(void) {

	uint8 BulletsReloaded = 0;

	// check if we are attempting to reload more bullets than permitted; if true, just set the currently active bullets to maximum, otherwise, proceed as normal
	if ((BulletsCurrentlyActive + BulletPerReload) > MaximumCapacity) {

		BulletsReloaded = (MaximumCapacity - BulletsCurrentlyActive);
		BulletsExpended += BulletsReloaded;
		BulletsCurrentlyActive = MaximumCapacity;

	}
	else {

		BulletsExpended += BulletPerReload;
		BulletsCurrentlyActive += BulletPerReload;
		BulletsReloaded = BulletPerReload;

	}

	// Increment reload cycles
	++PerBulletReloadCycles;

	// Update HUDInfo via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	BulletsAddedDelegate.Broadcast(BulletsReloaded);

	bool MaximumBulletsPerLifeReached = (bCapBulletsPerLife && (BulletsExpended == BulletsPerLife));
	bool MaximumBulletCapacityReached = (BulletsCurrentlyActive >= MaximumCapacity);

	if (MaximumBulletsPerLifeReached || MaximumBulletCapacityReached) {

		HandleReloadCancel(UReloadCancelTrigger::TriggerByLimitReached);

	}
	// if there is no reason to cancel the reload, broadcast the start of a new reload cycle
	else {

		ReloadStartedDelegate.Broadcast(ReloadCycleDuration);

	}
	UE_LOG(LogTemp, Log, TEXT("Loaded %u bullets, current capacity %u out of %u maximum, %u bullets used out of %u maximum per this life"), BulletPerReload, BulletsCurrentlyActive, MaximumCapacity, BulletsExpended, bCapBulletsPerLife ? BulletsPerLife : INT32_MAX);

}


// Function called once the winddown period has passed (the winddown period is set in motion by the HandleReloadCancel function)
void USPerBulletReloadComponent::OnReloadComplete(UReloadCancelTrigger CancelType) {

	ReloadCompletedDelegate.Broadcast(CancelType);

	// Reset protection flags (IsReloading, retrigger and minimum cycles), clear timer and update HUD
	bIsReloading = false;
	bReloadIsBeingCancelled = false;
	PerBulletReloadCycles = 0;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

	UE_LOG(LogTemp, Log, TEXT("Per-bullet reload finished"));

}