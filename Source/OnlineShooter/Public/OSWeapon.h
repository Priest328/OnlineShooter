// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OSWeapon.generated.h"

UCLASS()
class ONLINESHOOTER_API AOSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOSWeapon();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual void Fire();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapons")
	USkeletalMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* MuzzleEffect = nullptr;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* ImpactEffect = nullptr;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* TraceEffect = nullptr;
};
