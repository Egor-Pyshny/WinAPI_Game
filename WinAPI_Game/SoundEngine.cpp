#include "SoundEngine.h"
#include <Windows.h>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

SoundEngine::SoundEngine()
{
    HRESULT hr;

    if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        MessageBox(NULL, TEXT("Error occured initializing COM"),
            TEXT("SoundController error"), MB_ICONERROR);
    }
    if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    {
        MessageBox(NULL, TEXT("Error occured creating the XAudio2!"),
            TEXT("SoundController error"), MB_ICONERROR);
    }
    if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
    {
        MessageBox(NULL, TEXT("Error occured creating the MasterVoice!"),
            TEXT("SoundController error"), MB_ICONERROR);
    }
}

SoundEngine::~SoundEngine()
{
    for (auto it = voicesPool.begin(); it != voicesPool.end(); it++)
    {
        delete it->second.pBuffer;
        delete it->second.pWfx;
    }
    pMasterVoice->DestroyVoice();
    pXAudio2->Release();
}

HRESULT SoundEngine::PlayMusic(const TCHAR* filename)
{
    HRESULT hr;
    XAUDIO2_BUFFER* pBuffer = voicesPool[filename].pBuffer;
    WAVEFORMATEX* pWfx = voicesPool[filename].pWfx;
    IXAudio2SourceVoice* pSourceVoice = voicesPool[filename].pSourceVoice;

    if (pSourceVoice)
        pSourceVoice->DestroyVoice();

    if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, pWfx)))
        return hr;

    if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(pBuffer)))
        return hr;

    if (FAILED(hr = pSourceVoice->Start(0)))
        return hr;

    voicesPool[filename].pSourceVoice = pSourceVoice;
}

HRESULT SoundEngine::StopAllSounds()
{
    HRESULT hr;
    IXAudio2SourceVoice* pSourceVoice;

    for (auto it = voicesPool.begin(); it != voicesPool.end(); it++)
    {
        pSourceVoice = it->second.pSourceVoice;
        if (pSourceVoice)
            hr = pSourceVoice->Stop(0);
    }

    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Error occured stopping all sounds"),
            TEXT("SoundEngine error"), MB_ICONERROR);
    }
    return hr;
}

HRESULT SoundEngine::StopSound(const TCHAR* filename)
{
    HRESULT hr;
    IXAudio2SourceVoice* pSourceVoice = voicesPool[filename].pSourceVoice;

    if (pSourceVoice)
        hr = pSourceVoice->Stop(0);

    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Error occured stopping all sounds"),
            TEXT("SoundEngine error"), MB_ICONERROR);
    }
    return hr;
}

HRESULT SoundEngine::StartAllSounds()
{
    HRESULT hr;
    IXAudio2SourceVoice* pSourceVoice;

    for (auto it = voicesPool.begin(); it != voicesPool.end(); it++)
    {
        pSourceVoice = it->second.pSourceVoice;
        if (it->second.pSourceVoice)
            hr = pSourceVoice->Start(0);
    }

    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Error occured starting all sounds"),
            TEXT("SoundEngine error"), MB_ICONERROR);
    }
    return hr;
}

HRESULT SoundEngine::StartSound(const TCHAR* filename)
{
    HRESULT hr;
    IXAudio2SourceVoice* pSourceVoice = voicesPool[filename].pSourceVoice;

    if (pSourceVoice)
        hr = pSourceVoice->Start(0);

    if (FAILED(hr))
    {
        MessageBox(NULL, TEXT("Error occured stopping all sounds"),
            TEXT("SoundEngine error"), MB_ICONERROR);
    }
    return hr;
}

HRESULT SoundEngine::LoadFile(const TCHAR* filename, bool loop)
{
    XAUDIO2_BUFFER* pBuffer = new XAUDIO2_BUFFER();
    WAVEFORMATEX* pWfx = new WAVEFORMATEX();
    HRESULT hr;

    ZeroMemory(pBuffer, sizeof(XAUDIO2_BUFFER));
    ZeroMemory(pWfx, sizeof(WAVEFORMATEX));

    HANDLE hFile = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return HRESULT_FROM_WIN32(GetLastError());

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
        return S_FALSE;

    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, pWfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    pBuffer->AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    pBuffer->pAudioData = pDataBuffer;  //buffer containing audio data
    pBuffer->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
    if (loop)
    {
        pBuffer->LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    SOUND sound;
    sound.pBuffer = pBuffer;
    sound.pWfx = pWfx;
    sound.pSourceVoice = nullptr;
    voicesPool[filename] = sound;
}

HRESULT SoundEngine::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;

}

HRESULT SoundEngine::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}