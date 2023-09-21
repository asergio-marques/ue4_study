// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetDummy.generated.h"



class USAttributesComponent;



/*
 *
 * This class is merely a dummy class so we can cast to it when a damaging actor hits it and therefore only make it explode
 * when it hits a damageable actor
 * 
 */
UCLASS()
class COOPGAME_API ATargetDummy : public AActor {
	GENERATED_BODY()
	
	
public:	
	// Sets default values for this actor's properties
	ATargetDummy();

	// Component that handles everything related to attributes (calculation of damage, health management...)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributesComponent* StatsComp;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};