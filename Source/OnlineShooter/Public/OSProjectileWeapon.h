// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OSWeapon.h"
#include "OSProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESHOOTER_API AOSProjectileWeapon : public AOSWeapon
{
	GENERATED_BODY()
	
public:
	
    virtual void Fire() override;


protected:

	UPROPERTY(EditDefaultsOnly,Category = "Projectile")
	TSubclassOf<AActor> Projectile = nullptr;
};
