// Fill out your copyright notice in the Description page of Project Settings.



#include "Gameplay/Characters/SPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/Characters/Components/SCharacterEquipmentComponent.h"
#include "Gameplay/Characters/Components/SAttributesComponent.h"
#include "CoopGame/CoopGame.h"



// Sets default values
ASPlayerCharacter::ASPlayerCharacter() {

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// character is alive by default
	bIsAlive = true;

	
	// Set weapon channel collision
	GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	
	// Create and setup components
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	// Set up camera default parameters 1st
	DefaultFOV = 90.0f;
	ZoomedInFOV = 45.0f;
	POVInterpSpeed = 500.0f;
	bWantsToZoomIn = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetFieldOfView(DefaultFOV);
	CameraComp->SetupAttachment(SpringArmComp);

	// Create equipment component
	CharEquipComp = CreateDefaultSubobject<USCharacterEquipmentComponent>(TEXT("CharEquipComp"));

	// Create attributes component
	StatsComp = CreateDefaultSubobject<USAttributesComponent>(TEXT("StatsComp"));

	// Make it possible to crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
}


// Called every frame
void ASPlayerCharacter::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);

	ProcessPOV(DeltaTime);
	
}


// Called to bind functionality to input
void ASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind WASD movement
	PlayerInputComponent->BindAxis("MoveForward", this, &ASPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASPlayerCharacter::MoveRight);

	// Bind mouse movement
	PlayerInputComponent->BindAxis("LookUp", this, &ASPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASPlayerCharacter::AddControllerYawInput);

	// Bind Ctrl
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASPlayerCharacter::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASPlayerCharacter::CrouchEnd);

	// Bind Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASPlayerCharacter::Jump);

	// Bind Weapon Switches
	PlayerInputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &ASPlayerCharacter::ChangetoWeaponSlot1);
	PlayerInputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &ASPlayerCharacter::ChangetoWeaponSlot2);
	PlayerInputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &ASPlayerCharacter::ChangetoWeaponSlot3);

	
	// Bind Mouse Button Press & Release
	PlayerInputComponent->BindAction("MousePrimaryAction", IE_Pressed, this, &ASPlayerCharacter::WeaponPrimaryActionPress);
	PlayerInputComponent->BindAction("MousePrimaryAction", IE_Released, this, &ASPlayerCharacter::WeaponPrimaryActionRelease);
	PlayerInputComponent->BindAction("MouseSecondaryAction", IE_Pressed, this, &ASPlayerCharacter::WeaponSecondaryActionPress);
	PlayerInputComponent->BindAction("MouseSecondaryAction", IE_Released, this, &ASPlayerCharacter::WeaponSecondaryActionRelease);

	// Bind Reload
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, this, &ASPlayerCharacter::ReloadPressed);
	
}


// Returns player character's eye location
FVector ASPlayerCharacter::GetPawnViewLocation() const {

	if (CameraComp) {

		return CameraComp->GetComponentLocation();
		
	}
	else {

		return Super::GetPawnViewLocation();
		
	}
	
}


// Called when the game starts or when spawned
void ASPlayerCharacter::BeginPlay() {

	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	if (CharEquipComp) {

		CharEquipComp->SpawnDefaultLoadout();
		
	}

	if (StatsComp) {

		StatsComp->HPChangedDelegate.AddDynamic(this, &ASPlayerCharacter::OnHPChanged);
		
	}
	
}


// Called when "Move Forward" or "Move Backward" keys are pressed; "Move Forward" -> Value = 1.0f, "Move Backward" -> Value = -1.0f
void ASPlayerCharacter::MoveForward(float Value) {

	AddMovementInput(GetActorForwardVector() * Value);

}


// Called when "Move Right" or "Move Left" keys are pressed; "Move Right" -> Value = 1.0f, "Move Left" -> Value = -1.0f
void ASPlayerCharacter::MoveRight(float Value) {

	AddMovementInput(GetActorRightVector() * Value);

}


// Called when "Crouch" key is pressed
void ASPlayerCharacter::CrouchStart() {

	Crouch();
	
}


// Called when "Crouch" key is released
void ASPlayerCharacter::CrouchEnd() {

	UnCrouch();
	
}


// Called when "Jump" key is pressed
void ASPlayerCharacter::Jump() {
	
	Super::Jump();
	
}


// Called when "WeaponSlot1" key is pressed
void ASPlayerCharacter::ChangetoWeaponSlot1(void) {

	bool Success = false;
	
	if (CharEquipComp) {

		Success = CharEquipComp->ChangeToWeapon1();

		// request weapon info update for HUD if change successful
		if (Success) {

			// broadcast is done in the equipment manager
		
		}
		// log if there were problems
		else {
		
			UE_LOG(LogTemp, Error, TEXT("Change to weapon slot 1 unsuccessful, problem with previous weapon deactivation or new weapon activation"))
		
		}
		
	}
	
}


// Called when "WeaponSlot2" key is pressed
void ASPlayerCharacter::ChangetoWeaponSlot2(void) {

	bool Success = false;
	
	if (CharEquipComp) {

		Success = CharEquipComp->ChangeToWeapon2();

		// request weapon info update for HUD if change successful
		if (Success) {

			// broadcast is done in the equipment manager
		
		}
		// log if there were problems
		else {
		
			UE_LOG(LogTemp, Error, TEXT("Change to weapon slot 2 unsuccessful, problem with previous weapon deactivation or new weapon activation"))
		
		}
		
	}
	
}


// Called when "WeaponSlot3" key is pressed
void ASPlayerCharacter::ChangetoWeaponSlot3(void) {

	bool Success = false;
	
	if (CharEquipComp) {

		Success = CharEquipComp->ChangeToWeapon3();
	
		// request weapon info update for HUD if change successful
		if (Success) {

			// broadcast is done in the equipment manager
		
		}
		// log if there were problems
		else {
		
			UE_LOG(LogTemp, Error, TEXT("Change to weapon slot 3 unsuccessful, problem with previous weapon deactivation or new weapon activation"))
		
		}
		
	}
	
}



// Called when "MousePrimaryAction" key is pressed
void ASPlayerCharacter::WeaponPrimaryActionPress() {

	if (CharEquipComp) {

		CharEquipComp->PerformCurrentWeaponPrimaryActionPress();
		
	}
	
}


// Called when "MousePrimaryAction" key is released
void ASPlayerCharacter::WeaponPrimaryActionRelease() {

	if (CharEquipComp) {

		CharEquipComp->PerformCurrentWeaponPrimaryActionRelease();
		
	}
	
}


// Called when "MouseSecondaryAction" key is pressed
void ASPlayerCharacter::WeaponSecondaryActionPress() {

	if (CharEquipComp) {

		CharEquipComp->PerformCurrentWeaponSecondaryActionPress();
		
	}
	
}


// Called when "MouseSecondaryAction" key is released
void ASPlayerCharacter::WeaponSecondaryActionRelease() {

	if (CharEquipComp) {

		CharEquipComp->PerformCurrentWeaponSecondaryActionRelease();
		
	}
	
}


// Called when "ReloadWeapon" is pressed
void ASPlayerCharacter::ReloadPressed() {

	if (CharEquipComp) {

		CharEquipComp->ReloadCurrentWeapon();
		
	}

}


// Called when "WeaponSlot1" is pressed
void ASPlayerCharacter::ChangeToWeaponSlot1() {

	UE_LOG(LogTemp, Log, TEXT("Switch to Weapon 1 requested"));
	
}


// Called when "WeaponSlot2" is pressed
void ASPlayerCharacter::ChangeToWeaponSlot2() {

	UE_LOG(LogTemp, Log, TEXT("Switch to Weapon 2 requested"));
	
}

// Called when "WeaponSlot3" is pressed
void ASPlayerCharacter::ChangeToWeaponSlot3() {

	UE_LOG(LogTemp, Log, TEXT("Switch to Weapon 3 requested"));
	
}


// Called when a health change has been detected by the attribute component
void ASPlayerCharacter::OnHPChanged(USAttributesComponent* AttrComp, float CurrentHP, float DeltaHP
	, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {

	// if the character is still alive and the current HP reaches zero, kill it!
	if (bIsAlive && (CurrentHP <= 0.0f)) {

		bIsAlive = false;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);

		CharacterDiedDelegate.Broadcast();
		
	}	

	
}


// Smooths the adjustment of the Camera FOV, called in Tick()
void ASPlayerCharacter::ProcessPOV(float DeltaTime) {
	
	float TargetFOV = bWantsToZoomIn ? ZoomedInFOV : DefaultFOV;
	float NewFOV = FMath::FInterpConstantTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, POVInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);
	
}