// Copyright (C) 2024 owoDra

#include "AudioDeveloperSettings.h"

#include "GameplayTag/GACTags_Audio.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AudioDeveloperSettings)


UAudioDeveloperSettings::UAudioDeveloperSettings()
{
	CategoryName = TEXT("Game XXX Core");
	SectionName = TEXT("Game Audio Core");

	ControlBusMixMap =
	{
		{ TAG_Audio_ControlBusMix_DefaultBase	, FSoftObjectPath() }
	};

	ControlBusMap =
	{
		{ TAG_Audio_ControlBus_Overrall	, FSoftObjectPath() },
		{ TAG_Audio_ControlBus_Music	, FSoftObjectPath() },
		{ TAG_Audio_ControlBus_SoundFX	, FSoftObjectPath() }
	};
}


FSoftObjectPath UAudioDeveloperSettings::GetControlBusMixByTag(const FGameplayTag& Tag) const
{
	return ControlBusMixMap.FindRef(Tag);
}

FSoftObjectPath UAudioDeveloperSettings::GetControlBusByTag(const FGameplayTag& Tag) const
{
	return ControlBusMap.FindRef(Tag);
}
