// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(BULLETTIME, Log, All);

// 유틸리티 매크로
#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define PRINT_CALLINFO() UE_LOG(BULLETTIME, Warning, TEXT("%s"), *CALLINFO)

#define PRINT_LOG(fmt, ...) UE_LOG(BULLETTIME, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
