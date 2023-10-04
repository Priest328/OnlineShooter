#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class ONLINESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ToExpload();

	UFUNCTION()
	void ToExploadWithDelay();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   FVector NormalImpulse, const FHitResult& Hit);
public:
	// UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	// UMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComp = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	UParticleSystem* ExplosionEffect = nullptr;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Granade Projectile")
	UProjectileMovementComponent *ProjectileMovementComp = nullptr;
	
public:

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Granade Projectile")
	float DamageRadius = 20.0f;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Granade Projectile")
	float DamageAmount = 45.0f;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Granade Projectile")
	float RateToCheckExplosion = 0.25f;
	
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category = "Granade Projectile")
	float DelayToExplosion = 1.0f;

private:

	FTimerHandle DelayTimer;
};
