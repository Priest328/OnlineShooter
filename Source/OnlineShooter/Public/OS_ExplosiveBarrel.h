// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/OS_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "OS_ExplosiveBarrel.generated.h"

UCLASS()
class ONLINESHOOTER_API AOS_ExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOS_ExplosiveBarrel();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(UOS_HealthComponent* HealthComp, float Health, float HealthDelta,
	                     const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UFUNCTION()
	void OnRep_Exploaded();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UOS_HealthComponent* HealthComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp = nullptr;

public:
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float ExplosionImpulse = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* ExploadedMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 0.0f, ClampMax = 500.0f))
	float MaxDamage = 0.0;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float MinDamage = 0.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<UDamageType> BarrelDamageType;
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Exploaded)
	bool bIsExploaded = false;
};
