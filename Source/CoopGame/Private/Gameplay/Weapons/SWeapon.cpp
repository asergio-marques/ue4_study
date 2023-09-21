// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Gameplay/Characters/SPlayerCharacter.h"



// Sets default values
ASWeapon::ASWeapon() {

	// Create and setup components
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->bCastHiddenShadow = false;
	RootComponent = MeshComp;

	// Create default socket names
	FullWeaponName = FName("Generic Weapon");
	ShortWeaponName = FName("Generic");
	CharacterSocketName = FName("default_weapon_socket");

	// active status is false by default
	IsActive = false;
	
}


// Execute the pressing action of this weapon's primary action (to be fully overriden by child classes, no Super!)
void ASWeapon::OnPrimaryWeaponActionPressed() {

	UE_LOG(LogTemp, Log, TEXT("Pressed weapon primary button"));
	
}


// Execute the releasing action of this weapon's primary action (to be fully overriden by child classes, no Super!)
void ASWeapon::OnPrimaryWeaponActionReleased() {
	
	UE_LOG(LogTemp, Log, TEXT("Released weapon primary button"));
	
}


// Execute the pressing action of this weapon's secondary action (to be fully overriden by child classes, no Super!)
void ASWeapon::OnSecondaryWeaponActionPressed() {
	
	UE_LOG(LogTemp, Log, TEXT("Pressed weapon secondary button"));
	
}


// Execute the releasing action of this weapon's secondary action (to be fully overriden by child classes, no Super!)
void ASWeapon::OnSecondaryWeaponActionReleased() {
	
	UE_LOG(LogTemp, Log, TEXT("Released weapon secondary button"));
	
}


// Call the weapon reload function
void ASWeapon::OnReloadPressed() {

	UE_LOG(LogTemp, Log, TEXT("Pressed reload button"));
	
}


// Make the player camera's shake according to the weapon's parameters
void ASWeapon::ShakePlayerCamera() {
	
	if (WeaponOwner) {

		APlayerController* PC = Cast<APlayerController>(WeaponOwner->GetController());

		if (PC) {

			PC->ClientStartCameraShake(UseCameraShake);
			
		}
		
	}
	
}



// Perform necessary actions to make a weapon ready for use in gameplay
bool ASWeapon::ActivateWeapon() {

	bool Success = false;

	IsActive = true;

	// make weapon visible
	MeshComp->SetCastShadow(true);
	MeshComp->SetVisibility(true, true);
	
	if (IsActive && MeshComp->IsVisible()) {

		// set success flag
		Success = true;
		UE_LOG(LogTemp, Log, TEXT("Weapon %s activated"), *(FullWeaponName.ToString()));
		
	}
	
	return Success;
	
}



// Perform necessary actions to make a weapon unable to be used in gameplay
bool ASWeapon::DeactivateWeapon() {

	bool Success = false;

	IsActive = false;

	// make weapon invisible
	MeshComp->SetCastShadow(false);
	MeshComp->SetVisibility(false, true);

	// cancel any actions currently happening
	CancelOngoingActions();

	if (!IsActive && !(MeshComp->IsVisible())) {

		// set success flag
		Success = true;
		UE_LOG(LogTemp, Log, TEXT("Weapon %s deactivated"), *(FullWeaponName.ToString()));
		
	}
	
	return Success;

}


// Function to be defined by child classes for automatic activation of weapon stereotype/specific actions (Super should always be called on child classes!)
void ASWeapon::TriggerAutomaticActions() {

	UE_LOG(LogTemp, Log, TEXT("Activating automatic properties of weapon %s"), *(FullWeaponName.ToString()));
	
}


// Requests broadcast of information from the subcomponents after this weapon's activation has been confirmed to be successful and broadcast
void ASWeapon::RequestSubcomponentBroadcast() {

	UE_LOG(LogTemp, Log, TEXT("Subcomponent broadcast requested"));
	
}


// Returns the reload type of this weapon (NoReload by default, should be overwritten by child classes if they have a reload system)
UReloadType ASWeapon::GetReloadType() const {

	return UReloadType::NoReload;
	
}


// Get the name of the socket to which the weapon shall attach to
const FName& ASWeapon::GetCharacterSocketName() const {

	return CharacterSocketName;
	
}


// Get the weapon's short hand name
const FName& ASWeapon::GetWeaponShortName() const {

	return ShortWeaponName;
	
}


// Get the weapon's activity status
bool ASWeapon::IsWeaponActive() const {

	return IsActive;
	
}


// Function to be defined by child classes for cancelling of weapon stereotype/specific actions (Super should always be called on child classes!)
void ASWeapon::CancelOngoingActions() {

	UE_LOG(LogTemp, Log, TEXT("Cancelling actions of weapon %s"), *(FullWeaponName.ToString()));
	
}
