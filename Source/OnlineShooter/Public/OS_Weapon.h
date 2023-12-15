// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "LegacyCameraShake.h"
#include "OS_AmmoWidget.h"
#include "GameFramework/Actor.h"
#include "OS_Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReloadDelegate, bool, bReloadInProgress);


USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
	
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};


UCLASS()
class ONLINESHOOTER_API AOSWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOSWeapon();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ToShoot")
	virtual void Fire();

	UFUNCTION()
	void Reload();
	
	void BeginReload();

	UFUNCTION()
	void OnRep_HitScanTrace();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ToShoot")
	void SingleFire();

	UFUNCTION(BlueprintCallable, Category = "ToShoot")
	void AutomaticFire();

	UFUNCTION()
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmoAmount();
private:
	void PlayFireEffect(FVector TraceEnd);

	void EnableFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

public:



	UPROPERTY(BlueprintAssignable, Category = "Reload")
	FReloadDelegate OnReloadStatusChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ToShoot", meta = (ClampMin = 1, ClampMax = 45))
	float ShotsPerSecond = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ToShoot", meta = (ClampMin = 0, ClampMax = 25))
	float DelayBetweenSingleShots = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ToShoot")
	int32 MaxBulletAmount = 1.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ToShoot")
	float ReloadTime = 1.0f;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "ToShoot")
	int32 CurrentBulletAmount = 5.0;
	
	// Bullet spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 0.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	USkeletalMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	TSubclassOf<ULegacyCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* MuzzleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* DefaultImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* FleshImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* ValnurableFleshImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* TraceEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ToShoot")
	bool bIsProvideAutomaticFire = true;

	
protected:
	FTimerHandle TimerHandle_AutomaticFire;
	FTimerHandle TimerHandle_GapBetweenSingleShots;
	FTimerHandle TimerHandle_ToReload;


private:

	//UPROPERTY(Replicated)
	bool bIsInReload = false;
	
	bool bIsSingleFireMode = false;
	bool bIsInAutoFire = false;
	bool bIsSingleShot = true;
	bool bIsCanFire = true;
};


