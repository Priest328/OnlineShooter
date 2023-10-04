// Fill out your copyright notice in the Description page of Project Settings.


#include "OSWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AOSWeapon::AOSWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void AOSWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOSWeapon::Fire()
{
	FHitResult HitRes;

	AActor* OwnerActor = GetOwner();


	if (OwnerActor)
	{
		FVector EyeLocation;
		FRotator EyeRotation;


		OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector EndLocation = EyeLocation + (EyeRotation.Vector() * 5000);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerActor);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		FVector TraceEndPoint = EndLocation;
		
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitRes, EyeLocation, EndLocation, ECollisionChannel::ECC_Visibility,
		                                         QueryParams))
		{
			AActor* HitActor = HitRes.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 35.0f, EyeRotation.Vector(), HitRes,
			                                   OwnerActor->GetInstigatorController(), this, DamageType);
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitRes.ImpactPoint,
				                                         HitRes.ImpactNormal.Rotation());
			}
			TraceEndPoint = HitRes.ImpactPoint;
		}
		DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
		if (TraceEffect)
		{
			if (UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect,
				MeshComp->GetSocketLocation(MuzzleSocketName)))
			{
				TraceComp->SetVectorParameter(TracerTargetName, TraceEndPoint );
			}
		}
	}
}
