// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Weapons/Components/ISAmmoSystemComponent.h"



// Sets default values for this component's properties
UISAmmoSystemComponent::UISAmmoSystemComponent() {

	bIsReloading = false;
	bReloadIsBeingCancelled = false;

}


// Requests the cancelling of any reloading action that is in process and consumes a given number of bullets, returning true if successful in spending ammo (also returns the number of bullets truly consumed by the ammo system as the third parameter)
bool UISAmmoSystemComponent::HandleAmmoUseRequest(bool CanBePartial, uint8 NumToExpend, uint8& BulletsTrulyExpended) {

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
		//AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());

	}

	return retval;

}


// ABSTRACT // Handling function for reload triggering requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool UISAmmoSystemComponent::HandleReloadTrigger(bool OtherWeaponActionOccurring, const UReloadCancelTrigger CancelType) {

	unimplemented();
	return false;

}


// ABSTRACT // Handling function for reload cancelling requests, to be called when player fires weapon, changes weapon, orders reload (among others)
bool UISAmmoSystemComponent::HandleReloadCancel(const UReloadCancelTrigger& CancelType) {

	unimplemented();
	return false;

}


// Activate the automatic properties of the weapon upon switching
void UISAmmoSystemComponent::ActivateAutomaticProperties(void) {

	// Deliberately empty, no automatic properties by default

}


// ABSTRACT // Returns the maximum number of bullets that can be available for the weapon simultaneously (depends on reload type)
int32 UISAmmoSystemComponent::GetMaximumBullets(void) const {

	unimplemented();
	return 0;
}


// ABSTRACT // Expose the reload type
UReloadType UISAmmoSystemComponent::GetReloadType(void) const {

	unimplemented();
	return UReloadType::NoReload;
}


// Broadcasts information on generic request
void UISAmmoSystemComponent::OnBroadcastRequested(void) const {

	AmmoChangedDelegate.Broadcast(BulletsCurrentlyActive, GetAvailableReloads());

}


void UISAmmoSystemComponent::BeginPlay() {

	BulletsCurrentlyActive = GetMaximumBullets();

}


// ABSTRACT // Returns the number of leftover bullets/magazines, returns -1 for passive reload type
int32 UISAmmoSystemComponent::GetAvailableReloads(void) const {

	unimplemented();
	return 0;

}


// ABSTRACT // Indicates whether this weapon can be fired, using intBulletsCurrentlyActive as a basis vs the number of bullets the attack to be used requires and whether reloading blocks firing the weapon
bool UISAmmoSystemComponent::CanFire(bool CanBePartial, uint8 NumToExpend) const {

	unimplemented();
	return false;

}