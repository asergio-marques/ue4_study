// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributesComponent.generated.h"


// Declaration of delegate type for broadcast of initial parameters
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSpawnSignature, float, MaximumHP, float, MaximumATP);

// Declaration of delegate type for broadcast of HP change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHPChangedSignature, class USAttributesComponent*, AttrComp, float, CurrentHP, float, DeltaHP, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);



UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USAttributesComponent : public UActorComponent {

	GENERATED_BODY()

	
public:	
	// Sets default values for this component's properties
	USAttributesComponent();
	
	// Delegate to broadcast the initial parameters/attributes of a character
	// NOTE: Should be called in PostInitializeComponents of the owning character
	FOnCharacterSpawnSignature CharacterSpawnedDelegate;
	
	// Delegate to broadcast a change in the current HP of the owning actor
	FOnHPChangedSignature HPChangedDelegate;

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	//
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser); 

	// Variable that represents the current HP of the the owning actor
	UPROPERTY(BlueprintReadOnly, Category = "Live Attributes")
	float CurrentHP;
	
	// Variable that represents the maximum HP the owning actor may have
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base Attributes | Basic")
	float MaximumHP;

	// Variable that represents the current ATP of the the owning actor
	UPROPERTY(BlueprintReadOnly, Category = "Live Attributes")
	float CurrentATP;
	
	// Variable that represents the maximum ATP the owning actor may have
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base Attributes | Basic")
	float MaximumATP;

	
private:
	// Function used to broadcast the initial parameters of the character with some delay so it's done after all necessary bindings have been performed
	void BroadcastInitialParameters();

	// Timer for the broadcast of the starting parameters
	FTimerHandle InitialBroadcastTimer;
};
