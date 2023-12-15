// Fill out your copyright notice in the Description page of Project Settings.


#include "OS_ExplosiveBarrel.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AOS_ExplosiveBarrel::AOS_ExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UOS_HealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &AOS_ExplosiveBarrel::OnHealthChanged);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetCanEverAffectNavigation(false);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetCollisionProfileName(TEXT("OverlapAll"));

	SphereComp->SetupAttachment(MeshComponent);
	SetRootComponent(MeshComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->bIgnoreOwningActor = true;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AOS_ExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	RadialForceComponent->Radius = SphereComp->GetUnscaledSphereRadius();
}

void AOS_ExplosiveBarrel::OnHealthChanged(UOS_HealthComponent* HealthComp, float Health, float HealthDelta,
                                          const UDamageType* DamageType, AController* InstigatedBy,
                                          AActor* DamageCauser)
{
	if (bIsExploaded)
	{
		return;
	}
	if (Health <= 0.0f)
	{
		bIsExploaded = true;
		OnRep_Exploaded();

		TArray<AActor*> OverlappingActors;
		SphereComp->GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			// Calculate damage based on proximity to the epicenter
			FVector ActorLocation = OverlappingActor->GetActorLocation();
			float DistanceToEpicenter = (ActorLocation - GetActorLocation()).Size();
			float Damage = FMath::Lerp(MinDamage, MaxDamage, FMath::Clamp(1 - (DistanceToEpicenter / SphereComp->GetUnscaledSphereRadius()), 0.0f, 1.0f));
            FHitResult HitResult;
			// Apply damage to the overlapping actor
			UGameplayStatics::ApplyPointDamage(OverlappingActor, Damage, GetActorLocation(),HitResult ,DamageCauser->GetInstigatorController(), this, BarrelDamageType);
		}
		
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComponent->AddImpulse(BoostIntensity,NAME_None,true);

		RadialForceComponent->FireImpulse();
	}
}

void AOS_ExplosiveBarrel::OnRep_Exploaded()
{
	if (ExplosionEffect && ExploadedMaterial)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplosionEffect,GetActorLocation());
		MeshComponent->SetMaterial(0,ExploadedMaterial);
	}
}

void AOS_ExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOS_ExplosiveBarrel, bIsExploaded);

}