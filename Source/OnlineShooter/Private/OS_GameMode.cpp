#include "OS_GameMode.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "AI/OS_TrackerBot.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

AOS_GameMode::AOS_GameMode()
{
	// if (PlayerController)
	// {
	// 	PlayerControllerClass = PlayerController->StaticClass();
	// }
}

void AOS_GameMode::CheckGameState()
{
	CheckWaveState();
	CheckAnyPlayerAlive();
}

void AOS_GameMode::SetWaveState(EWaveState NewState)
{
	AOS_GameState* GS = GetGameState<AOS_GameState>();
	if (ensure(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void AOS_GameMode::RestartAllPlayers()
{
	for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
	{
		APlayerController* PC = *It;
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void AOS_GameMode::StartPlay()
{
	Super::StartPlay();

	PrepareToNextWave();

	GetWorldTimerManager().SetTimer(TimerHandle_CheckGameState, this, &AOS_GameMode::CheckGameState,
	                                1.25f, true, 6.0f);
}

void AOS_GameMode::StartWave()
{
	WaveCount++;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AOS_GameMode::SpawnBotTimerElapsed,
	                                IntervalOfSpawningBots, true, -1.0f);
	SetWaveState(EWaveState::WaveInProgress);
}

void AOS_GameMode::EndWave()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_BotSpawner))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	}

	if (bIsGameOver)
	{
		SetWaveState(EWaveState::WaitingToComplete);
	}

	if (NumOfBotsToSpawn == 0)
	{
		GameOver();
		return;
	}

	NumOfBotsToSpawn = NumOfSpawnedBots * 1.5;
	NumOfSpawnedBots = 0;
}

void AOS_GameMode::PrepareToNextWave()
{
	SetWaveState(EWaveState::WaitingToStart);
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AOS_GameMode::StartWave, TimeBetweenWaves, false);
	RestartAllPlayers();
}

void AOS_GameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumOfSpawnedBots++;

	if (NumOfBotsToSpawn == NumOfSpawnedBots)
	{
		EndWave();
	}
}

void AOS_GameMode::CheckAnyPlayerAlive()
{
	// Assuming bIsGameOver is initially false
	if (!bIsGameOver)
	{
		for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
		{
			APlayerController* PC = *It;
			if (PlayerController)
			{
				APawn* PC_Pawn = PC->GetPawn();
				if (PC_Pawn)
				{
					UOS_HealthComponent* HealthComp = Cast<UOS_HealthComponent>(
						PC_Pawn->GetComponentByClass(UOS_HealthComponent::StaticClass()));
					if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
					{
						return;
					}
				}
			}
		}
		bIsGameOver = true;
		GameOver();
	}
}

void AOS_GameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);
	UE_LOG(LogTemp, Log, TEXT("GAME OVER"));
}

void AOS_GameMode::CheckWaveState()
{
	bool bIsPreparingToWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);
	bool bIsAnyBotAlive = false;
	
	if (NumOfBotsToSpawn == NumOfSpawnedBots || bIsPreparingToWave)
	{
		return;
	}
	// Use TActorIterator to iterate over all actors in the world
	
	
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* CurrentPawn = *It;
		if (CurrentPawn == nullptr || CurrentPawn->IsPlayerControlled())
		{
			continue;
		}
		if (AOS_TrackerBot* TrackerBot = Cast<AOS_TrackerBot>(CurrentPawn))
		{
			if (TrackerBot->HealthComp && TrackerBot->HealthComp->GetHealth() > 0.0f)
			{
				bIsAnyBotAlive = true;
				break;
			}
		}
		if (ASCharacter* AdvancedBot = Cast<ASCharacter>(CurrentPawn))
		{
			if (!AdvancedBot->IsPlayerControlled())
			{
				if (AdvancedBot->HealthComponent && AdvancedBot->HealthComponent->GetHealth() > 0.0f)
				{
					bIsAnyBotAlive = true;
					break;
				}
			}
		}
	}
	if (bIsAnyBotAlive == false)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareToNextWave();
	}
}
