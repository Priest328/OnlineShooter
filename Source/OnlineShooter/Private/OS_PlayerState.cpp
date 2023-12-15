// Fill out your copyright notice in the Description page of Project Settings.


#include "OS_PlayerState.h"

void AOS_PlayerState::AddScore(float ScoreDelta)
{
	ScoreDelta += GetScore();
	SetScore(ScoreDelta);
}
