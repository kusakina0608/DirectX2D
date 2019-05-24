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

#include "Animation.h"

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

#define SCENE_OPENING 0
#define SCENE_OPENING_TO_PLAY 1
#define SCENE_PLAY 2

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
	
	// �ؽ�Ʈ
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_pWhiteBrush;


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
	void OnResize(UINT width, UINT height); // ����Ÿ���� resize
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap);
};
