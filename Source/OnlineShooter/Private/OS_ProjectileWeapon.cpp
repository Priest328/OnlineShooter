// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\OS_ProjectileWeapon.h"

#include "Projects.h"

void AOSProjectileWeapon::Fire()
{
	AActor* OwnerActor = GetOwner();


	if (OwnerActor && Projectile)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Projectile)
		{
			GetWorld()->SpawnActor<AActor>(Projectile, MuzzleLocation, EyeRotation, SpawnParameters);
		}
	}
}
