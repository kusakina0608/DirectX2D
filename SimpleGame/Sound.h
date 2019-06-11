#include <vector>
#include <windows.h>
#include <xaudio2.h>
#include <strsafe.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <conio.h>
#include "SDKwavefile.h"

//--------------------------------------------------------------------------------------
// Helper macros
//--------------------------------------------------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

typedef struct SoundInfo {
	BYTE* pbWaveData;
	IXAudio2SourceVoice* pSourceVoice;
	WCHAR szFilename[100];
} SoundInfo;



class CSoundManager
{
protected:
private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	std::vector <SoundInfo> m_soundVector;

public:
	CSoundManager();
	~CSoundManager();

	int init();
	void reset(int id);
	void volume(float vol);
	HRESULT play(int id);
	HRESULT replay(int id);
	HRESULT stop(int id);
	HRESULT add(WCHAR* szFilename, int id);
	HRESULT FindMediaFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename);
};