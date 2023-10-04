// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSWeapon.h"
#include "Camera/CameraComponent.h"
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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void BeginEndCrouch();

	UFUNCTION()
	void BeginJump();



protected:
	virtual void BeginPlay() override;

private:

	void ChangeFOV();
public:

	UFUNCTION()
	void ToAim();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringArmComp")
	UCameraComponent* CameraComp = nullptr;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, Category = "SpringArmComp")
	USpringArmComponent* SpringArmComponent = nullptr;

public:

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Zoom")
	float ZoomedFov = 0.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Zoom", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed = 0.0f;
private:
	bool bIsCrouch= false;

	bool bIsAiming = false;

	float DefaultFOV = 0.0f;
    float CurrentFov = 0.0f;
	float ZoomDeltaTime = 0.03;
	
	FVector GetPawnViewLocation() const override;

	FTimerHandle ZoomTimerHandle;
};
