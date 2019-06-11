#include "Sound.h"


CSoundManager::CSoundManager()
{
	HRESULT hr;
	//
	// Initialize XAudio2
	//
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	pXAudio2 = NULL;
	UINT32 flags = 0;

#ifdef _DEBUG
	//	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	if(FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		wprintf(L"Failed to init XAudio2 engine: %#X\n", hr);
		CoUninitialize();
		//      return 0;
	}

	//
	// Create a mastering voice
	//
	pMasteringVoice = NULL;

	if(FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		wprintf(L"Failed creating mastering voice: %#X\n", hr);
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		//	    return 0;
	}
}

CSoundManager::~CSoundManager()
{
	pMasteringVoice->DestroyVoice();

	SAFE_RELEASE(pXAudio2);
	CoUninitialize();
}


HRESULT CSoundManager::add(WCHAR* szFilename, int id)
{
	SoundInfo newSound;
	newSound.pbWaveData = NULL;
	newSound.pSourceVoice = NULL;
	wsprintf(newSound.szFilename, szFilename, 100);
	HRESULT hr = S_OK;
	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	if (FAILED(hr = FindMediaFileCch(strFilePath, MAX_PATH, newSound.szFilename)))
	{
		wprintf(L"Failed to find media file: %s\n", newSound.szFilename);
		return hr;
	}

	//
	// Read in the wave file
	//
	CWaveFile wav;
	if (FAILED(hr = wav.Open(strFilePath, NULL, WAVEFILE_READ)))
	{
		wprintf(L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath);
		return hr;
	}

	// Get format of wave file
	WAVEFORMATEX* pwfx = wav.GetFormat();

	// Calculate how many bytes and samples are in the wave
	DWORD cbWaveSize = wav.GetSize();

	// Read the sample data into memory
	newSound.pbWaveData = new BYTE[cbWaveSize];

	if (FAILED(hr = wav.Read(newSound.pbWaveData, cbWaveSize, &cbWaveSize)))
	{
		wprintf(L"Failed to read WAV data: %#X\n", hr);
		SAFE_DELETE_ARRAY(newSound.pbWaveData);
		return hr;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	newSound.pSourceVoice;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&newSound.pSourceVoice, pwfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		SAFE_DELETE_ARRAY(newSound.pbWaveData);
		return hr;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = newSound.pbWaveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = cbWaveSize;

	if (FAILED(hr = newSound.pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		newSound.pSourceVoice->DestroyVoice();
		SAFE_DELETE_ARRAY(newSound.pbWaveData);
		return hr;
	}

	// Succeeded so add to our vector and fill id for return
	m_soundVector.insert(m_soundVector.begin() + id, newSound);

	return TRUE;
}

//--------------------------------------------------------------------------------------
// Helper function to try to find the location of a media file
//--------------------------------------------------------------------------------------
HRESULT CSoundManager::FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
{
	bool bFound = false;

	if (NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10)
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] = { 0 };
	WCHAR strExeName[MAX_PATH] = { 0 };
	WCHAR* strLastSlash = NULL;
	GetModuleFileName(NULL, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr(strExePath, TEXT('\\'));
	if (strLastSlash)
	{
		wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr(strExeName, TEXT('.'));
		if (strLastSlash)
			* strLastSlash = 0;
	}

	wcscpy_s(strDestPath, cchDest, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	// Search all parent directories starting at .\ and using strFilename as the leaf name
	WCHAR strLeafName[MAX_PATH] = { 0 };
	wcscpy_s(strLeafName, MAX_PATH, strFilename);

	WCHAR strFullPath[MAX_PATH] = { 0 };
	WCHAR strFullFileName[MAX_PATH] = { 0 };
	WCHAR strSearch[MAX_PATH] = { 0 };
	WCHAR* strFilePart = NULL;

	GetFullPathName(L".", MAX_PATH, strFullPath, &strFilePart);
	if (strFilePart == NULL)
		return E_FAIL;

	while (strFilePart != NULL && *strFilePart != '\0')
	{
		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName);
		if (GetFileAttributes(strFullFileName) != 0xFFFFFFFF)
		{
			wcscpy_s(strDestPath, cchDest, strFullFileName);
			bFound = true;
			break;
		}

		swprintf_s(strSearch, MAX_PATH, L"%s\\..", strFullPath);
		GetFullPathName(strSearch, MAX_PATH, strFullPath, &strFilePart);
	}
	if (bFound)
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s(strDestPath, cchDest, strFilename);

	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}


HRESULT CSoundManager::play(int id)
{
	HRESULT hr = S_OK;
	hr = m_soundVector[id].pSourceVoice->Start(0); //사운드재생

	XAUDIO2_VOICE_STATE state;
	m_soundVector[id].pSourceVoice->GetState(&state);
	if (SUCCEEDED(hr) && state.BuffersQueued == 0) // 재생이끝나면 리소스해제
		reset(id);
	return hr;
}
HRESULT CSoundManager::replay(int id)
{
	HRESULT hr = S_OK;

	reset(id);//리셋 추가
	hr = m_soundVector[id].pSourceVoice->Start(0); //사운드재생

	XAUDIO2_VOICE_STATE state;
	m_soundVector[id].pSourceVoice->GetState(&state);
	if (SUCCEEDED(hr) && state.BuffersQueued == 0) // 재생이끝나면 리소스해제
		reset(id);
	return hr;
}

HRESULT CSoundManager::stop(int id)
{
	HRESULT hr = S_OK;
	hr = m_soundVector[id].pSourceVoice->Stop();
	return hr;
}

void CSoundManager::reset(int id)
{
	WCHAR szFilename[100];
	wcsncpy_s(szFilename, m_soundVector[id].szFilename, 100);

	m_soundVector[id].pSourceVoice->DestroyVoice();
	SAFE_DELETE_ARRAY(m_soundVector[id].pbWaveData);

	m_soundVector.erase(m_soundVector.begin() + id);
	CSoundManager::add(szFilename, id);
}

void CSoundManager::volume(float vol)
{
	/*vol값으로 모든사운드 음량조절*/
	for (int i = 0; i < (int)m_soundVector.size(); i++)
		m_soundVector[i].pSourceVoice->SetVolume(vol);

}