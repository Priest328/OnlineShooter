// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->bUsePawnControlRotation = true;
    //Weapon = CreateDefaultSubobject<AOSWeapon>(TEXT("Weapon"));
	
	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComp->SetupAttachment(SpringArmComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

}

void ASCharacter::ChangeFOV()
{
	if (CurrentFov == ZoomedFov)
	{
		GetWorldTimerManager().ClearTimer(ZoomTimerHandle);
	}
	if (bIsAiming)
	{
		float Current = FMath::FInterpTo(CameraComp->FieldOfView,ZoomedFov,ZoomDeltaTime,ZoomInterpSpeed);
		CameraComp->SetFieldOfView(Current);
	}
	else
	{
		float Current = FMath::FInterpTo(CameraComp->FieldOfView,DefaultFOV,ZoomDeltaTime,ZoomInterpSpeed);
		CameraComp->SetFieldOfView(Current);
	}
	
	
}

void ASCharacter::ToAim()
{
	float TargetFOV = 0.0f;
	if (!bIsAiming)
	{
		bIsAiming = true;
		GetWorldTimerManager().SetTimer(ZoomTimerHandle, this, &ASCharacter::ChangeFOV, ZoomDeltaTime, true);
	}
	else
	{
		bIsAiming = false;
		GetWorldTimerManager().SetTimer(ZoomTimerHandle, this, &ASCharacter::ChangeFOV, ZoomDeltaTime, true);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
    PlayerInputComponent->BindAxis("MoveForward",this,&ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp",this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn",this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed,this, &ASCharacter::BeginEndCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released,this, &ASCharacter::BeginEndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed,this, &ASCharacter::ToAim);
	PlayerInputComponent->BindAction("Zoom", IE_Released,this, &ASCharacter::ToAim);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}
    void ASCharacter::BeginEndCrouch()
    	{
		if(bIsCrouch == false)
		{
			Crouch();
			bIsCrouch = true;
		}
		else
		{
			UnCrouch();
			bIsCrouch = false;
		}
	}

void ASCharacter::BeginJump()
{
	Jump();
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if(CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	
	return Super::GetPawnViewLocation();
	
}

