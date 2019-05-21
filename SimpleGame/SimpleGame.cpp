#include "SimpleGame.h"


// playerState
unsigned int ELFState = WALK;
unsigned int ELFFrame = 16;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 이 프로세스가 사용하는 힙에 에러가 발생하면 이 프로세스를 종료하도록 명시함.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL))) {
		{
			SimpleGame app;
			if (SUCCEEDED(app.Initialize())) {  // SimpleGame 클래스의 instance 초기화
				app.RunMessageLoop();  // 메시지루프 시작
			}
		}
		CoUninitialize();
	}
	return 0;
}

SimpleGame::SimpleGame() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pWICFactory(NULL),
	m_pGroundBitmap(NULL),
	m_pElfIdleBitmap(),
	m_pElfWalkBitmap(),
	m_pFighterIdleBitmap(),
	m_pFighterWalkBitmap(),
	m_pSorceressIdleBitmap(),
	m_pSorceressWalkBitmap()
{
}

SimpleGame::~SimpleGame()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);

	SAFE_RELEASE(m_pWICFactory);

	SAFE_RELEASE(m_pGroundBitmap);
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkBitmap[i]);
	}
}

HRESULT SimpleGame::Initialize() {
	HRESULT hr = CreateDeviceIndependentResources(); //장치 독립적 자원들을 초기화함

	if (SUCCEEDED(hr)) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;  wcex.lpfnWndProc = SimpleGame::WndProc;
		wcex.cbClsExtra = 0;  wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;  wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;  wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"Kina Roguelike";
		RegisterClassEx(&wcex); // 윈도우 클래스를 등록함
		FLOAT dpiX, dpiY;
		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY); //현재의 시스템 DPI를 얻음
		m_hwnd = CreateWindow( // 윈도우를 생성함
			L"Kina Roguelike", L"Kina Roguelike",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			/*static_cast<UINT>(ceil(640.f * dpiX / 96.f))*/1200,
			/*static_cast<UINT>(ceil(480.f * dpiY / 96.f))*/800,
			NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}
	return hr;
}

void SimpleGame::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT SimpleGame::CreateDeviceIndependentResources()
{
	// 장치 독립적 자원들을 생성함
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	// Create a WIC factory.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	return hr;
}

HRESULT SimpleGame::CreateDeviceResources()
{
	// 장치 의존적 자원들을 생성함
	HRESULT hr = S_OK;

	if (!m_pRenderTarget) { // 렌더타겟이 이미 유효하면 실행하지 않음
		// 클라이언트 영역의 크기를 얻음.
		RECT rc;  GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// D2D 렌더타겟을 생성함.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\ground.png", 1200, 100, &m_pGroundBitmap);
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_IDLE\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfIdleBitmap[i++]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_WALK\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfWalkBitmap[i++]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_IDLE\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterIdleBitmap[i++]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_WALK\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterWalkBitmap[i++]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_IDLE\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressIdleBitmap[i++]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_WALK\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressWalkBitmap[i++]);
			}
		}
	}
	return hr;
}

void SimpleGame::DiscardDeviceResources()
{
	// CreateDeviceResources 함수에서 생성한 모든 자원들을 반납
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pGroundBitmap);
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkBitmap[i]);
	}
}

HRESULT SimpleGame::OnRender()
{
	HRESULT hr = S_OK;
	hr = CreateDeviceResources(); // 항상 호출되며, 렌더타겟이 유효하면 아무일도 하지 않음.
	if (SUCCEEDED(hr)) { //렌더타겟이 유효함.

		m_pRenderTarget->BeginDraw(); //그리기 시작.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //변환행렬을 항등행렬로.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); // 창을 클리어.

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize(); //그리기 영역의 크기를 얻음.

		D2D1_SIZE_F bitmapSize = m_pGroundBitmap->GetSize();
		// 지면 그리기
		m_pRenderTarget->DrawBitmap(m_pGroundBitmap, D2D1::RectF(0, rtSize.height-100, rtSize.width, rtSize.height));

		// 캐릭터 그리기
		if (ELFState == IDLE)
		{
			bitmapSize = m_pElfIdleBitmap[ELFFrame]->GetSize();
			m_pRenderTarget->DrawBitmap(m_pElfIdleBitmap[ELFFrame++], D2D1::RectF(300 - bitmapSize.width / 2, rtSize.height - 100 - bitmapSize.height, 300 + bitmapSize.width / 2, rtSize.height - 100));
			if (ELFFrame >= ELF_IDLE_NUM) ELFFrame = 0;
		}
		if (ELFState == WALK)
		{
			bitmapSize = m_pElfWalkBitmap[ELFFrame]->GetSize();
			m_pRenderTarget->DrawBitmap(m_pElfWalkBitmap[ELFFrame++], D2D1::RectF(300 - bitmapSize.width / 2, rtSize.height - 100 - bitmapSize.height, 300 + bitmapSize.width / 2, rtSize.height - 100));
			if (ELFFrame >= ELF_WALK_NUM) ELFFrame = 0;
		}
		hr = m_pRenderTarget->EndDraw();  //그리기를 수행함. 성공하면 S_OK를 리턴함.
	}
	DiscardDeviceResources();
	if (hr == D2DERR_RECREATE_TARGET) { // 렌더타겟을 재생성해야 함.
		// 실행중에 D3D 장치가 소실된 경우,
		hr = S_OK;
		DiscardDeviceResources(); // 장치 의존 자원들을 반납함.
		// 다음번 OnRender 함수가 호출될 때에 재생성함.
	}
	return hr;
}

void SimpleGame::OnResize(UINT width, UINT height)
{
	// 창이 resize될 경우에 렌더타겟도 이에 맞도록 resize함.
	if (m_pRenderTarget) {
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		// Resize함수가 실패할 수도 있으나, 예외처리하지 않았음.
		// 왜냐하면, 실패하는  경우에, 다음번 EndDraw 호출에서도 실패할 것이므로.
	}
}

LRESULT CALLBACK SimpleGame::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		SimpleGame* pSimpleGame = (SimpleGame*)pcs->lpCreateParams;
		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pSimpleGame));
		result = 1;
	}
	else {
		SimpleGame* pSimpleGame = reinterpret_cast<SimpleGame*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
		bool wasHandled = false;
		if (pSimpleGame) {
			switch (message) {
			case WM_SIZE:
			{  UINT width = LOWORD(lParam);  // 새 창의 크기
			UINT height = HIWORD(lParam);
			pSimpleGame->OnResize(width, height);  // 창의 크기를 다시 조절
			}
			result = 0;  wasHandled = true;  break;
			case WM_DISPLAYCHANGE:
			{  InvalidateRect(hwnd, NULL, FALSE); //WM_PAINT를 발생시킴
			}
			result = 0;  wasHandled = true;  break;
			case WM_PAINT:
			{  pSimpleGame->OnRender();  // 창을 그림
			ValidateRect(hwnd, NULL);
			}
			result = 0;  wasHandled = true;  break;
			case WM_DESTROY:
			{  PostQuitMessage(0);
			}
			result = 1;  wasHandled = true;  break;
			}//switch

		}
		if (!wasHandled)
			result = DefWindowProc(hwnd, message, wParam, lParam);
	}
	return result;
}

HRESULT SimpleGame::LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(pSource, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}