// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "OS_PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OnlineShooter/OnlineShooter.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UOS_HealthComponent>(TEXT("HealthComponent"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));


	SpringArmComponent->bUsePawnControlRotation = true;

	SpringArmComponent->SetupAttachment(GetRootComponent());
	CameraComp->SetupAttachment(SpringArmComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;
	// GetCapsuleComponent()->SetCollisionResponseToChannel(WEAPON_COLLISION,ECR_Ignore);
	// GetMesh()->SetCollisionResponseToChannel(WEAPON_COLLISION,ECR_Ignore);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &ASCharacter::OnHealthChanged);

	DefaultFOV = CameraComp->FieldOfView;

	WeaponsSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (GetLocalRole() == ROLE_Authority)
	{
		if (StarterWeapon)
		{
			CurrentWeapon = GetWorld()->SpawnActor<AOSWeapon>(StarterWeapon, FVector::ZeroVector, FRotator::ZeroRotator,
			                                                  WeaponsSpawnParameters);
		}

		AOS_PlayerController* PC = GetController<AOS_PlayerController>();

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                                 WeaponAttachSocketName);
		}
	}
 
	CreateAmmoWidget();

	if (CurrentWeapon && AmmoAmountWidget && IsLocallyControlled())
		AmmoAmountWidget->UpdateWidgetAmount(CurrentWeapon->CurrentBulletAmount,
		                                     CurrentWeapon->MaxBulletAmount);
}

void ASCharacter::CreateAmmoWidget()
{
	if (AmmoAmountWidgetBP && IsLocallyControlled())
	{
		AOS_PlayerController* PlayerController = GetController<AOS_PlayerController>();

		if (PlayerController)
		{
			AmmoAmountWidget = CreateWidget<UOS_AmmoWidget>(PlayerController, AmmoAmountWidgetBP);

			if (AmmoAmountWidget)
			{
				AmmoAmountWidget->AddToPlayerScreen();
			}
		}
	}
}

void ASCharacter::ServerCreateAmmoWidget_Implementation()
{
	CreateAmmoWidget();
}


bool ASCharacter::ServerCreateAmmoWidget_Validate()
{
	return true;
}

void ASCharacter::ChangeFOV()
{
	if (CurrentFov == ZoomedFov)
	{
		GetWorldTimerManager().ClearTimer(ZoomTimerHandle);
	}
	if (bIsAiming)
	{
		float Current = FMath::FInterpTo(CameraComp->FieldOfView, ZoomedFov, ZoomDeltaTime, ZoomInterpSpeed);
		CameraComp->SetFieldOfView(Current);
	}
	else
	{
		float Current = FMath::FInterpTo(CameraComp->FieldOfView, DefaultFOV, ZoomDeltaTime, ZoomInterpSpeed);
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

void ASCharacter::OnHealthChanged(UOS_HealthComponent* HealthComp, float Health, float HealthDelta,
                                  const class UDamageType* DamageType, class AController* InstigatedBy,
                                  AActor* DamageCauser)
{
	if (Health <= 0.0f && bIsAlive)
	{
		bIsAlive = false;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
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

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginEndCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::BeginEndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::ToAim);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::ToAim);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::Fire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::ReloadGun);
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
	if (bIsCrouch == false)
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

void ASCharacter::Fire()
{
	CurrentWeapon->Fire();
}

void ASCharacter::ReloadGun()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->BeginReload();
	}
}


FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bIsAlive);
}
