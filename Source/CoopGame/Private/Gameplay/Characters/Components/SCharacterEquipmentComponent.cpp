// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Characters/Components/SCharacterEquipmentComponent.h"
#include "Gameplay/Characters/SPlayerCharacter.h"
#include "Gameplay/Weapons/SWeapon.h"



// Sets default values for this component's properties
USCharacterEquipmentComponent::USCharacterEquipmentComponent() {


	
}

// Spawns the configured loadout for the character and delegates its addition to the loadout
void USCharacterEquipmentComponent::SpawnDefaultLoadout() {

	// Success flag used to trigger broadcast of loadout spawn
	bool Success = false;

	// Used to set the initially active weapon
	uint8 InitialActiveWeaponSlotNumber = 0;
	
	ASPlayerCharacter* OwningCharacter = Cast<ASPlayerCharacter>(GetOwner());
	
	if (OwningCharacter) {

		// Guarantees we will only try to spawn only the weapons types from the first 3 members of the DefaultWeapons array
		uint8 NumUniqueWeapons = (DefaultWeapons.Num() <= 3) ? DefaultWeapons.Num() : 3;

		for (uint8 i = 0; i < NumUniqueWeapons; i++){

			// Spawn Weapon only if the subclass has been set up properly
			if (DefaultWeapons[i]) { 

				// Spawn weapon proper
				FTransform PlayerTransform = OwningCharacter->GetActorTransform();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
				ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(DefaultWeapons[i], PlayerTransform, SpawnParameters);

				// If weapon spawn, add it to the loadout
				if (NewWeapon) {

					// If we're currently spawning the first weapon class, we want to set it to active
					bool bEquipFirstWeapon = (i == 0);
					bool AddWeaponSuccess = AddWeaponToLoadout(NewWeapon, bEquipFirstWeapon);
					
					if (AddWeaponSuccess) {

						bool WeaponCharSetup = SetupWeaponCharacterRelationship(NewWeapon, OwningCharacter);
						
						// since Success is initiated as false, we cannot use AND flags for first equipped weapon, plus, tell delegate broadcast which is the initially active slot
						if (bEquipFirstWeapon && WeaponCharSetup) {

							Success = true;
							InitialActiveWeaponSlotNumber = (i + 1);

							// We can send nullptr here, all subscribed functions must always have nullptr checks!
							WeaponChangeDelegate.Broadcast(nullptr, NewWeapon, 1);
							NewWeapon->RequestSubcomponentBroadcast();
														
						}
						// if it was successful before and is successful still, flag remains true
						else if (WeaponCharSetup) {

							Success &= WeaponCharSetup;
							
						}
						
						if (!WeaponCharSetup) {

							UE_LOG(LogTemp, Error, TEXT("Error when setting up character-weapon relationship for index %u of Default Weapons"), i);
							
						}
						
					}
					else {

						UE_LOG(LogTemp, Error, TEXT("Error when adding Weapon spawned from index %u of Default Weapons"), i);
				
					}
					
				}
				else {

					UE_LOG(LogTemp, Error, TEXT("Weapon has NOT spawned from index %u of Default Weapons, null pointer"), i);
					
				}

			}
			else {

				UE_LOG(LogTemp, Error, TEXT("Index %u of DefaultWeapons is not set properly, null pointer"), i);
				
			}
			
		}

		// todo is this code leftover? this is odd
		// Spawn weapon and attach to player skeletal mesh
		if (WeaponInSlot1) {

			WeaponInSlot1->SetOwner(OwningCharacter);

			// Assign weapon in slot 1 as current weapon by default
			CurrentWeapon = WeaponInSlot1;

			if (CurrentWeapon) {

				FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
				CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, CurrentWeapon->GetCharacterSocketName());
		
			}
		
		}
		
	}

	if (Success) {

		FLoadoutInfo temp = CreateLoadoutInfo();
		LoadoutSpawnDelegate.Broadcast(temp, CurrentWeapon, InitialActiveWeaponSlotNumber);
		
	}
	
}


// Deactivate the current weapon and change the currently active weapon to the weapon in slot 1
bool USCharacterEquipmentComponent::ChangeToWeapon1(void) {

	bool Success = false;

	if (CurrentWeapon && WeaponInSlot1) {

		// don't change weapons if the requested slot is already active
		if (CurrentWeapon != WeaponInSlot1) {
			
			// Save weapon to be deactivated to be passed to weapon change delegate
			ASWeapon* PreviousWeapon = CurrentWeapon;

			// deactivate current weapon, assign new weapon and activate it
			bool DeactivateSuccess = CurrentWeapon->DeactivateWeapon();
			CurrentWeapon = WeaponInSlot1;
			bool ActivateSuccess = WeaponInSlot1->ActivateWeapon();

			Success = (DeactivateSuccess && ActivateSuccess);

			// if operation was successful, broadcast the previous and new weapons for rebinding
			if (Success) {

				WeaponChangeDelegate.Broadcast(PreviousWeapon, CurrentWeapon, 1);
				// start any automatic actions
				CurrentWeapon->RequestSubcomponentBroadcast();
				CurrentWeapon->TriggerAutomaticActions();
				
			}
			
		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Nullptr error in change to weapon slot 1"));
		
	}

	return Success;
}

// Deactivate the current weapon and change the currently active weapon to the weapon in slot 2
bool USCharacterEquipmentComponent::ChangeToWeapon2(void) {

	bool Success = false;

	if (CurrentWeapon && WeaponInSlot2) {

		// don't change weapons if the requested slot is already active
		if (CurrentWeapon != WeaponInSlot2) {
			
			// Save weapon to be deactivated to be passed to weapon change delegate
			ASWeapon* PreviousWeapon = CurrentWeapon;

			// deactivate current weapon, assign new weapon and activate it
			bool DeactivateSuccess = CurrentWeapon->DeactivateWeapon();
			CurrentWeapon = WeaponInSlot2;
			bool ActivateSuccess = WeaponInSlot2->ActivateWeapon();

			Success = (DeactivateSuccess && ActivateSuccess);

			// if operation was successful, broadcast the previous and new weapons for rebinding
			if (Success) {

				WeaponChangeDelegate.Broadcast(PreviousWeapon, CurrentWeapon, 2);
				CurrentWeapon->RequestSubcomponentBroadcast();
				CurrentWeapon->TriggerAutomaticActions();
				
			}

		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Nullptr error in change to weapon slot 2"));
		
	}

	return Success;
}



// Deactivate the current weapon and change the currently active weapon to the weapon in slot 3
bool USCharacterEquipmentComponent::ChangeToWeapon3(void) {

	bool Success = false;

	if (CurrentWeapon && WeaponInSlot3) {

		// don't change weapons if the requested slot is already active
		if (CurrentWeapon != WeaponInSlot3) {
		
			// Save weapon to be deactivated to be passed to weapon change delegate
			ASWeapon* PreviousWeapon = CurrentWeapon;

			// deactivate current weapon, assign new weapon and activate it
			bool DeactivateSuccess = CurrentWeapon->DeactivateWeapon();
			CurrentWeapon = WeaponInSlot3;
			bool ActivateSuccess = WeaponInSlot3->ActivateWeapon();

			Success = (DeactivateSuccess && ActivateSuccess);

			// if operation was successful, broadcast the previous and new weapons for rebinding
			if (Success) {

				WeaponChangeDelegate.Broadcast(PreviousWeapon, CurrentWeapon, 3);
				CurrentWeapon->RequestSubcomponentBroadcast();
				CurrentWeapon->TriggerAutomaticActions();				
				
			}
			
		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Nullptr error in change to weapon slot 3"));
		
	}

	return Success;
	
}



// Execute the pressing action of the currently active weapon's primary action
void USCharacterEquipmentComponent::PerformCurrentWeaponPrimaryActionPress(void) {

	if (CurrentWeapon) {
		
		CurrentWeapon->OnPrimaryWeaponActionPressed();
		
	}
	
}



// Execute the releasing action of the currently active weapon's primary action
void USCharacterEquipmentComponent::PerformCurrentWeaponPrimaryActionRelease(void) {

	if (CurrentWeapon) {
		
		CurrentWeapon->OnPrimaryWeaponActionReleased();
		
	}
	
}



// Execute the pressing action of the currently active weapon's secondary action
void USCharacterEquipmentComponent::PerformCurrentWeaponSecondaryActionPress(void) {

	if (CurrentWeapon) {
		
		CurrentWeapon->OnSecondaryWeaponActionPressed();
		
	}
	
}



// Execute the releasing action of the currently active weapon's secondary action
void USCharacterEquipmentComponent::PerformCurrentWeaponSecondaryActionRelease(void) {

	if (CurrentWeapon) {
		
		CurrentWeapon->OnSecondaryWeaponActionReleased();
		
	}
	
}


// Request a reload to the current weapon
void USCharacterEquipmentComponent::ReloadCurrentWeapon() {

	if (CurrentWeapon) {
		
		CurrentWeapon->OnReloadPressed();
		
	}
	
}


// Called when the game starts
void USCharacterEquipmentComponent::BeginPlay() {

	Super::BeginPlay();
		
}


// Adds a new weapon to the loadout. Returns false if the weapon already exists in the inventory, true if it's a new weapon.
bool USCharacterEquipmentComponent::AddWeaponToLoadout(ASWeapon* NewWeapon, bool bActivateFirstWeapon) {

	bool Success = false;
	
	if (CanNewWeaponBeAdded(NewWeapon)) {
		
		bool WeaponAdded = FindAndAddWeaponToSlot(NewWeapon);
	
		if (WeaponAdded) {

			// Activate weapon automatically if it is the first to be added
			if (bActivateFirstWeapon) {
				
				NewWeapon->ActivateWeapon();
				CurrentWeapon = NewWeapon;
				
			}
			// Deactivate weapon if it's the second or third weapons
			else {
				
				NewWeapon->DeactivateWeapon();
				
			}

			Success = true;
			
		}

		else {

			UE_LOG(LogTemp, Error, TEXT("Could not add weapon to slot"));
	
		}
		
	}
	else {
		
		UE_LOG(LogTemp, Error, TEXT("Weapon cannot be added, a weapon of the same type already exists in the loadout"));
		
	}
	
	return Success;
	
}


// Checks if a weapon of the same type as the newly created one is already present in the loadout
bool USCharacterEquipmentComponent::CanNewWeaponBeAdded(ASWeapon* NewWeapon) {


	// Check if a weapon of the same class is already in the loadout
	bool SameWeaponInSlot1 = (NewWeapon && WeaponInSlot1 && (NewWeapon->GetClass() == WeaponInSlot1->GetClass()));
	bool SameWeaponInSlot2 = (NewWeapon && WeaponInSlot2 && (NewWeapon->GetClass() == WeaponInSlot2->GetClass()));
	bool SameWeaponInSlot3 = (NewWeapon && WeaponInSlot3 && (NewWeapon->GetClass() == WeaponInSlot3->GetClass()));

	// Return true if there is no weapon of the same type in slots 1, 2 or 3
	return (!SameWeaponInSlot1 && !SameWeaponInSlot2 && !SameWeaponInSlot3);
	
}


// Finds the first open slot and adds the new weapon to said slot, returning the success of the operation
bool USCharacterEquipmentComponent::FindAndAddWeaponToSlot(ASWeapon* NewWeapon) {

	bool Success = false;

	// Check slot 1 and add if possible
	if (!WeaponInSlot1) {
	
		WeaponInSlot1 = NewWeapon;
		Success = true;

	}
	// Check slot 2 and add if possible
	else if (!WeaponInSlot2) {

		WeaponInSlot2 = NewWeapon;
		Success = true;

	}
	// Check slot 3 and add if possible
	else if (!WeaponInSlot3) {

		WeaponInSlot3 = NewWeapon;
		Success = true;

	}

	return Success;

}


// Set the weapon's OwningCharacter variable and attachment to the player character, setting weapons invisible at start if they are not the first weapon to be created
bool USCharacterEquipmentComponent::SetupWeaponCharacterRelationship(ASWeapon* NewWeapon, ASPlayerCharacter* OwningCharacter) {

	bool Success = false;

	if (NewWeapon && OwningCharacter) {

		// Set the weapon's owner to be the player character
		NewWeapon->WeaponOwner = OwningCharacter;
		NewWeapon->SetOwner(OwningCharacter);

		// Attach weapon to the appropriate socket in the player character mesh and disable collision
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		NewWeapon->AttachToComponent(OwningCharacter->GetMesh(), AttachRules, NewWeapon->GetCharacterSocketName());
		NewWeapon->MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Success = true;

	}

	return Success;
	
}

FLoadoutInfo USCharacterEquipmentComponent::CreateLoadoutInfo() const {

	FLoadoutInfo temp;

	// Set-up Weapon 1 info
	if (WeaponInSlot1) {
		
		temp.Weapon1Info.IsEquipped = true;
		temp.Weapon1Info.EquipmentShortName = WeaponInSlot1->GetWeaponShortName();
		temp.Weapon1Info.EquipmentSlot = 1;
		
	}
	// Set-up Weapon 2 info
	if (WeaponInSlot2) {
		
		temp.Weapon2Info.IsEquipped = true;
		temp.Weapon2Info.EquipmentShortName = WeaponInSlot2->GetWeaponShortName();
		temp.Weapon2Info.EquipmentSlot = 2;
		
	}
	// Set-up Weapon 3 info
	if (WeaponInSlot3) {
		
		temp.Weapon3Info.IsEquipped = true;
		temp.Weapon3Info.EquipmentShortName = WeaponInSlot3->GetWeaponShortName();
		temp.Weapon3Info.EquipmentSlot = 3;
		
	}
	// Set-up skill info (placeholder, no skills implemented)
	temp.SkillInfo.IsEquipped = false;
	
	return temp;
	
}
