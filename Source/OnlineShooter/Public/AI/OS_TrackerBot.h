// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/OS_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "OS_TrackerBot.generated.h"

UCLASS()
class ONLINESHOOTER_API AOS_TrackerBot : public APawn
{
	GENERATED_BODY()

public:

	AOS_TrackerBot();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void HandleMovement();

	UFUNCTION()
	void HandleTakeDamage(UOS_HealthComponent* HealthComponent, float Health,
											 float HealthDelta, const class UDamageType* DamageType,
											 class AController* InstigatedBy, AActor* DamageCauser);
	void SelfDestruct();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	UFUNCTION()
	void DamageSelf();

	UFUNCTION()
	void CheckForAllies();
	// UFUNCTION(Server, Reliable,WithValidation)
	// void ServerHandleMovement(FVector VectorForceDirection);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void MulticastHandleMovement(FVector VectorForceDirection);
private:

	FVector GetNextPathPoint();

	UFUNCTION()
	void RefreshPath();
public:

	UPROPERTY(VisibleDefaultsOnly,Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly,Category = "Components")
	UOS_HealthComponent* HealthComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USphereComponent* OverlapSphereComp = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USphereComponent* ExplosionSphereComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USphereComponent* AllyTrackingSphereComp = nullptr;

public:

	UPROPERTY(EditDefaultsOnly,Category = "TrackerBot")
	USoundCue* SelfDestructSound = nullptr;

	UPROPERTY(EditDefaultsOnly,Category = "TrackerBot")
	USoundCue* ExploadSound = nullptr;
	
protected:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "TrackerBot")
	float Damage = 100;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamageInterval = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange = false;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot", meta = (ClampMin = 30, ClampMax = 100))
	int FrequencyOfMoveCheckingPerSec = 100;

	UPROPERTY(EditDefaultsOnly,Category = "TrackerBot", meta = (ClampMin = 0, ClampMax = 7))
	int MaxPowerLevel = 7;

	UPROPERTY(BlueprintReadOnly)
	int CurrentPowerLevel = 0;

private:

	bool bIsExploaded = false;

	bool bIsStartedSelfDesctruction = false;
	
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialIns = nullptr;
	
	FTimerHandle CheckMovingPathTimer;

	FTimerHandle TimerHandle_SelfDamage;

	FTimerHandle TimerHandle_CheckForAllies;

	FTimerHandle  TimerHandle_RefreshPath;
	FVector NextPathPoint;
	
	float CheckLocationInterval = 0.06f;
};
