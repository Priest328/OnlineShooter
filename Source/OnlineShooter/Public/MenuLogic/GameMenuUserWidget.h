// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuUserWidget.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;

UCLASS()
class ONLINESHOOTER_API UGameMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeGameButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitToMainMenuButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResumeGameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuitToMainMenuTextBlock;

};
