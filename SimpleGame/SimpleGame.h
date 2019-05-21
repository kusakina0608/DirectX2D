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
	HRESULT Initialize(); // 윈도우 클래스를 등록, 그리기 자원들을 생성하는 함수들을 호출
	void RunMessageLoop(); // 메시지를 처리

private:
	HRESULT CreateDeviceIndependentResources(); // 장치 독립 자원들을 초기화
	HRESULT CreateDeviceResources(); // 장치 의존 자원들을 초기화
	void DiscardDeviceResources(); // 장치 의존 자원들을 반납
	HRESULT OnRender(); // 내용을 그리기
	void OnResize(UINT width, UINT height); // 렌더타겟을 resize
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, // 윈도우 프로시져
		WPARAM wParam, LPARAM lParam);
};
