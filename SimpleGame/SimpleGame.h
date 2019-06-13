#pragma once
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <Dwmapi.h>
#include <strsafe.h>

#include "Animation.h"
#include "Sound.h"
#include "FiniteStateMachine.h"
#include "Fighter.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

template<typename Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define TRUE 1
#define FALSE 0

#define SCENE_OPENING 0
#define SCENE_OPENING_TO_PLAY 1
#define SCENE_PLAY 2

#define CONV_PLAYER_01 1001
#define CONV_ELF_01 2001
#define CONV_ELF_02 2002
#define CONV_ELF_03 2003
#define CONV_ELF_04 2004
#define CONV_ELF_05 2005
#define CONV_ELF_06 2006
#define CONV_FIGHTER_01 3001
#define CONV_FIGHTER_02 3002
#define CONV_FIGHTER_03 3003
#define CONV_FIGHTER_04 30044
#define CONV_FIGHTER_05 3005

#define IDLE 1
#define WALK 2
#define ELF_IDLE_NUM 21
#define ELF_WALK_NUM 12
#define FIGHTER_IDLE_NUM 39
#define FIGHTER_WALK_NUM 36
#define SORCERESS_IDLE_NUM 31
#define SORCERESS_WALK_NUM 12
#define LEFT 0
#define RIGHT 1

#define NUM_SOUND 10

int sceneNo = SCENE_OPENING;
float opacity = 0.0;
int ConversationDisplayed = FALSE;
int convNo = CONV_PLAYER_01;
bool Mute = false;
bool Attack = false;

unsigned int ELFState = IDLE;
unsigned int ELFFrame = 0;
unsigned int ELFFaced = LEFT;
unsigned int ELFChatTrigger = 0;

unsigned int FIGHTERState = WALK;
unsigned int FIGHTERFaced = LEFT;

unsigned int SORCERESSState = IDLE;
unsigned int SORCERESSFrame = 0;
unsigned int SORCERESSFaced = RIGHT;

unsigned int Bushes1Face = RIGHT;
unsigned int Bushes2Face = RIGHT;
D2D1_POINT_2F playerMove = D2D1::Point2F(0, 0);
D2D1_POINT_2F elfMove = D2D1::Point2F(600, 0);

float skewAngle1 = 3;
float skewAngle2 = -8;
int houseHP = 100;

const WCHAR PlayerInfo[] = L"유혜린(25세, 공무원)";
const WCHAR ElfInfo[] = L"재개발 구역 거주민(23세, 취업 준비생)";
const WCHAR FighterInfo[] = L"한상정(27세, 용역 깡패)";

const WCHAR sc_OpeningStory[] =
L"인천시 시청 공무원이 되기위해 3년동안 공부한 유혜린(25세, 기술직 공무원).\n\n\n결국 3년의 공부끝에 시청 공무원이 되었다.\n\n\n그녀는 첫 업무로 재개발 구역으로 지정된 숲에 사는 시민들에게 퇴거명령을 보낸다.\n\n\n(엔터 키를 눌러서 계속)";
int OpeningTextLen = ARRAYSIZE(sc_OpeningStory);

const WCHAR sc_PlayerMain1[] = L"여기가 재개발 구역인가..?";
int PlayerMain1Len = ARRAYSIZE(sc_PlayerMain1);

const WCHAR sc_FighterMain1[] = L"선생님... 시작할까요...?";
const WCHAR sc_FighterMain2[] = L"어쩌지?";
const WCHAR sc_FighterMain3[] = L"부순다(Y)    기다린다(N)";
const WCHAR sc_FighterMain4[] = L"노후... 건물... 부순다...";
const WCHAR sc_FighterMain5[] = L"알겠습니다...";
int FighterMain1Len = ARRAYSIZE(sc_FighterMain1);
int FighterMain2Len = ARRAYSIZE(sc_FighterMain2);
int FighterMain3Len = ARRAYSIZE(sc_FighterMain3);
int FighterMain4Len = ARRAYSIZE(sc_FighterMain4);
int FighterMain5Len = ARRAYSIZE(sc_FighterMain5);

const WCHAR sc_ElfMain1[] = L"내 집...";
const WCHAR sc_ElfMain2[] = L"저기 있는 깡패가 저희 집을 부수려고 해요 도와주세요!!";
const WCHAR sc_ElfMain3[] = L"어쩌지?";
const WCHAR sc_ElfMain4[] = L"도와준다(Y)    거절한다(N)";
const WCHAR sc_ElfMain5[] = L"고맙습니다!";
const WCHAR sc_ElfMain6[] = L"캭~ 퉷!";
int ElfMain1Len = ARRAYSIZE(sc_ElfMain1);
int ElfMain2Len = ARRAYSIZE(sc_ElfMain2);
int ElfMain3Len = ARRAYSIZE(sc_ElfMain3);
int ElfMain4Len = ARRAYSIZE(sc_ElfMain4);
int ElfMain5Len = ARRAYSIZE(sc_ElfMain5);
int ElfMain6Len = ARRAYSIZE(sc_ElfMain6);

//--------------------------------------------------------------------------------------
// Helper macros
//--------------------------------------------------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

class SimpleGame
{
private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	// 비트맵 로드를 위한 m_pWICFactory 객체
	IWICImagingFactory* m_pWICFactory;

	// 배경을 위한 비트맵
	ID2D1Bitmap* m_pOpeningBackgroundBitmap;
	ID2D1Bitmap* m_pGroundBitmap;
	ID2D1Bitmap* m_pForestBitmap;
	ID2D1Bitmap* m_pTreesBitmap;
	ID2D1Bitmap* m_pBushesBitmap;
	ID2D1Bitmap* m_pStoneBitmap;
	ID2D1Bitmap* m_pHouseBitmap[5];

	// 텍스트
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_pWhiteBrush;

	ID2D1Bitmap* m_pChatIconBitmap;

	ID2D1Bitmap* m_pTextBoxBitmap;
	ID2D1Bitmap* m_pFrameBitmap;
	/*--------------------------------------------------------------------------------*/
	// ELF의 사진을 위한 비트맵
	ID2D1Bitmap* m_pElfPortraitBitmap;

	// ELF의 IDLE_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pElfIdleLBitmap[ELF_IDLE_NUM];
	// ELF의 IDLE_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pElfIdleRBitmap[ELF_IDLE_NUM];

	// ELF의 WALK_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pElfWalkLBitmap[ELF_WALK_NUM];
	// ELF의 WALK_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pElfWalkRBitmap[ELF_WALK_NUM];
	/*--------------------------------------------------------------------------------*/
	// FIGHTER의 사진을 위한 비트맵
	ID2D1Bitmap* m_pFighterPortraitBitmap;

	// FIGHTER의 IDLE_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pFighterIdleLBitmap[FIGHTER_IDLE_NUM];
	// FIGHTER의 IDLE_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pFighterIdleRBitmap[FIGHTER_IDLE_NUM];

	// FIGHTER의 WALK_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pFighterWalkLBitmap[FIGHTER_WALK_NUM];
	// FIGHTER의 WALK_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pFighterWalkRBitmap[FIGHTER_WALK_NUM];
	/*--------------------------------------------------------------------------------*/
	// SORCERESS의 사진을 위한 비트맵
	ID2D1Bitmap* m_pSorceressPortraitBitmap;

	// SORCERESS의 IDLE_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pSorceressIdleLBitmap[SORCERESS_IDLE_NUM];
	// SORCERESS의 IDLE_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pSorceressIdleRBitmap[SORCERESS_IDLE_NUM];

	// SORCERESS의 WALK_L 상태를 위한 비트맵
	ID2D1Bitmap* m_pSorceressWalkLBitmap[SORCERESS_WALK_NUM];
	// SORCERESS의 WALK_R 상태를 위한 비트맵
	ID2D1Bitmap* m_pSorceressWalkRBitmap[SORCERESS_WALK_NUM];
	/*--------------------------------------------------------------------------------*/
	ID2D1Bitmap* m_pGoodBitmap[4];
	/*--------------------------------------------------------------------------------*/
	ID2D1Bitmap* m_pBadBitmap[4];
	/*--------------------------------------------------------------------------------*/
	ID2D1Bitmap* m_pPlayerBitmap[4];
	/*--------------------------------------------------------------------------------*/
	// LightningBug
	ID2D1Bitmap* m_pYellowBitmap;
	ID2D1BitmapBrush* m_pYellowBitmapBrush;
	ID2D1RectangleGeometry* m_pRectGeo;
	ID2D1RadialGradientBrush* m_pRadialGradientBrush;

	// 벌레의 이동 경로를 위한 PathGeometry
	ID2D1PathGeometry* m_pPathGeometry;
	AnimationEaseInOut m_Animation;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	// 인공지능

public:
	SimpleGame();
	~SimpleGame();
	HRESULT Initialize(); // 윈도우 클래스를 등록, 그리기 자원들을 생성하는 함수들을 호출
	void RunMessageLoop(); // 메시지를 처리

private:
	HRESULT CreateDeviceIndependentResources(); // 장치 독립 자원들을 초기화
	HRESULT CreateDeviceResources(); // 장치 의존 자원들을 초기화
	void DiscardDeviceResources(); // 장치 의존 자원들을 반납
	HRESULT OnRender(); // 내용을 그리기
	void DisplayConversation(ID2D1Bitmap* m_pPortraitBitmap, D2D1_SIZE_F rtSize, int convNo);
	void OnResize(UINT width, UINT height); // 렌더타겟을 resize
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap);
};
