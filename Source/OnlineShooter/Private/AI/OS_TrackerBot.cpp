#include "AI/OS_TrackerBot.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "SCharacter.h"
#include "asio/detail/timer_queue_set.hpp"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("COOP.DebugTreckerBot"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines And Spheres for TB"),
	ECVF_Cheat);


AOS_TrackerBot::AOS_TrackerBot()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	HealthComp = CreateDefaultSubobject<UOS_HealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChangedDelegate.AddDynamic(this, &AOS_TrackerBot::HandleTakeDamage);

	ExplosionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSphereComp"));
	ExplosionSphereComp->InitSphereRadius(175);
	ExplosionSphereComp->SetupAttachment(GetRootComponent());

	OverlapSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphereComp"));
	OverlapSphereComp->InitSphereRadius(100);
	OverlapSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphereComp->SetupAttachment(GetRootComponent());

	AllyTrackingSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AllyTrackingSphereComp"));
	AllyTrackingSphereComp->InitSphereRadius(325);
	OverlapSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AllyTrackingSphereComp->SetupAttachment(GetRootComponent());
}

void AOS_TrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();

		GetWorldTimerManager().SetTimer(CheckMovingPathTimer, this, &AOS_TrackerBot::HandleMovement,
		                                CheckLocationInterval, true);

		GetWorldTimerManager().SetTimer(TimerHandle_CheckForAllies, this, &AOS_TrackerBot::CheckForAllies,
		                                1.2f, true);
	}
}

void AOS_TrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOS_TrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOS_TrackerBot::HandleMovement()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			NextPathPoint = GetNextPathPoint();

			FVector VectorForceDirection = NextPathPoint - GetActorLocation();
			VectorForceDirection.Normalize();
			VectorForceDirection *= MovementForce;

			FVector CurrentVelocity = MeshComponent->GetComponentVelocity();
			FVector StopForce = -CurrentVelocity;
			MeshComponent->AddForce(StopForce, NAME_None, bUseVelocityChange);

			MeshComponent->AddForce(VectorForceDirection, NAME_None, bUseVelocityChange);
			if (DebugTrackerBotDrawing)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + VectorForceDirection, 32,
				                          FColor::Orange, false, 4, 0, 2.0f);
			}
		}
		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Emerald, false, 4, 2.0f);
	}
}


FVector AOS_TrackerBot::GetNextPathPoint()
{
	float NearestTargetDistance = FLT_MIN;
	AActor* TargetActor = nullptr;

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* CurrentPawn = *It;
		if (CurrentPawn == nullptr || UOS_HealthComponent::IsFriendly(CurrentPawn, this))
		{
			continue;
		}

		UOS_HealthComponent* PawnHealthComp = Cast<UOS_HealthComponent>(
			CurrentPawn->GetComponentByClass(UOS_HealthComponent::StaticClass()));
		if (PawnHealthComp && PawnHealthComp->GetHealth() > 0.0f)
		{
			float Distance = (CurrentPawn->GetActorLocation() - GetActorLocation()).Size();
			if (Distance > NearestTargetDistance)
			{
				TargetActor = CurrentPawn;
				NearestTargetDistance = Distance;
			}
		}
	}

	if (TargetActor)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(
			this, GetActorLocation(), TargetActor);

		GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &AOS_TrackerBot::RefreshPath, 3.0f, false);
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}

	return GetActorLocation();
}

void AOS_TrackerBot::RefreshPath()
{
	NextPathPoint = GetNextPathPoint();
}


void AOS_TrackerBot::HandleTakeDamage(UOS_HealthComponent* HealthComponent, float Health,
                                      float HealthDelta, const class UDamageType* DamageType,
                                      class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!MaterialIns)
	{
		MaterialIns = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}
	if (MaterialIns)
	{
		MaterialIns->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
	UE_LOG(LogTemp, Log, TEXT("Health %f"), Health);

	if (Health <= 0)
	{
		SelfDestruct();
	}
}

void AOS_TrackerBot::SelfDestruct()
{
	if (bIsExploaded == true)
	{
		return;
	}
	if (ExplosionEffect && ExplosionSphereComp)
	{
		bIsExploaded = true;
		float BoostedDamage = Damage * (1 + CurrentPowerLevel / static_cast<float>(MaxPowerLevel));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(this, ExploadSound, GetActorLocation());

		MeshComponent->SetVisibility(false, true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (GetLocalRole() == ROLE_Authority)
		{
			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(this);
			UGameplayStatics::ApplyRadialDamage(this, BoostedDamage, GetActorLocation(),
			                                    ExplosionSphereComp->GetScaledSphereRadius(),
			                                    nullptr, IgnoredActors, this, GetInstigatorController(), true);

			GetWorldTimerManager().ClearTimer(TimerHandle_SelfDamage);
			if (DebugTrackerBotDrawing)
			{
				UE_LOG(LogTemp, Log, TEXT("Damage %f"), BoostedDamage);
				DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionSphereComp->GetScaledSphereRadius(), 24,
				                FColor::Orange,false, 5.0f, 0, 2.0f);
			}

			SetLifeSpan(2.0f);
		}
	}
}

void AOS_TrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bIsStartedSelfDesctruction == false && bIsExploaded == false)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn && !UOS_HealthComponent::IsFriendly(OtherActor, this))
		{
			bIsStartedSelfDesctruction = true;
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, GetRootComponent());

			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AOS_TrackerBot::DamageSelf,
				                                SelfDamageInterval, true);
			}
		}
	}
}

void AOS_TrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 30, GetInstigatorController(), this, nullptr);
}

void AOS_TrackerBot::CheckForAllies()
{
	CurrentPowerLevel = 0;
	TArray<FOverlapResult> OverlapRes;

	FCollisionShape CollShape;
	CollShape.SetSphere(1000);
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	GetWorld()->OverlapMultiByObjectType(OverlapRes, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	for (FOverlapResult Result : OverlapRes)
	{
		// Check if the overlapping actor is a tracker bot.
		AOS_TrackerBot* TrackerBot = Cast<AOS_TrackerBot>(Result.GetActor());
		if (TrackerBot && TrackerBot != this)
		{
			CurrentPowerLevel++;
			CurrentPowerLevel = FMath::Clamp(CurrentPowerLevel, 0, MaxPowerLevel);
		}
	}

	if (!MaterialIns)
	{
		MaterialIns = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}
	if (MaterialIns)
	{
		float AlphaForMaterial = CurrentPowerLevel / static_cast<float>(MaxPowerLevel);
		MaterialIns->SetScalarParameterValue("PowerLevelAlpha", AlphaForMaterial);
	}
}

