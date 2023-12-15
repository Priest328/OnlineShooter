// Fill out your copyright notice in the Description page of Project Settings.


#include "OS_AmmoWidget.h"

#include "Components/TextBlock.h"

void UOS_AmmoWidget::UpdateWidgetAmount(int32 CurrentAmmoAmount, int32 MaxAmmoAmount)
{
	if (CurrentAmmoAmount_TextBlock && MaxAmmoAmount_TextBlock)
	{
		CurrentAmmoAmount_TextBlock->SetText(FText::FromString(FString::FromInt(CurrentAmmoAmount)));
		MaxAmmoAmount_TextBlock->SetText(FText::FromString(FString::FromInt(MaxAmmoAmount)));
	}
}
