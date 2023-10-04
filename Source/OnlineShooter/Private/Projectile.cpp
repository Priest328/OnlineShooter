// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	SetRootComponent(MeshComp);
	
	ProjectileMovementComp->UpdatedComponent = GetRootComponent();
	SphereComp->SetupAttachment(MeshComp);
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	MeshComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	//StaticMeshComp->SetupAttachment(MeshComp);
	//StaticMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DelayTimer, this, &AProjectile::ToExploadWithDelay, RateToCheckExplosion, true);
}

void AProjectile::ToExpload()
{
	FVector SphereLocation = MeshComp->GetComponentLocation();
	FCollisionQueryParams CollisionParams;

	ECollisionChannel CollisionChannel_1 = ECC_Pawn;
	FCollisionObjectQueryParams ObjectParams(ECC_Pawn);

	TArray<FHitResult> HitResults;

	bool bHit = GetWorld()->SweepMultiByChannel(HitResults,
	                                            SphereLocation, SphereLocation, FQuat::Identity,
	                                            CollisionChannel_1, SphereComp->GetCollisionShape(), CollisionParams);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (HitResult.GetActor() != nullptr && HitResult.GetActor()->CanBeDamaged())
			{
				UE_LOG(LogTemp, Warning, TEXT("Collision is detected"));
				if (ExplosionEffect)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect,
					                                         MeshComp->GetComponentLocation(), FRotator::ZeroRotator);
					Destroy();
				}
			}
		}
	}
}

void AProjectile::ToExploadWithDelay()
{
	DelayToExplosion -= RateToCheckExplosion;
	if (DelayToExplosion <= 0)
	{
		ToExpload();
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	ToExpload();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
