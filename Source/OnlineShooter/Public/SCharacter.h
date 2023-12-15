// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OS_AmmoWidget.h"
#include "OS_PlayerController.h"
#include "OS_Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/OS_HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "SCharacter.generated.h"

UCLASS()
class ONLINESHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Movement
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void BeginEndCrouch();

	UFUNCTION()
	void BeginJump();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Fire();

	UFUNCTION()
	void ReloadGun();

	
protected:
	virtual void BeginPlay() override;

	void CreateAmmoWidget();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCreateAmmoWidget();

	
private:
	void ChangeFOV();

public:
	UFUNCTION()
	void ToAim();

	UFUNCTION()
	void OnHealthChanged(UOS_HealthComponent* HealthComp, float Health, float HealthDelta,
	                     const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArmComp")
	UCameraComponent* CameraComp = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "SpringArmComp")
	USpringArmComponent* SpringArmComponent = nullptr;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UOS_AmmoWidget> AmmoAmountWidgetBP = nullptr;

	UPROPERTY()
	UOS_AmmoWidget* AmmoAmountWidget = nullptr;

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite)
	AOSWeapon* CurrentWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AOSWeapon> StarterWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	UOS_HealthComponent* HealthComponent = nullptr;

	
private:
	FActorSpawnParameters WeaponsSpawnParameters;
	
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapons")
	FName WeaponAttachSocketName = TEXT("hand_WeaponSocket");

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom")
	float ZoomedFov = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsAlive = true;

private:
	bool bIsCrouch = false;

	bool bIsAiming = false;

	float DefaultFOV = 0.0f;
	float CurrentFov = 0.0f;
	float ZoomDeltaTime = 0.03;

	FVector GetPawnViewLocation() const override;

	FTimerHandle ZoomTimerHandle;
};
