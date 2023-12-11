#pragma once

#include <xaudio2.h>
#include <string>
#include <map>

struct SOUND
{
	XAUDIO2_BUFFER* pBuffer;
	WAVEFORMATEX* pWfx;
	IXAudio2SourceVoice* pSourceVoice;
};

class SoundEngine
{
public:
	SoundEngine();
	~SoundEngine();

	HRESULT PlayMusic(const TCHAR* filename);
	HRESULT LoadFile(const TCHAR* filename, bool loop);
	HRESULT ChangeVolume(const TCHAR* filename, float volume);
	HRESULT StopAllSounds();
	HRESULT StopSound(const TCHAR* filename);
	HRESULT StartAllSounds();
	HRESULT StartSound(const TCHAR* filename);
protected:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	std::map<const TCHAR*, SOUND> voicesPool;

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
};

