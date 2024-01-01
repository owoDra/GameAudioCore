// Copyright (C) 2023 owoDra


#include "AudioMixSubsystem.h"

#include "AudioDeveloperSettings.h"
#include "GameplayTag/GACTags_Audio.h"

#include "AudioModulationStatics.h"
#include "AudioMixerBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AudioMixSubsystem)


bool UAudioMixSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	//  Do not create this class if a derived class exists

	TArray<UClass*> ChildClasses;
	GetDerivedClasses(GetClass(), ChildClasses, false);

	if (ChildClasses.Num() > 0)
	{
		return false;
	}

	// Check for support world types

	if (const auto* World{ Outer ? Outer->GetWorld() : nullptr })
	{
		return DoesSupportWorldType(World->WorldType);
	}

	return false;
}

void UAudioMixSubsystem::PostInitialize()
{
	const auto* DevSettings{ GetDefault<UAudioDeveloperSettings>() };

	// Load Control Bus Mixies

	for (const auto& KVP : DevSettings->ControlBusMixMap)
	{
		if (auto* ObjPath{ KVP.Value.TryLoad() })
		{
			if (auto* SoundControlBusMix{ Cast<USoundControlBusMix>(ObjPath) })
			{
				ControlBusMixMap.Add(KVP.Key, SoundControlBusMix);
			}
			else
			{
				ensureMsgf(SoundControlBusMix, TEXT("[%s] Control Bus Mix reference missing from AudioDeveloperSettings."), *KVP.Key.GetTagName().ToString());
			}
		}
	}

	// Load Control Buses

	for (const auto& KVP : DevSettings->ControlBusMap)
	{
		if (auto* ObjPath{ KVP.Value.TryLoad() })
		{
			if (auto* SoundControlBusMix{ Cast<USoundControlBus>(ObjPath) })
			{
				ControlBusMap.Add(KVP.Key, SoundControlBusMix);
			}
			else
			{
				ensureMsgf(SoundControlBusMix, TEXT("[%s] Control Bus reference missing from AudioDeveloperSettings."), *KVP.Key.GetTagName().ToString());
			}
		}
	}

	// Load HDR Submix Effect Chain

	for (const auto& SoftSubmixEffectChain : DevSettings->HDRAudioSubmixEffectChain)
	{
		FAudioSubmixEffectsChain NewEffectChain;

		if (auto* SubmixObjPath{ SoftSubmixEffectChain.Submix.LoadSynchronous() })
		{
			if (auto* Submix{ Cast<USoundSubmix>(SubmixObjPath) })
			{
				NewEffectChain.Submix = Submix;
				TArray<USoundEffectSubmixPreset*> NewPresetChain;

				for (const auto& SoftEffect : SoftSubmixEffectChain.SubmixEffectChain)
				{
					if (auto* EffectObjPath{ SoftEffect.LoadSynchronous() })
					{
						if (auto* SubmixPreset{ Cast<USoundEffectSubmixPreset>(EffectObjPath) })
						{
							NewPresetChain.Add(SubmixPreset);
						}
					}
				}

				NewEffectChain.SubmixEffectChain.Append(NewPresetChain);
			}
		}

		HDRSubmixEffectChain.Add(NewEffectChain);
	}

	// Load LDR Submix Effect Chain

	for (const auto& SoftSubmixEffectChain : DevSettings->LDRAudioSubmixEffectChain)
	{
		FAudioSubmixEffectsChain NewEffectChain;

		if (auto* SubmixObjPath{ SoftSubmixEffectChain.Submix.LoadSynchronous() })
		{
			if (auto* Submix{ Cast<USoundSubmix>(SubmixObjPath) })
			{
				NewEffectChain.Submix = Submix;
				TArray<USoundEffectSubmixPreset*> NewPresetChain;

				for (const auto& SoftEffect : SoftSubmixEffectChain.SubmixEffectChain)
				{
					if (auto* EffectObjPath{ SoftEffect.LoadSynchronous() })
					{
						if (auto* SubmixPreset{ Cast<USoundEffectSubmixPreset>(EffectObjPath) })
						{
							NewPresetChain.Add(SubmixPreset);
						}
					}
				}

				NewEffectChain.SubmixEffectChain.Append(NewPresetChain);
			}
		}

		LDRSubmixEffectChain.Add(NewEffectChain);
	}
}

void UAudioMixSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	if (const auto * World{ InWorld.GetWorld() })
	{
		ApplyControlBusMix(TAG_Audio_ControlBusMix_DefaultBase);
		ApplyControlBusMix(TAG_Audio_ControlBusMix_User);
	}
}

bool UAudioMixSubsystem::DoesSupportWorldType(const EWorldType::Type World) const
{
	// We only need this subsystem on Game worlds (PIE included)

	return (World == EWorldType::Game || World == EWorldType::PIE);
}


void UAudioMixSubsystem::ApplyDynamicRangeEffectsChains(bool bHDRAudio)
{
	TArray<FAudioSubmixEffectsChain> AudioSubmixEffectsChainToApply;
	TArray<FAudioSubmixEffectsChain> AudioSubmixEffectsChainToClear;

	// If HDR Audio is selected, then we clear out any existing LDR Submix Effect Chain Overrides
	// otherwise the reverse is the case.

	if (bHDRAudio)
	{
		AudioSubmixEffectsChainToApply.Append(HDRSubmixEffectChain);
		AudioSubmixEffectsChainToClear.Append(LDRSubmixEffectChain);
	}
	else
	{
		AudioSubmixEffectsChainToApply.Append(LDRSubmixEffectChain);
		AudioSubmixEffectsChainToClear.Append(HDRSubmixEffectChain);
	}

	// We want to collect just the submixes we need to actually clear, otherwise they'll be overridden by the new settings

	TArray<USoundSubmix*> SubmixesLeftToClear;

	// We want to get the submixes that are not being overridden by the new effect chains, so we can clear those out separately

	for (const auto& EffectChainToClear : AudioSubmixEffectsChainToClear)
	{
		auto bAddToList{ true };

		for (const auto& SubmixEffectChain : AudioSubmixEffectsChainToApply)
		{
			if (SubmixEffectChain.Submix == EffectChainToClear.Submix)
			{
				bAddToList = false;

				break;
			}
		}

		if (bAddToList)
		{
			SubmixesLeftToClear.Add(EffectChainToClear.Submix);
		}
	}

	// Override submixes

	for (const auto& SubmixEffectChain : AudioSubmixEffectsChainToApply)
	{
		if (SubmixEffectChain.Submix)
		{
			UAudioMixerBlueprintLibrary::SetSubmixEffectChainOverride(GetWorld(), SubmixEffectChain.Submix, SubmixEffectChain.SubmixEffectChain, 0.1f);

		}
	}

	// Clear remaining submixes
	for (USoundSubmix* Submix : SubmixesLeftToClear)
	{
		UAudioMixerBlueprintLibrary::ClearSubmixEffectChainOverride(GetWorld(), Submix, 0.1f);
	}
}

void UAudioMixSubsystem::ApplyControlBusMix(FGameplayTag ControlBusMixTag, bool NewActivate)
{
	if (auto* World{ GetWorld() })
	{
		if (auto ControlBusMix{ ControlBusMixMap.FindRef(ControlBusMixTag) })
		{
			if (NewActivate)
			{
				UAudioModulationStatics::ActivateBusMix(World, ControlBusMix);
			}
			else
			{
				UAudioModulationStatics::DeactivateBusMix(World, ControlBusMix);
			}
		}
	}
}

void UAudioMixSubsystem::ApplyControlBusVolume(FGameplayTag ControlBusTag, float NewVolume)
{
	if (auto* World{ GetWorld() })
	{
		if (auto ControlBusMix{ ControlBusMixMap.FindRef(TAG_Audio_ControlBusMix_User) })
		{
			TArray<FSoundControlBusMixStage> ControlBusMixStageArray;

			if (auto ControlBus{ ControlBusMap.FindRef(ControlBusTag) })
			{
				const auto NewControlBusMixStage
				{
					UAudioModulationStatics::CreateBusMixStage(World, ControlBus, NewVolume)
				};

				ControlBusMixStageArray.Add(NewControlBusMixStage);
			}

			UAudioModulationStatics::UpdateMix(World, ControlBusMix, ControlBusMixStageArray);
		}
	}
}
