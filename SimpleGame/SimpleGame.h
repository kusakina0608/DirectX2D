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

const WCHAR PlayerInfo[] = L"������(25��, ������)";
const WCHAR ElfInfo[] = L"�簳�� ���� ���ֹ�(23��, ��� �غ��)";
const WCHAR FighterInfo[] = L"�ѻ���(27��, �뿪 ����)";

const WCHAR sc_OpeningStory[] =
L"��õ�� ��û �������� �Ǳ����� 3�⵿�� ������ ������(25��, ����� ������).\n\n\n�ᱹ 3���� ���γ��� ��û �������� �Ǿ���.\n\n\n�׳�� ù ������ �簳�� �������� ������ ���� ��� �ùε鿡�� ��Ÿ���� ������.\n\n\n(���� Ű�� ������ ���)";
int OpeningTextLen = ARRAYSIZE(sc_OpeningStory);

const WCHAR sc_PlayerMain1[] = L"���Ⱑ �簳�� �����ΰ�..?";
int PlayerMain1Len = ARRAYSIZE(sc_PlayerMain1);

const WCHAR sc_FighterMain1[] = L"������... �����ұ��...?";
const WCHAR sc_FighterMain2[] = L"��¼��?";
const WCHAR sc_FighterMain3[] = L"�μ���(Y)    ��ٸ���(N)";
const WCHAR sc_FighterMain4[] = L"����... �ǹ�... �μ���...";
const WCHAR sc_FighterMain5[] = L"�˰ڽ��ϴ�...";
int FighterMain1Len = ARRAYSIZE(sc_FighterMain1);
int FighterMain2Len = ARRAYSIZE(sc_FighterMain2);
int FighterMain3Len = ARRAYSIZE(sc_FighterMain3);
int FighterMain4Len = ARRAYSIZE(sc_FighterMain4);
int FighterMain5Len = ARRAYSIZE(sc_FighterMain5);

const WCHAR sc_ElfMain1[] = L"�� ��...";
const WCHAR sc_ElfMain2[] = L"���� �ִ� ���а� ���� ���� �μ����� �ؿ� �����ּ���!!";
const WCHAR sc_ElfMain3[] = L"��¼��?";
const WCHAR sc_ElfMain4[] = L"�����ش�(Y)    �����Ѵ�(N)";
const WCHAR sc_ElfMain5[] = L"�����ϴ�!";
const WCHAR sc_ElfMain6[] = L"Ľ~ ��!";
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

	// ��Ʈ�� �ε带 ���� m_pWICFactory ��ü
	IWICImagingFactory* m_pWICFactory;

	// ����� ���� ��Ʈ��
	ID2D1Bitmap* m_pOpeningBackgroundBitmap;
	ID2D1Bitmap* m_pGroundBitmap;
	ID2D1Bitmap* m_pForestBitmap;
	ID2D1Bitmap* m_pTreesBitmap;
	ID2D1Bitmap* m_pBushesBitmap;
	ID2D1Bitmap* m_pStoneBitmap;
	ID2D1Bitmap* m_pHouseBitmap[5];

	// �ؽ�Ʈ
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_pWhiteBrush;

	ID2D1Bitmap* m_pChatIconBitmap;

	ID2D1Bitmap* m_pTextBoxBitmap;
	ID2D1Bitmap* m_pFrameBitmap;
	/*--------------------------------------------------------------------------------*/
	// ELF�� ������ ���� ��Ʈ��
	ID2D1Bitmap* m_pElfPortraitBitmap;

	// ELF�� IDLE_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfIdleLBitmap[ELF_IDLE_NUM];
	// ELF�� IDLE_R ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfIdleRBitmap[ELF_IDLE_NUM];

	// ELF�� WALK_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfWalkLBitmap[ELF_WALK_NUM];
	// ELF�� WALK_R ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfWalkRBitmap[ELF_WALK_NUM];
	/*--------------------------------------------------------------------------------*/
	// FIGHTER�� ������ ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterPortraitBitmap;

	// FIGHTER�� IDLE_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterIdleLBitmap[FIGHTER_IDLE_NUM];
	// FIGHTER�� IDLE_R ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterIdleRBitmap[FIGHTER_IDLE_NUM];

	// FIGHTER�� WALK_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterWalkLBitmap[FIGHTER_WALK_NUM];
	// FIGHTER�� WALK_R ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterWalkRBitmap[FIGHTER_WALK_NUM];
	/*--------------------------------------------------------------------------------*/
	// SORCERESS�� ������ ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressPortraitBitmap;

	// SORCERESS�� IDLE_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressIdleLBitmap[SORCERESS_IDLE_NUM];
	// SORCERESS�� IDLE_R ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressIdleRBitmap[SORCERESS_IDLE_NUM];

	// SORCERESS�� WALK_L ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressWalkLBitmap[SORCERESS_WALK_NUM];
	// SORCERESS�� WALK_R ���¸� ���� ��Ʈ��
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

	// ������ �̵� ��θ� ���� PathGeometry
	ID2D1PathGeometry* m_pPathGeometry;
	AnimationEaseInOut m_Animation;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	// �ΰ�����

public:
	SimpleGame();
	~SimpleGame();
	HRESULT Initialize(); // ������ Ŭ������ ���, �׸��� �ڿ����� �����ϴ� �Լ����� ȣ��
	void RunMessageLoop(); // �޽����� ó��

private:
	HRESULT CreateDeviceIndependentResources(); // ��ġ ���� �ڿ����� �ʱ�ȭ
	HRESULT CreateDeviceResources(); // ��ġ ���� �ڿ����� �ʱ�ȭ
	void DiscardDeviceResources(); // ��ġ ���� �ڿ����� �ݳ�
	HRESULT OnRender(); // ������ �׸���
	void DisplayConversation(ID2D1Bitmap* m_pPortraitBitmap, D2D1_SIZE_F rtSize, int convNo);
	void OnResize(UINT width, UINT height); // ����Ÿ���� resize
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap);
};
