#include "SoundController.h"
#include <tchar.h>

SoundController::SoundController()
{
	soundEngine = new SoundEngine();

	soundEngine->LoadFile(IN_TARGET_SOUND, false);
	soundEngine->LoadFile(OUTSIDE_TARGET_SOUND, false);
	soundEngine->LoadFile(BACKGROUND_THEME, true);
}

SoundController::~SoundController()
{
	delete soundEngine;
}

HRESULT SoundController::PlayInTargetSound()
{
	return soundEngine->PlayMusic(IN_TARGET_SOUND);
}

HRESULT SoundController::PlayOutsideTargerSound()
{
	return soundEngine->PlayMusic(OUTSIDE_TARGET_SOUND);
}

HRESULT SoundController::PauseAllSounds()
{
	return soundEngine->StopAllSounds();
}

HRESULT SoundController::PauseMainTheme()
{
	return soundEngine->StopSound(BACKGROUND_THEME);
}

HRESULT SoundController::ResumeAllSounds()
{
	return soundEngine->StartAllSounds();
}

HRESULT SoundController::PlayMainTheme()
{
	return soundEngine->PlayMusic(BACKGROUND_THEME);
}