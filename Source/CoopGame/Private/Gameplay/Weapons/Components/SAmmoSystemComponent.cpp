// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Components/SAmmoSystemComponent.h"
#include "TimerManager.h"



// Sets default values for this component's properties
USAmmoSystemComponent::USAmmoSystemComponent() {

	// Not reloading nor cancelling it at construction, 0 out usages
	bIsReloading = false;
	bReloadIsBeingCancelled = false;
	MagazinesExpended = 0;
    BulletsExpended = 0;
	PerBulletReloadCycles = 0;
	
}


// Requests the cancelling of any reloading action that is in process and consumes a given number of bullets, returning true if successful in spending ammo (also returns the number of bullets truly consumed by the ammo system as the third parameter)
bool USAmmoSystemComponent::ManageAmmoWhenUseRequested(bool CanBePartial, const uint8 NumToExpend, uint8& BulletsTrulyExpended) {

	bool retval = false;
	
	// Check if there are limitations to the ammo system itself to firing
	if (CanFire(CanBePartial, NumToExpend)) {

		// Subtract bullets from the ones available in the weapon
		// Prevent underflow by checking if there are more bullets being requested than available
		if (NumToExpend <= BulletsCurrentlyActive) {

			// Expend the bullets being requested and increment count in full
			BulletsTrulyExpended = NumToExpend;
			// BulletsExpended += NumToExpend;
			BulletsCurrentlyActive -= NumToExpend;
			
			retval = true;
		
		}

		// If the requested number of bullets to be consumed is higher than what we have available, expend those
		else {

			// Expend all available bullets and adjust count increment accordingly
			BulletsTrulyExpended = BulletsCurrentlyActive;
			BulletsCurrentlyActive = 0;

			retval = true;
			
		}

		// We try to cancel the reload
		HandleReloadCancel(UReloadCancelTrigger::TriggerByWeaponAction);
		
		// independent of which branch chosen above, the bullets currently available had their value updated, so broadcast it
		AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
		
	}

	return retval;
	
}


// Main reload function, delegates and branches according to reload type; returns true on reload success
bool USAmmoSystemComponent::HandleReloadTrigger(bool WeaponIsBusy, const UReloadCancelTrigger CancelType) {

	bool Success = false;

	switch (ReloadType) {
		
	case UReloadType::PassiveReload:
		Success = TriggerPassiveReload(CancelType);
		break;
			
	case UReloadType::ActiveMagazineReload:
		Success = TriggerMagazineReload(WeaponIsBusy);
		break;
		
	case UReloadType::ActivePerBulletReload:
		Success = TriggerPerBulletReload(WeaponIsBusy);
		break;
		
	default:
		UE_LOG(LogTemp, Error, TEXT("No reload system configured!"))
		Success = false;
		break;
		
	}

	UE_LOG(LogTemp, Log, TEXT("Weapon reload trigger %u"), Success);
	
	return Success;
	
}


// Handling function for reload cancelling requests, either called indirectly via HandleReloadTrigger (in which we use IsReloading as a flip-flop between triggering reload or cancel
bool USAmmoSystemComponent::HandleReloadCancel(const UReloadCancelTrigger& CancelType) {

	bool Success = false;
		
	switch (ReloadType) {
		
	case UReloadType::PassiveReload:
		Success = CancelPassiveReload(CancelType);
		break;
			
	case UReloadType::ActiveMagazineReload:
		Success = CancelMagazineReload(CancelType);
		break;
		
	case UReloadType::ActivePerBulletReload:
		Success = CancelPerBulletReload(CancelType);
		break;
		
	default:
		UE_LOG(LogTemp, Error, TEXT("No reload system configured!"))
		Success = false;
		break;
		
	}

	UE_LOG(LogTemp, Log, TEXT("Weapon reload cancel %u"), Success);
	
	return Success;
	
}


// Activate the automatic properties of the weapon upon switching
void USAmmoSystemComponent::ActivateAutomaticProperties() {

	// trigger passive reload if weapon is of that type
	if (ReloadType == UReloadType::PassiveReload) {

		TriggerPassiveReload(UReloadCancelTrigger::GenericPassiveTrigger);
		
	}
	
}


// Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
int32 USAmmoSystemComponent::GetMaximumBullets() const {

	switch (ReloadType) {
		
	case UReloadType::PassiveReload:
		return MaximumPassiveCapacity;
		
	case UReloadType::ActiveMagazineReload:
		return BulletPerMagazine;
		
	case UReloadType::ActivePerBulletReload:
		return MaximumBulletCapacity;
		
	default:
		return 0;
		
	}

}


// Expose the reload type
UReloadType USAmmoSystemComponent::GetReloadType() const {

	return ReloadType;
	
}


// Broadcasts information on generic request
void USAmmoSystemComponent::OnBroadcastRequested() const {

	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	
}


// Called when the game starts
void USAmmoSystemComponent::BeginPlay() {

	Super::BeginPlay();

	// Set status variables to their defaults (start always at maximum capacity)
	BulletsCurrentlyActive = GetMaximumBullets();

	// Calculate leftover bullets/magazines for Active Magazine and Active Per-Bullet weapons
	if (ReloadType == UReloadType::ActiveMagazineReload) {

		// Always 1 because at game start we've only used 1 magazine (hurr durr)
		MagazinesExpended = 1;
		
	}
	else if (ReloadType == UReloadType::ActivePerBulletReload) {

		// Always the currently available bullets at start
		BulletsExpended = BulletsCurrentlyActive;
		
	}

	// no need to broadcast changes of ammo and whatnot here because that initial set up can be done once the weapon to which this ammo system belongs is activated
	
}


// Returns the number of available bullets/magazines depending on the active reload type; if the reload type is passive or if there is no maximum cap on reloads for this weapon's type, this function returns -1
int32 USAmmoSystemComponent::GetAvailableReloads() const {

	// values under 0 are coded values for no reload limit
	switch (ReloadType) {
		
	case UReloadType::PassiveReload:
		return -1;
		
	case UReloadType::ActiveMagazineReload:
		if (bCapMagazinesPerLife) return (MagazinesPerLife - MagazinesExpended);
		else return -1;
		
	case UReloadType::ActivePerBulletReload:
		if (bCapBulletsPerLife) {

			// Guard against bad configuration in which MaximumBulletCapacity > BulletsPerLife (recall that at BeginGame, BulletsExpended = MaximumBulletCapacity
			return ((BulletsExpended > BulletsPerLife) ? 0 : (BulletsPerLife - BulletsExpended));
			
		}
		else {
			return -1;
		}
		
	default:
		return 0;
		
	}
	
}


// Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires (depending on whether partial fire is accepted) and whether reloading blocks firing the weapon
bool USAmmoSystemComponent::CanFire(bool CanBePartial, uint8 NumToExpend) const {

	// If we can fire only part of the RequiredBullets, we check only if we have 1 or more bullets
	// Otherwise, we check if we can fire all required bullets
	bool EnoughBulletsAvailable = (CanBePartial && (BulletsCurrentlyActive > 0)) || (BulletsCurrentlyActive >= NumToExpend);

	// reload does not block firing in passive reload or no reload, only in per-bullet or magazine types
	bool IsNotBlockedByReload = (!bIsReloading || (ReloadType == UReloadType::PassiveReload) || (ReloadType == UReloadType::NoReload));
	
	return (EnoughBulletsAvailable && IsNotBlockedByReload);
	
}


// Returns true if the request performed and the weapon status and configuration is proper for triggering of passive-type reload
bool USAmmoSystemComponent::CanTriggerPassiveReload(const UReloadCancelTrigger& CancelType) const {

	// Bullets currently active are less than the maximum bullet capacity (there is space in weapon for more bullets)
	bool SpaceAvailableInWeapon = (BulletsCurrentlyActive < MaximumPassiveCapacity);
	
	bool ReloadKeyPressed = (CancelType == UReloadCancelTrigger::TriggerByReload);

	UE_LOG(LogTemp, Log, TEXT("Passive Reload Trigger Validation, SpaceAvailableInWeapon %u, ReloadKeyPressed %u"), SpaceAvailableInWeapon, ReloadKeyPressed);
	
	// If there is space in the weapon and the reload key is not requesting passive reload, we can trigger passive reload
	return (!bIsReloading && SpaceAvailableInWeapon && !ReloadKeyPressed);
	
}


// Returns true if the weapon has magazine reload type configured and has no impediments to reload
bool USAmmoSystemComponent::CanTriggerMagazineReload(bool OtherWeaponActionOccurring) const {

	// There is no maximum on magazines per life OR the magazines expended in this life are less than the maximum magazines available per life (there are magazines in storage)
	bool MagazinesAvailableForUse = (!bCapMagazinesPerLife || (MagazinesExpended < MagazinesPerLife));

	// If both the bullet timer and the completion timer are set, we do not execute magazine reload (just-in-case check, as this should never be reached since bIsReloading is only set to false once OnCompleteMagazineReload is called
	bool NewBulletTimerNotActive = (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_NewBulletsTimer));
	bool ReloadCompletionTimerNotActive = (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_ReloadCompletion));

	UE_LOG(LogTemp, Log, TEXT("Magazine Reload Trigger Validation, bIsReloading %u, OtherWeaponActionOccurring %u, MagazinesAvailableForUse %u, NewBulletTimerNotActive %u, ReloadCompletionTimerNotActive %u"), bIsReloading, OtherWeaponActionOccurring, MagazinesAvailableForUse, NewBulletTimerNotActive, ReloadCompletionTimerNotActive);
	
	// If there are magazines in storage AND no reload-blocking weapon action is happening, reload is possible	
	return (!bIsReloading && !OtherWeaponActionOccurring && MagazinesAvailableForUse && NewBulletTimerNotActive && ReloadCompletionTimerNotActive);
	
}


// Returns true if the weapon has magazine per-bullet type configured and has no impediments to reload
bool USAmmoSystemComponent::CanTriggerPerBulletReload(bool OtherWeaponActionOccurring) const {

	// Bullets currently active are less than the maximum bullet capacity (there is space in weapon for more bullets)
	bool SpaceAvailableInWeapon = (BulletsCurrentlyActive < MaximumBulletCapacity);

	// There is no maximum on bullets per life OR the required bullets for a reload are equal or less than the bullets expended in this life (bullets available per life minus the bullets already loaded into the weapon)
	bool EnoughBulletsAvailableInStorage = (!bCapBulletsPerLife || (BulletsPerLife - BulletsExpended) >= BulletPerReload);

	// If the bullet timer is set, we do not execute per-bullet reload (just-in-case check, as this should never be reached since bIsReloading is only set to false once OnCompletePerBulletReload is called
	bool NewBulletTimerNotActive = !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_NewBulletsTimer);

	UE_LOG(LogTemp, Log, TEXT("Per-Bullet Reload Trigger Validation, bIsReloading %u, SpaceAvailableInWeapon %u, EnoughBulletsAvailableInStorage %u, NewBulletTimerNotActive %u"), bIsReloading, SpaceAvailableInWeapon, EnoughBulletsAvailableInStorage, NewBulletTimerNotActive);
	
	// If the above conditions are true AND no reload-blocking weapon action is happening, reload is possible
	return (!bIsReloading && !OtherWeaponActionOccurring && SpaceAvailableInWeapon && EnoughBulletsAvailableInStorage && NewBulletTimerNotActive);
	
}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of passive-type reload
bool USAmmoSystemComponent::CanCancelPassiveReload(UReloadCancelTrigger CancelType) const {

	bool WeaponActionTriggered = (CancelType == UReloadCancelTrigger::TriggerByWeaponAction) && (BulletsCurrentlyActive > 0);

	// needed to prevent some multithreading fuckery going on with WeaponActionTriggered
	bool temp = WeaponActionTriggered;
	
	bool LimitReached = (CancelType == UReloadCancelTrigger::TriggerByLimitReached);
	bool WeaponSwitched = (CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch);	
	bool OutsideFactors = (CancelType == UReloadCancelTrigger::TriggerByExternal);
	
	UE_LOG(LogTemp, Log, TEXT("Passive Reload Cancel Validation, bIsReloading %u, WeaponActionTriggered %u, LimitReached %u, WeaponSwitched %u, OutsideFactors %u"), bIsReloading, WeaponActionTriggered, LimitReached, WeaponSwitched, OutsideFactors);

	if (BulletsCurrentlyActive == 0) {
		
		//UE_LOG(LogTemp, Log, TEXT("TEXT bulletsatzero %u, wepact %u"), bulletsatzero, wepact);
		
	}
	
	// passive weapon reload can only be cancelled indirectly (limit reached, weapon switch, others) or by a weapon action
	// no need to check if reloading is happening; cancelling may be done anyways as cancelling has no nefarious side effects
	return (bIsReloading && (WeaponActionTriggered || LimitReached || WeaponSwitched || OutsideFactors));
	
}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of magazine-type reload
bool USAmmoSystemComponent::CanCancelMagazineReload(UReloadCancelTrigger CancelType) const {

	// Weapon with magazine-type reload type is not reloading OR is reloading and is configured with cancellation properties
	bool ReloadKeyPressed = ((CancelType == UReloadCancelTrigger::TriggerByReload) && bEnableMagazineReloadCancel);
	bool LimitReached = (CancelType == UReloadCancelTrigger::TriggerByLimitReached);
	bool WeaponSwitched = (CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch);	
	bool OutsideFactors = (CancelType == UReloadCancelTrigger::TriggerByExternal);

	UE_LOG(LogTemp, Log, TEXT("Magazine Reload Cancel Validation, bIsReloading %u, ReloadKeyPressed %u, LimitReached %u, WeaponSwitched %u, OutsideFactors %u"), bIsReloading, ReloadKeyPressed, LimitReached, WeaponSwitched, OutsideFactors);
	
	// magazine weapon reload can only be cancelled indirectly (limit reached, weapon switch, others) or via pressing the reload key
	// no need to check if reloading is happening; cancelling may be done anyways as cancelling has no nefarious side effects
	return (bIsReloading && (ReloadKeyPressed || LimitReached || WeaponSwitched || OutsideFactors));
	
}


// Returns true if the request performed and the weapon status and configuration is proper for cancellation of per-bullet-type reload
bool USAmmoSystemComponent::CanCancelPerBulletReload(UReloadCancelTrigger CancelType) const {

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


// todo HIGH PRIORITY: if there are no available bullets but the user continues using the weapon, passive reload never starts (perhaps a functionality in which holding the button down doesn't retrigger the passive reload timer, blocking firing, but releasing then pressing the button anew would make it possible to reuse the weapon? just an idea, not high priority)
// Function dedicated to handling passive reload
bool USAmmoSystemComponent::TriggerPassiveReload(const UReloadCancelTrigger& CancelType) {

	bool ReloadSuccess = false;

	if (CanTriggerPassiveReload(CancelType)) {
		
		// Immediately set up the timer afterwards
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitialDelayTimer, this, &USAmmoSystemComponent::OnStartDelayFinishedPassiveReload, PassiveReloadDelay);

		// Update reload status and update HUDInfo via broadcast
		PassiveCancelDelegate.Broadcast(CancelType);
		PassiveStartDelegate.Broadcast(PassiveReloadDelay);
		bIsReloading = true;
		
		UE_LOG(LogTemp, Log, TEXT("Set passive reload timers, with delay of %f seconds and per bullet tick of %f seconds"), PassiveReloadDelay, ReloadTimePassive);	

		ReloadSuccess = true;
		
		
	}

	return ReloadSuccess;
	
}


// Function dedicated to handling active magazine reload
bool USAmmoSystemComponent::TriggerMagazineReload(bool OtherWeaponActionOccurring) {

	bool ReloadSuccess = false;

	if (CanTriggerMagazineReload(OtherWeaponActionOccurring)) {
		
		// Set timers for completion of the reload phases
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USAmmoSystemComponent::OnBulletsInMagazineReload, BulletsReloadTimeMagazine);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReloadCompletion, this, &USAmmoSystemComponent::OnCompleteMagazineReload, (BulletsReloadTimeMagazine + WinddownReloadTimeMagazine));

		// Update reload status and update HUDInfo, if bEnableMagazineReloadCancel then this action can be cancelled
		bIsReloading = true;

		UE_LOG(LogTemp, Log, TEXT("Set magazine reload timers, bIsReloading %u"), bIsReloading);

		ReloadSuccess = true;

		MagazineStartDelegate.Broadcast(BulletsReloadTimeMagazine);
		
	}
	
	return ReloadSuccess;
	
}

// todo LOW PRIORITY: per-bullet rate of fire still on-going when weapon action reload triggers cancel, meaning that there's still a bit of delay when reload process is finished because an attack is used
// Function dedicated to handling active bullet-per-bullet reload
bool USAmmoSystemComponent::TriggerPerBulletReload(bool OtherWeaponActionOccurring) {

	bool ReloadSuccess = false;

	if (CanTriggerPerBulletReload(OtherWeaponActionOccurring)) {
		
		// Set timer for the initial delay
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_InitialDelayTimer, this, &USAmmoSystemComponent::OnStartDelayFinishedPerBulletReload, ReloadDelayTimeBullet);
		
		// Update reload status and update HUDInfo, reload cancelling always possible in Per-Bullet types
		bIsReloading = true;

		// Start "wind up" animation here too - see https://forums.unrealengine.com/t/elegant-solution-to-play-certain-animations-depending-on-what-weapon-a-player-is-using/27939/7 on how to handle
	
		UE_LOG(LogTemp, Log, TEXT("Set per-bullet initial delay timer with duration %f, bIsReloading %u"), ReloadDelayTimeBullet, bIsReloading);

		ReloadSuccess = true;
		
		PerBulletStartDelegate.Broadcast(ReloadDelayTimeBullet);
		
	}	

	return ReloadSuccess;
	
}


// Function called once the initial delay for passive reload passes
void USAmmoSystemComponent::OnStartDelayFinishedPassiveReload() {

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);

	// Set bullets-in retriggerable timer
	// Set looping timer for the introduction of ammo in the weapon
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USAmmoSystemComponent::OnBulletsInPassiveReload, ReloadTimePassive, true);
	
	// Update HUD Info via broadcast
	PassiveReloadCycleStartDelegate.Broadcast(ReloadTimePassive);
	
	UE_LOG(LogTemp, Log, TEXT("Cleared passive initial delay timer, started passive bullets-in looping timer with duration %f"), ReloadTimePassive);
	
}


// Function called every time ReloadTimePassive elapses in passive reload weapons
void USAmmoSystemComponent::OnBulletsInPassiveReload() {
	
	// Increment one-by-one always, there is no need to track bullets expended but why not
	BulletsExpended	+= 1;
	BulletsCurrentlyActive += 1;
	
	// Update HUD Info via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	BulletsAddedDelegate.Broadcast(1);	/*this is always 1 */
	
	UE_LOG(LogTemp, Log, TEXT("Loaded 1 bullet, current capacity %u out of %u maximum"), BulletsCurrentlyActive, MaximumPassiveCapacity);
	
	// If we have reached the maximum capacity, we cancel the reload automatically
	if (BulletsCurrentlyActive == MaximumPassiveCapacity) {

		HandleReloadCancel(UReloadCancelTrigger::TriggerByLimitReached);
		
	}
	// if there is no reason to cancel the reload, broadcast the start of a new reload cycle
	else {
		
		PassiveReloadCycleStartDelegate.Broadcast(ReloadTimePassive);
		
	}
	
}


// Function called when the ammo is added to the currently available ammo
void USAmmoSystemComponent::OnBulletsInMagazineReload() {

	// Decrement current available magazines, reset bullets available to maximum value
	++MagazinesExpended;
	BulletsCurrentlyActive = BulletPerMagazine;

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);

	// Update HUD Info via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	MagazineBulletsInDelegate.Broadcast(WinddownReloadTimeMagazine);
	
	UE_LOG(LogTemp, Log, TEXT("Cleared magazine bullet timer"));
	
}


// Function called when the animation for magazine reload is complete
void USAmmoSystemComponent::OnCompleteMagazineReload() {

	// Make weapon available for other actions
	bIsReloading = false;

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

	// Update HUD Info via broadcast
	MagazineCompleteDelegate.Broadcast();
	
	UE_LOG(LogTemp, Log, TEXT("Cleared magazine reload timer, bIsReloading %u"), bIsReloading);
	
}


// Function called once the initial delay for per-bullet reload passes
void USAmmoSystemComponent::OnStartDelayFinishedPerBulletReload() {

	// Clear timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);

	// Set bullets-in retriggerable timer
	// Set looping timer for the introduction of ammo in the weapon
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NewBulletsTimer, this, &USAmmoSystemComponent::OnBulletsInPerBulletReload, ReloadTimeBullet, true);
	
	// Update HUD Info via broadcast
	PerBulletReloadCycleStartDelegate.Broadcast(ReloadTimeBullet);
	
	UE_LOG(LogTemp, Log, TEXT("Cleared per-bullet initial delay timer, started per-bullet bullets-in looping timer with duration %f"), ReloadTimeBullet);
	
}


// Function called after the initial delay for bullet-per-bullet reload has finished (section 1, wind up)
void USAmmoSystemComponent::OnBulletsInPerBulletReload() {

	uint8 BulletsReloaded = 0;
	
	// check if we are attempting to reload more bullets than permitted; if true, just set the currently active bullets to maximum, otherwise, proceed as normal
	if ((BulletsCurrentlyActive + BulletPerReload) > MaximumBulletCapacity) {

		BulletsReloaded = (MaximumBulletCapacity - BulletsCurrentlyActive);
		BulletsExpended += BulletsReloaded;
		BulletsCurrentlyActive = MaximumBulletCapacity;
		
	}
	else {
		
		BulletsExpended	+= BulletPerReload;
		BulletsCurrentlyActive += BulletPerReload;
		BulletsReloaded = BulletPerReload;
		
	}

	// Increment reload cycles
	++PerBulletReloadCycles;

	// Update HUDInfo via broadcast
	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());
	BulletsAddedDelegate.Broadcast(BulletsReloaded);
	
	bool MaximumBulletsPerLifeReached = (bCapBulletsPerLife && (BulletsExpended == BulletsPerLife));
	bool MaximumBulletCapacityReached = (BulletsCurrentlyActive >= MaximumBulletCapacity);
	
	if (MaximumBulletsPerLifeReached || MaximumBulletCapacityReached) {

		HandleReloadCancel(UReloadCancelTrigger::TriggerByLimitReached);
		
	}
	// if there is no reason to cancel the reload, broadcast the start of a new reload cycle
	else {
		
		PerBulletReloadCycleStartDelegate.Broadcast(ReloadTimeBullet);
		
	}
	UE_LOG(LogTemp, Log, TEXT("Loaded %u bullets, current capacity %u out of %u maximum, %u bullets used out of %u maximum per this life"), BulletPerReload, BulletsCurrentlyActive, MaximumBulletCapacity, BulletsExpended, bCapBulletsPerLife ? BulletsPerLife : INT32_MAX);
	
}


// Inner function that handles reload cancelling in passive reload type weapons, after the conditions have been checked
bool USAmmoSystemComponent::CancelPassiveReload(UReloadCancelTrigger CancelType) {

	bool Success = false;

	if (CanCancelPassiveReload(CancelType)) {
		
		// Reset IsReloading and retrigger protection flags, clear timer
		bIsReloading = false;
		bReloadIsBeingCancelled = false;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);

		// Update HUD info via broadcast
		PassiveCancelDelegate.Broadcast(CancelType);
		
		Success = true;
		
	}
	
	// if the passive reload cancel operation was successful and the weapon has not been switched out, we want to trigger the reload anew immediately, so we can do it here
	bool CanRetriggerImmediately = ((CancelType != UReloadCancelTrigger::TriggerByWeaponSwitch) && (CancelType != UReloadCancelTrigger::TriggerByLimitReached));
	
	if (!bIsReloading && CanRetriggerImmediately) {
		
		HandleReloadTrigger(false);
		
	}

	// no need to update HUD info

	return Success;
	
}


// Inner function that handles reload cancelling in active magazine reload type weapons, after the conditions have been checked
bool USAmmoSystemComponent::CancelMagazineReload(UReloadCancelTrigger CancelType) {

	bool Success = false;
	
	if (CanCancelMagazineReload(CancelType)) {
		
		// Make weapon available for other actions
		bIsReloading = false;

		// Magazine Reload cancel is instantaneous so we don't need to set bReloadIsBeingCancelled
	
		// Clear timers
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

		// update HUD info via broadcast
		MagazineCancelDelegate.Broadcast();

		Success = true;
		
	}

	return Success;
	
}


// Inner function that handles reload cancelling in active per-bullet reload type weapons, after the conditions have been checked
bool USAmmoSystemComponent::CancelPerBulletReload(UReloadCancelTrigger CancelType) {

	bool Success = false;

	if (CanCancelPerBulletReload(CancelType)) {
		
		// If the reload cancel is not caused directly by the player (cancel via weapon action or reload key) or not caused by limit reaching, then cancel must be instantaneous, also stopping retriggering of reload cancel
		bool bSkipCancelProcess = ((CancelType == UReloadCancelTrigger::TriggerByWeaponSwitch) || (CancelType == UReloadCancelTrigger::TriggerByExternal));
	
		UE_LOG(LogTemp, Log, TEXT("Per-bullet reload status, bSkipCancelProcess %u, bReloadIsBeingCancelled %u"), bSkipCancelProcess, bReloadIsBeingCancelled);
		
		// todo LOW PRIORITY: perhaps this could be nerfed as otherwise, weapon switching will yield an advantage vs letting the reload time finish?
		if (bSkipCancelProcess) {

			// No need to set flag against retrigger because cancel is automatic
		
			// Clear timer for reloading and complete cancel instantly
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_InitialDelayTimer);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NewBulletsTimer);
			OnCompleteWithCancelPerBulletReload();
			
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
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReloadCompletion, this, &USAmmoSystemComponent::OnCompleteWithSuccessPerBulletReload, ReloadEndTimeBullet);

				// update HUD via broadcast (timed cancel)
				PerBulletTimedCancelDelegate.Broadcast(ReloadEndTimeBullet);
				
				// start playing reload end animation
				UE_LOG(LogTemp, Log, TEXT("Per-bullet reload cancelled with delay"));

				Success = true;
			
			}
		
		}
		
	}
	
	return Success;
	
}


// Inner function that handles the cancelling of the per-bullet reload proper in case it's a timed cancel
void USAmmoSystemComponent::OnCompleteWithSuccessPerBulletReload() {
		
	// update HUD via broadcast (timed cancel)
	PerBulletCompleteDelegate.Broadcast();

	OnCompletePerBulletReload();
	
}


// Inner function that handles the cancelling of the per-bullet reload proper in case it's an instant cancel
void USAmmoSystemComponent::OnCompleteWithCancelPerBulletReload() {

	// update HUD via broadcast (timed cancel)
	PerBulletInstantCancelDelegate.Broadcast();

	OnCompletePerBulletReload();
	
}


// Inner function that handles the cancelling of the per-bullet reload proper after the animation has played
void USAmmoSystemComponent::OnCompletePerBulletReload() {
	
	// Reset protection flags (IsReloading, retrigger and minimum cycles), clear timer and update HUD
	bIsReloading = false;
	bReloadIsBeingCancelled = false;
	PerBulletReloadCycles = 0;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ReloadCompletion);

	UE_LOG(LogTemp, Log, TEXT("Per-bullet reload finished"));
	
}
