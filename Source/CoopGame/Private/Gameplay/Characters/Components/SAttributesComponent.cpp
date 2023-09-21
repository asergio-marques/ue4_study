// Fill out your copyright notice in the Description page of Project Settings.




#include "Gameplay/Characters/Components/SAttributesComponent.h"



// Sets default values for this component's properties
USAttributesComponent::USAttributesComponent() {

	bAutoActivate = true;

	MaximumHP = 100;
	
}


// Called when the game starts
void USAttributesComponent::BeginPlay() {
	
	Super::BeginPlay();

	// Initialize current HP/ATP at game start
	CurrentHP = MaximumHP;
	CurrentATP = MaximumATP;

	// 
	AActor* OwningActor = GetOwner();
	if (OwningActor) {

		OwningActor->OnTakeAnyDamage.AddDynamic(this, &USAttributesComponent::HandleTakeAnyDamage);
		
	}

	// This is hackish as fuck but it works. Basically we only wanna broadcast the value with a bit of delay but doing it in the owning character's
	// BeginPlay ain't doing the trick so fuck it let's do it here and at least have it encapsulated
	GetWorld()->GetTimerManager().SetTimer(InitialBroadcastTimer, this, &USAttributesComponent::BroadcastInitialParameters, 0.01f, false);
	
}


// 
void USAttributesComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType
	, AController* InstigatedBy, AActor* DamageCauser) {

	if (Damage == 0.0f) {

		// do nothing
		// negative damage = healing
	
	}
	else {

		// handle healing/damage intake by guaranteeing the character's health never goes below 0 or surpasses the maximum
		CurrentHP = FMath::Clamp((CurrentHP - Damage), 0.0f, MaximumHP);

		// Calculate actual damage taken/healed after clamping
		float ActualDamage = (CurrentHP - Damage);

		UE_LOG(LogTemp, Log, TEXT("Took %f damage, current HP is %f out of %f maximum"), Damage, CurrentHP, MaximumHP);
		
		HPChangedDelegate.Broadcast(this, CurrentHP, ActualDamage, DamageType, InstigatedBy, DamageCauser);
		
	}
	
}


// Function used to broadcast the initial parameters of the character with some delay so it's done after all necessary bindings have been performed
void USAttributesComponent::BroadcastInitialParameters() {

	CharacterSpawnedDelegate.Broadcast(MaximumHP, MaximumATP);

	// clear the timer just in case
	GetWorld()->GetTimerManager().ClearTimer(InitialBroadcastTimer);
	
}

