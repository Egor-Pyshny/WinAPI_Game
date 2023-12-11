#pragma once

#include <Windows.h>
#include <TCHAR.H>
#include "SoundEngine.h"
#include "MyDefines.h"

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

class SoundController
{
public:
	SoundController();
	~SoundController();

	HRESULT PlayGameOverSound();
	HRESULT PlayMainTheme();
	HRESULT PlayRowDeleteSound();
	HRESULT PauseAllSounds();
	HRESULT PauseMainTheme();
	HRESULT ResumeAllSounds();
protected:
	static constexpr const TCHAR* GAME_OVER_SOUND = TEXT("game_over_1.wav");
	static constexpr const TCHAR* ROW_DELETE_SOUND = TEXT("beep_1.wav");
	static constexpr const TCHAR* BACKGROUND_THEME = TEXT("tetris_theme_1.wav");

	SoundEngine* soundEngine;
};

