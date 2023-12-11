#include "SoundController.h"
#include <tchar.h>

SoundController::SoundController()
{
	soundEngine = new SoundEngine();

	soundEngine->LoadFile(GAME_OVER_SOUND, false);
	soundEngine->LoadFile(ROW_DELETE_SOUND, false);
	soundEngine->LoadFile(BACKGROUND_THEME, true);
}

SoundController::~SoundController()
{
	delete soundEngine;
}

HRESULT SoundController::PlayGameOverSound()
{
	return soundEngine->PlayMusic(GAME_OVER_SOUND);
}

HRESULT SoundController::PlayRowDeleteSound()
{
	return soundEngine->PlayMusic(ROW_DELETE_SOUND);
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