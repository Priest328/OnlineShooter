// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuLogic/MenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	ShowMainMenu();

	if (UserMainMenuWidget)
	{
		MainMenuWidget = CreateWidget<UMainMenuUserWidget>(GetWorld(), UserMainMenuWidget);

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();

			MainMenuWidget->StartGameButton->OnClicked.AddDynamic(this, &AMenuGameMode::StartGame);
			MainMenuWidget->QuitGameButton->OnClicked.AddDynamic(this, &AMenuGameMode::QuitGame);
		}
	}
}

void AMenuGameMode::ShowMainMenu()
{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;
			PlayerController->bEnableClickEvents = true;
			PlayerController->bEnableMouseOverEvents = true;
		}
}

void AMenuGameMode::QuitGame()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		PlayerController->ConsoleCommand("quit");
	}
	
}

void AMenuGameMode::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Level1");
}

