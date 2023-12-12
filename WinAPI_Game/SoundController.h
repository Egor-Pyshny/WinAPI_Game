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

	HRESULT PlayInTargetSound();
	HRESULT PlayMainTheme();
	HRESULT PlayOutsideTargerSound();
	HRESULT PauseAllSounds();
	HRESULT PauseMainTheme();
	HRESULT ResumeAllSounds();
protected:
	static constexpr const TCHAR* IN_TARGET_SOUND = TEXT("intarget.wav");
	static constexpr const TCHAR* OUTSIDE_TARGET_SOUND = TEXT("outsidetarget.wav");
	static constexpr const TCHAR* BACKGROUND_THEME = TEXT("gamemusic.wav");

	SoundEngine* soundEngine;
};

