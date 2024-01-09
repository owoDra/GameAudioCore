// Copyright (C) 2024 owoDra

#pragma once

#include "AudioMixTypes.generated.h"

class USoundSubmix;
class USoundEffectSubmixPreset;


USTRUCT()
struct FAudioSubmixEffectsChain
{
	GENERATED_BODY()
public:
	FAudioSubmixEffectsChain() {}

public:
	//
	// Submix on which to apply the Submix Effect Chain Override
	//
	UPROPERTY(Transient)
	TObjectPtr<USoundSubmix> Submix{ nullptr };

	//
	// Submix Effect Chain Override (Effects processed in Array index order)
	//
	UPROPERTY(Transient)
	TArray<TObjectPtr<USoundEffectSubmixPreset>> SubmixEffectChain;

};


USTRUCT()
struct GACORE_API FAudioSubmixEffectChainMap
{
	GENERATED_BODY()
public:
	FAudioSubmixEffectChainMap() {}

public:
	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/Engine.SoundSubmix"))
	TSoftObjectPtr<USoundSubmix> Submix{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/Engine.SoundEffectSubmixPreset"))
	TArray<TSoftObjectPtr<USoundEffectSubmixPreset>> SubmixEffectChain;

};