// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\OS_Weapon.h"

#include "SCharacter.h"
#include "Animation/EditorAnimCompositeSegment.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineShooter/OnlineShooter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"


AOSWeapon::AOSWeapon()
{

	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void AOSWeapon::BeginPlay()
{
	Super::BeginPlay();
	ASCharacter* OwnerCharacter = Cast<ASCharacter>(GetOwner());
	if (OwnerCharacter && OwnerCharacter->AmmoAmountWidget)
	{
		OwnerCharacter->AmmoAmountWidget->UpdateWidgetAmount(CurrentBulletAmount, MaxBulletAmount);
	}
}


// Called every frame
void AOSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOSWeapon::SingleFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	if (CurrentBulletAmount == 0)
	{
		BeginReload();
		bIsInAutoFire = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_AutomaticFire);
		return;
	}

	FHitResult HitRes;

	ASCharacter* OwnerCharacter = Cast<ASCharacter>(GetOwner());


	if (OwnerCharacter && bIsInReload == false)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		float HalfRad = FMath::DegreesToRadians(BulletSpread);

		OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Apply random spread to the view direction
		FVector ViewDirection = FMath::VRandCone(EyeRotation.Vector(), HalfRad, HalfRad);

		// Calculate the end point of the trace
		FVector TraceEndPoint = EyeLocation + (ViewDirection * 5000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerCharacter);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		if (GetWorld()->LineTraceSingleByChannel(HitRes, EyeLocation, TraceEndPoint, WEAPON_COLLISION, QueryParams))
		{
			AActor* HitActor = HitRes.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitRes.PhysMaterial.Get());

			if (HitActor)
			{
				switch (SurfaceType)
				{
				case SURFACE_BODY_IMPACT:
				case SURFACE_LIMBS_IMPACT:
					Damage = 35.0f;
					break;
				case SURFACE_HEAD_IMPACT:
					Damage = 100.0f;
					break;
				default:
					Damage = 35.0f;
					break;
				}
			}

			UGameplayStatics::ApplyPointDamage(HitActor, Damage, ViewDirection, HitRes,
			                                   OwnerCharacter->GetInstigatorController(), OwnerCharacter, DamageType);

			CurrentBulletAmount -= 1;

			if (OwnerCharacter && OwnerCharacter->AmmoAmountWidget)
			{
				OwnerCharacter->AmmoAmountWidget->UpdateWidgetAmount(CurrentBulletAmount, MaxBulletAmount);
			}

			PlayImpactEffect(SurfaceType, HitRes.ImpactPoint);
			TraceEndPoint = HitRes.ImpactPoint;
		}

		PlayFireEffect(TraceEndPoint);

		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		//DrawDebugLine(GetWorld(), EyeLocation, TraceEndPoint, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

void AOSWeapon::AutomaticFire()
{
	float interval = 1 / ShotsPerSecond;

	if (bIsInAutoFire == false)
	{
		bIsInAutoFire = true;
		GetWorldTimerManager().SetTimer(TimerHandle_AutomaticFire, this, &AOSWeapon::SingleFire, interval, true);
	}
	else
	{
		bIsInAutoFire = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_AutomaticFire);
	}
}

void AOSWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	// Apply damage
	if (DefaultImpactEffect && FleshImpactEffect && ValnurableFleshImpactEffect)
	{
		switch (SurfaceType)
		{
		case SURFACE_BODY_IMPACT:
		case SURFACE_LIMBS_IMPACT:
			SelectedEffect = FleshImpactEffect;
			break;
		case SURFACE_HEAD_IMPACT:
			SelectedEffect = ValnurableFleshImpactEffect;
			break;
		default:
			SelectedEffect = DefaultImpactEffect;
			break;
		}
	}
	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint,
		                                         ShotDirection.Rotation());
	}
}

int32 AOSWeapon::GetMaxAmmoAmount()
{
	return MaxBulletAmount;
}


void AOSWeapon::Fire()
{
	if (bIsProvideAutomaticFire == true && bIsSingleFireMode == false && bIsCanFire == true)
	{
		AutomaticFire();
	}
	else
	{
		if (bIsSingleShot == true && bIsCanFire == true)
		{
			SingleFire();
			bIsSingleShot = false;
			bIsCanFire = false;
			GetWorldTimerManager().SetTimer(TimerHandle_GapBetweenSingleShots, this, &AOSWeapon::EnableFire,
			                                DelayBetweenSingleShots, false);
		}
		else
		{
			bIsSingleShot = true;
		}
	}
}

void AOSWeapon::Reload()
{
	CurrentBulletAmount = MaxBulletAmount;
	bIsCanFire = true;
	bIsInReload = false;
	
	ASCharacter* OwnerCharacter = Cast<ASCharacter>(GetOwner());
	
	if (OwnerCharacter && OwnerCharacter->AmmoAmountWidget)
	{
		OwnerCharacter->AmmoAmountWidget->UpdateWidgetAmount(CurrentBulletAmount, MaxBulletAmount);
	}
	
	GetWorldTimerManager().ClearTimer(TimerHandle_ToReload);
}


void AOSWeapon::BeginReload()
{
	if (CurrentBulletAmount < MaxBulletAmount && bIsInReload == false)
	{
		//OnReloadStatusChanged.Broadcast(true);
		bIsCanFire = false;
		bIsInReload = true;
		GetWorldTimerManager().SetTimer(TimerHandle_ToReload, this, &AOSWeapon::Reload,
		                                ReloadTime, false);
	}
}

void AOSWeapon::PlayFireEffect(FVector TraceEnd)
{
	APawn* WeaponOwner = Cast<APawn>(GetOwner());


	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (TraceEffect)
	{
		if (UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect,
			MeshComp->GetSocketLocation(MuzzleSocketName)))
		{
			TraceComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	if (WeaponOwner)
	{
		APlayerController* PlayerController = Cast<APlayerController>(WeaponOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientStartCameraShake(FireCamShake);
		}
	}
}

void AOSWeapon::EnableFire()
{
	bIsCanFire = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_GapBetweenSingleShots);
}

void AOSWeapon::ServerFire_Implementation()
{
	SingleFire();
}

bool AOSWeapon::ServerFire_Validate()
{
	// if (CurrentBulletAmount > MaxBulletAmount)
	// {
	// 	return true;
	// }
	// return false;
	return true;
}

void AOSWeapon::OnRep_HitScanTrace()
{
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void AOSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AOSWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(AOSWeapon, CurrentBulletAmount);
}
