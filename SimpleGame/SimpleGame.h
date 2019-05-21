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
class SimpleGame
{
private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;

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
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, // ������ ���ν���
		WPARAM wParam, LPARAM lParam);
};
