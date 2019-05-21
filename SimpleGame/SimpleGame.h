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

#define IDLE 1
#define WALK 2
#define ELF_IDLE_NUM 21
#define ELF_WALK_NUM 12
#define FIGHTER_IDLE_NUM 39
#define FIGHTER_WALK_NUM 36
#define SORCERESS_IDLE_NUM 31
#define SORCERESS_WALK_NUM 12

class SimpleGame
{
private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	// ��Ʈ�� �ε带 ���� m_pWICFactory ��ü
	IWICImagingFactory* m_pWICFactory;
	// Ground.png�� ���� ��Ʈ��
	ID2D1Bitmap* m_pGroundBitmap;
	// ELF�� IDLE ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfIdleBitmap[ELF_IDLE_NUM];
	// ELF�� WALK ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pElfWalkBitmap[ELF_WALK_NUM];
	// FIGHTER�� IDLE ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterIdleBitmap[FIGHTER_IDLE_NUM];
	// FIGHTER�� WALK ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pFighterWalkBitmap[FIGHTER_WALK_NUM];
	// SORCERESS�� IDLE ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressIdleBitmap[SORCERESS_IDLE_NUM];
	// SORCERESS�� WALK ���¸� ���� ��Ʈ��
	ID2D1Bitmap* m_pSorceressWalkBitmap[SORCERESS_WALK_NUM];

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
