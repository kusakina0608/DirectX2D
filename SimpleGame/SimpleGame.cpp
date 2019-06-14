#pragma warning(disable : 4996)
#include "SimpleGame.h"
#include "Animation.h"
// playerState

CSoundManager* soundManager = NULL;

float volume = 1.0;

DWORD fighterStateTransitions[][3] = {
		{ Fighter::STATE_STOP, Fighter::EVENT_STARTWALK, Fighter::STATE_WALK },
		{ Fighter::STATE_STOP, Fighter::EVENT_TALK, Fighter::STATE_TALK },
		{ Fighter::STATE_WALK, Fighter::EVENT_TALK, Fighter::STATE_TALK },
		{ Fighter::STATE_WALK, Fighter::EVENT_STOPWALK, Fighter::STATE_STOP },
		{ Fighter::STATE_TALK, Fighter::EVENT_YES, Fighter::STATE_MOVE },
		{ Fighter::STATE_TALK, Fighter::EVENT_NO, Fighter::STATE_WALK },
		{ Fighter::STATE_MOVE, Fighter::EVENT_REACHED, Fighter::STATE_ATTACK },
		{ Fighter::STATE_ATTACK, Fighter::EVENT_TERMINATE, Fighter::STATE_WALK }
};

Fighter* fighter = new Fighter(Fighter::TYPE_AI, fighterStateTransitions, 8);

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
	m_pOpeningBackgroundBitmap(NULL),
	m_pEndingBackgroundBitmap(NULL),
	m_pGroundBitmap(NULL),
	m_pHouseBitmap(),
	m_pForestBitmap(NULL),
	m_pTreesBitmap(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pBlackBrush(NULL),
	m_pWhiteBrush(NULL),
	m_pTextBoxBitmap(NULL),
	m_pChatIconBitmap(NULL),
	m_pFrameBitmap(NULL),
	m_pBushesBitmap(NULL),
	m_pStoneBitmap(NULL),
	m_pYellowBitmap(NULL),
	m_pYellowBitmapBrush(NULL),
	m_pRectGeo(NULL),
	m_pRadialGradientBrush(NULL),
	m_pElfPortraitBitmap(NULL),
	m_pElfIdleLBitmap(),
	m_pElfIdleRBitmap(),
	m_pElfWalkLBitmap(),
	m_pElfWalkRBitmap(),
	m_pFighterPortraitBitmap(NULL),
	m_pFighterIdleLBitmap(),
	m_pFighterIdleRBitmap(),
	m_pFighterWalkLBitmap(),
	m_pFighterWalkRBitmap(),
	m_pSorceressPortraitBitmap(NULL),
	m_pSorceressIdleLBitmap(),
	m_pSorceressIdleRBitmap(),
	m_pSorceressWalkLBitmap(),
	m_pSorceressWalkRBitmap(),
	m_pGoodBitmap(),
	m_pBadBitmap(),
	m_pPlayerBitmap()
{
}

SimpleGame::~SimpleGame()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);

	SAFE_RELEASE(m_pWICFactory);

	SAFE_RELEASE(m_pOpeningBackgroundBitmap);
	SAFE_RELEASE(m_pEndingBackgroundBitmap);
	SAFE_RELEASE(m_pGroundBitmap);
	SAFE_RELEASE(m_pForestBitmap);
	SAFE_RELEASE(m_pTreesBitmap);
	SAFE_RELEASE(m_pBushesBitmap);
	SAFE_RELEASE(m_pStoneBitmap);

	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pHouseBitmap[i]);
	}

	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pWhiteBrush);

	SAFE_RELEASE(m_pTextBoxBitmap);
	SAFE_RELEASE(m_pChatIconBitmap);
	SAFE_RELEASE(m_pFrameBitmap);

	// LightningBug
	SAFE_RELEASE(m_pYellowBitmap);
	SAFE_RELEASE(m_pYellowBitmapBrush);
	SAFE_RELEASE(m_pRectGeo);
	SAFE_RELEASE(m_pRadialGradientBrush);

	/*--------------------------------------------------------------------------------*/
	SAFE_RELEASE(m_pElfPortraitBitmap);
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleLBitmap[i]);
		SAFE_RELEASE(m_pElfIdleRBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkLBitmap[i]);
		SAFE_RELEASE(m_pElfWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	SAFE_RELEASE(m_pFighterPortraitBitmap);
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleLBitmap[i]);
		SAFE_RELEASE(m_pFighterIdleRBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkLBitmap[i]);
		SAFE_RELEASE(m_pFighterWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	SAFE_RELEASE(m_pSorceressPortraitBitmap);
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleLBitmap[i]);
		SAFE_RELEASE(m_pSorceressIdleRBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkLBitmap[i]);
		SAFE_RELEASE(m_pSorceressWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	for (int i = 0; i < 4; i++)
	{
		SAFE_RELEASE(m_pGoodBitmap[i]);
		SAFE_RELEASE(m_pBadBitmap[i]);
		SAFE_RELEASE(m_pPlayerBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
}

HRESULT SimpleGame::Initialize() {
	HRESULT hr = CreateDeviceIndependentResources(); //장치 독립적 자원들을 초기화함

	soundManager = new CSoundManager;

	PCWSTR str[NUM_SOUND] = { 
		L".\\Sound\\BGM\\MainMusic.wav",
		L".\\Sound\\BGM\\Forest.wav",
		L".\\Sound\\Effect\\Alert.wav",
		L".\\Sound\\Effect\\Return.wav",
		L".\\Sound\\Effect\\Running.wav",
		L".\\Sound\\Effect\\Laugh.wav",
		L".\\Sound\\Effect\\Busunda.wav",
		L".\\Sound\\Effect\\Badara.wav",
		L".\\Sound\\Effect\\Arata.wav",
		L".\\Sound\\Effect\\Aww.wav"
	};

	for (int i = 0; i < NUM_SOUND; i++) {
		wchar_t strFilePath[100] = { 0, };
		lstrcpy(strFilePath, str[i]);
		if (!soundManager->add(strFilePath, i))
			return FALSE;			//버튼음
	}

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
		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pPathGeometry->ComputeLength(NULL, &length);

			if (SUCCEEDED(hr))
			{
				m_Animation.SetStart(0); //start at beginning of path
				m_Animation.SetEnd(length); //length at end of path
				m_Animation.SetDuration(5.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}
	}

	QueryPerformanceFrequency(&m_nFrequency);
	QueryPerformanceCounter(&m_nPrevTime);
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
	ID2D1GeometrySink* pSink = NULL;

	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 25;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	
	// Create a WIC factory.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}
	// 나선형 모양의 경로 기하를 생성함.
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreatePathGeometry(&m_pPathGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pPathGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { 0, 0 };

		pSink->BeginFigure(D2D1::Point2F(100, 100), D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F(200, 50), D2D1::Point2F(300, 50), D2D1::Point2F(400, 100)));
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F(600, 200), D2D1::Point2F(800, 200), D2D1::Point2F(1000, 100)));
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F(950, 80), D2D1::Point2F(850, 20), D2D1::Point2F(800, 0)));
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F(700, 100), D2D1::Point2F(400, 100), D2D1::Point2F(100, 200)));
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F(50, 400), D2D1::Point2F(50, 200), D2D1::Point2F(100, 100)));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = pSink->Close();
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pDirect2dFactory->CreateRectangleGeometry(D2D1::RectF(0, 0, 100, 100), &m_pRectGeo);
	}
	/**/
	if (SUCCEEDED(hr))
	{
		// DirectWrite 팩토리를 생성함.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown * *>(&m_pDWriteFactory)
		);
	}
	if (SUCCEEDED(hr))
	{
		//DirectWrite 텍스트 포맷 객체를 생성함.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// 텍스트를 수평으로 중앙 정렬하고 수직으로도 중앙 정렬함.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\jpg\\OpeningBackground.jpg", size.width, size.height, &m_pOpeningBackgroundBitmap);
		}if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\jpg\\EndingBackground.jpg", size.width, size.height, &m_pEndingBackgroundBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\ground.png", 1200, 100, &m_pGroundBitmap);
		}
		for (int i = 0; i < 5; i++)
		{
			if (SUCCEEDED(hr))
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\png\\house%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pHouseBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\Forest.jpg", 0, 0, &m_pForestBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\Trees.png", 0, 0, &m_pTreesBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\bushes.png", 0, 0, &m_pBushesBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\stone.png", 0, 0, &m_pStoneBitmap);
		}
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\yellow.png", 0, 0, &m_pYellowBitmap);
		}
		//비트맵 브러쉬 생성
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(
				D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			hr = m_pRenderTarget->CreateBitmapBrush(m_pYellowBitmap, propertiesXClampYClamp, &m_pYellowBitmapBrush);
		}

		// 불투명 마스크로 사용할 방사형 계조 붓을 생성함.
		if (SUCCEEDED(hr))
		{
			ID2D1GradientStopCollection* pGradientStops = NULL;

			static const D2D1_GRADIENT_STOP gradientStops[] =
			{
				{   0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f)  },
				{   1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f)  },
			};

			hr = m_pRenderTarget->CreateGradientStopCollection(
				gradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP,
				&pGradientStops);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(D2D1::Point2F(50, 50), D2D1::Point2F(0, 0), 50, 50), pGradientStops,
					&m_pRadialGradientBrush);
			}
			pGradientStops->Release();
		}
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\TextBox.png", 0, 0, &m_pTextBoxBitmap);
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\chat.png", 0, 0, &m_pChatIconBitmap);
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\png\\frame.png", 0, 0, &m_pFrameBitmap);
		/*--------------------------------------------------------------------------------*/
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\jpg\\ELF.jpg", 0, 0, &m_pElfPortraitBitmap);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_IDLE_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\ELF_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pElfIdleLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\ELF_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pElfIdleRBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_WALK_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\ELF_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pElfWalkLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\ELF_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pElfWalkRBitmap[i]);
			}
		}
		/*--------------------------------------------------------------------------------*/
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\jpg\\FIGHTER.jpg", 0, 0, &m_pFighterPortraitBitmap);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\FIGHTER_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pFighterIdleLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\FIGHTER_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pFighterIdleRBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_WALK_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\FIGHTER_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pFighterWalkLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\FIGHTER_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pFighterWalkRBitmap[i]);
			}
		}
		/*--------------------------------------------------------------------------------*/
		hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Image\\jpg\\SORCERESS.jpg", 0, 0, &m_pSorceressPortraitBitmap);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\SORCERESS_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pSorceressIdleLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\SORCERESS_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pSorceressIdleRBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_WALK_NUM; i++)
			{
				wchar_t path_L[100];
				wsprintf(path_L, L".\\Image\\gif\\SORCERESS_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_L, 0, 0, &m_pSorceressWalkLBitmap[i]);
				wchar_t path_R[100];
				wsprintf(path_R, L".\\Image\\gif\\SORCERESS_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path_R, 0, 0, &m_pSorceressWalkRBitmap[i]);
			}
		}
		/*--------------------------------------------------------------------------------*/
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_WALK_NUM; i++)
			{
				wchar_t good[100];
				wsprintf(good, L".\\Image\\character\\good_%d.png", 2 * i + 2);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, good, 0, 0, &m_pGoodBitmap[i]);
				wchar_t bad[100];
				wsprintf(bad, L".\\Image\\character\\bad_%d.png", 2 * i + 2);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, bad, 0, 0, &m_pBadBitmap[i]);
				wchar_t player[100];
				wsprintf(player, L".\\Image\\character\\player_%d.png", 2 * i + 2);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, player, 0, 0, &m_pPlayerBitmap[i]);
			}
		}
		/*--------------------------------------------------------------------------------*/
	}
	return hr;
}

void SimpleGame::DiscardDeviceResources()
{
	// CreateDeviceResources 함수에서 생성한 모든 자원들을 반납
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pGroundBitmap);
	SafeRelease(&m_pForestBitmap);
	SafeRelease(&m_pTreesBitmap);
	SafeRelease(&m_pBushesBitmap);
	SafeRelease(&m_pStoneBitmap);
	SafeRelease(&m_pYellowBitmap);
	SafeRelease(&m_pYellowBitmapBrush);
	SafeRelease(&m_pRadialGradientBrush);
	SafeRelease(&m_pTextBoxBitmap);
	SafeRelease(&m_pChatIconBitmap);
	SafeRelease(&m_pFrameBitmap);

	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SafeRelease(&m_pHouseBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	SafeRelease(&m_pElfPortraitBitmap);
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleLBitmap[i]);
		SAFE_RELEASE(m_pElfIdleRBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkLBitmap[i]);
		SAFE_RELEASE(m_pElfWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	SafeRelease(&m_pFighterPortraitBitmap);
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleLBitmap[i]);
		SAFE_RELEASE(m_pFighterIdleRBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkLBitmap[i]);
		SAFE_RELEASE(m_pFighterWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	SafeRelease(&m_pSorceressPortraitBitmap);
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleLBitmap[i]);
		SAFE_RELEASE(m_pSorceressIdleRBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkLBitmap[i]);
		SAFE_RELEASE(m_pSorceressWalkRBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
	for (int i = 0; i < 4; i++)
	{
		SAFE_RELEASE(m_pGoodBitmap[i]);
		SAFE_RELEASE(m_pBadBitmap[i]);
		SAFE_RELEASE(m_pPlayerBitmap[i]);
	}
	/*--------------------------------------------------------------------------------*/
}

void DrawCharacter(ID2D1HwndRenderTarget* m_pRenderTarget, ID2D1Bitmap** character, int frame, D2D1_POINT_2F position, D2D1_POINT_2F ground)
{

	D2D1_SIZE_F bitmapSize;
	bitmapSize = character[frame / 3]->GetSize();
	m_pRenderTarget->DrawBitmap(character[frame++ / 3],
		D2D1::RectF(
			300 - bitmapSize.width / 2 + position.x,
			ground.y - bitmapSize.height,
			300 + bitmapSize.width / 2 + position.x,
			ground.y));
}

void SimpleGame::DisplayConversation(ID2D1Bitmap* m_pPortraitBitmap, D2D1_SIZE_F rtSize, int convNo)
{
	m_pRenderTarget->DrawBitmap(m_pTextBoxBitmap, D2D1::RectF(30, rtSize.height - 230, rtSize.width - 30, rtSize.height - 30), 0.5);
	m_pRenderTarget->DrawBitmap(m_pPortraitBitmap, D2D1::RectF(70, rtSize.height - 200, 210, rtSize.height - 60));
	m_pRenderTarget->DrawBitmap(m_pFrameBitmap, D2D1::RectF(70, rtSize.height - 200, 210, rtSize.height - 60));


	switch (convNo)
	{
	case CONV_PLAYER_01:
		m_pRenderTarget->DrawText(
			PlayerInfo,
			ARRAYSIZE(PlayerInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_PlayerMain1,
			ARRAYSIZE(sc_PlayerMain1) - PlayerMain1Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (PlayerMain1Len > 1)
		{
			PlayerMain1Len--;
			soundManager->play(5);
		}
		break;
	case CONV_FIGHTER_01:
		m_pRenderTarget->DrawText(
			FighterInfo,
			ARRAYSIZE(FighterInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_FighterMain1,
			ARRAYSIZE(sc_FighterMain1) - FighterMain1Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (FighterMain1Len > 1)
		{
			FighterMain1Len--;
			soundManager->play(6);
		}
		break;
	case CONV_FIGHTER_02:
		m_pRenderTarget->DrawText(
			FighterInfo,
			ARRAYSIZE(PlayerInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_FighterMain2,
			ARRAYSIZE(sc_FighterMain2) - FighterMain2Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (FighterMain2Len > 1)
		{
			FighterMain2Len--;
		}
		break;
	case CONV_FIGHTER_03:
		m_pRenderTarget->DrawText(
			FighterInfo,
			ARRAYSIZE(PlayerInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_FighterMain3,
			ARRAYSIZE(sc_FighterMain3) - FighterMain3Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (FighterMain3Len > 1)
		{
			FighterMain3Len--;
		}
		break;
	case CONV_FIGHTER_04:
		m_pRenderTarget->DrawText(
			FighterInfo,
			ARRAYSIZE(FighterInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_FighterMain4,
			ARRAYSIZE(sc_FighterMain4) - FighterMain4Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (FighterMain4Len > 1)
		{
			FighterMain4Len--;
			soundManager->play(7);
		}
		break;
	case CONV_FIGHTER_05:
		m_pRenderTarget->DrawText(
			FighterInfo,
			ARRAYSIZE(FighterInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_FighterMain5,
			ARRAYSIZE(sc_FighterMain5) - FighterMain5Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (FighterMain5Len > 1)
		{
			FighterMain5Len--;
			soundManager->play(8);
		}
		break;
	case CONV_ELF_01:
		m_pRenderTarget->DrawText(
			ElfInfo,
			ARRAYSIZE(ElfInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain1,
			ARRAYSIZE(sc_ElfMain1) - ElfMain1Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain1Len > 1)
		{
			ElfMain1Len--;
			soundManager->play(9);
		}
		break;
	case CONV_ELF_02:
		m_pRenderTarget->DrawText(
			ElfInfo,
			ARRAYSIZE(ElfInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain2,
			ARRAYSIZE(sc_ElfMain2) - ElfMain2Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain2Len > 1)
		{
			ElfMain2Len--;
		}
		break;
	case CONV_ELF_03:
		m_pRenderTarget->DrawText(
			PlayerInfo,
			ARRAYSIZE(PlayerInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain3,
			ARRAYSIZE(sc_ElfMain3) - ElfMain3Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain3Len > 1)
		{
			ElfMain3Len--;
		}
		break;
	case CONV_ELF_04:
		m_pRenderTarget->DrawText(
			PlayerInfo,
			ARRAYSIZE(PlayerInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain4,
			ARRAYSIZE(sc_ElfMain4) - ElfMain4Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain4Len > 1)
		{
			ElfMain4Len--;
		}
		break;
	case CONV_ELF_05:
		m_pRenderTarget->DrawText(
			ElfInfo,
			ARRAYSIZE(ElfInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain5,
			ARRAYSIZE(sc_ElfMain5) - ElfMain5Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain5Len > 1)
		{
			ElfMain5Len--;
		}
		break;
	case CONV_ELF_06:
		m_pRenderTarget->DrawText(
			ElfInfo,
			ARRAYSIZE(ElfInfo),
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 250, rtSize.width - 100, rtSize.height - 110),
			m_pWhiteBrush
		);
		m_pRenderTarget->DrawText(
			sc_ElfMain6,
			ARRAYSIZE(sc_ElfMain6) - ElfMain6Len,
			m_pTextFormat,
			D2D1::RectF(280, rtSize.height - 200, rtSize.width - 100, rtSize.height - 60),
			m_pWhiteBrush
		);
		if (ElfMain6Len > 1)
		{
			ElfMain6Len--;
		}
		break;
	}


}

HRESULT SimpleGame::OnRender()
{
	HRESULT hr = S_OK;
	hr = CreateDeviceResources(); // 항상 호출되며, 렌더타겟이 유효하면 아무일도 하지 않음.

	if (SUCCEEDED(hr))
	{ //렌더타겟이 유효함.

		m_pRenderTarget->BeginDraw(); //그리기 시작.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //변환행렬을 항등행렬로.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White)); // 창을 클리어.

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize(); //그리기 영역의 크기를 얻음.
		static float anim_time = 0.0f;
		ID2D1Layer* pLayer = NULL;

		if ((sceneNo == SCENE_OPENING) || (sceneNo == SCENE_OPENING_TO_PLAY))
		{
			if (sceneNo == SCENE_OPENING)
			{
				soundManager->play(0);
			}
			m_pRenderTarget->DrawBitmap(m_pOpeningBackgroundBitmap, D2D1::RectF(0, 0, rtSize.width, rtSize.height));

			hr = m_pRenderTarget->CreateLayer(NULL, &pLayer);

			static float localOpacity = 0.0;
			if (SUCCEEDED(hr))
			{
				if (localOpacity < 0.6)
				{
					localOpacity += 0.01f;
				}

				m_pRenderTarget->PushLayer(
					D2D1::LayerParameters(
						D2D1::InfiniteRect(),
						NULL,
						D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
						D2D1::IdentityMatrix(),
						localOpacity,
						m_pBlackBrush,
						D2D1_LAYER_OPTIONS_NONE),
					pLayer
				);
				m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBlackBrush);

				m_pRenderTarget->PopLayer();
			}
			SAFE_RELEASE(pLayer);

			
			if (localOpacity > 0.6)
			{
				m_pRenderTarget->DrawText(
					sc_OpeningStory,
					ARRAYSIZE(sc_OpeningStory) - OpeningTextLen,
					m_pTextFormat,
					D2D1::RectF(0, 0, rtSize.width, rtSize.height),
					m_pWhiteBrush
				);
				if (OpeningTextLen > 1)
				{
					OpeningTextLen--;
				}
			}
			if (sceneNo == SCENE_OPENING_TO_PLAY)
			{
				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateLayer(NULL, &pLayer);
					if (opacity < 1.0)
					{
						opacity += 0.01f;
					}

					m_pRenderTarget->PushLayer(
						D2D1::LayerParameters(
							D2D1::InfiniteRect(),
							NULL,
							D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
							D2D1::IdentityMatrix(),
							opacity,
							m_pBlackBrush,
							D2D1_LAYER_OPTIONS_NONE),
						pLayer
					);
					m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBlackBrush);

					m_pRenderTarget->PopLayer();
				}
				SAFE_RELEASE(pLayer);
				if(opacity >= 1.0)
				{
					sceneNo = SCENE_PLAY;

				}
			}
		}
		if (sceneNo == SCENE_PLAY || sceneNo == SCENE_PLAY_TO_ENDING)
		{
			soundManager->play(1);
			// 배경 그리기
			m_pRenderTarget->DrawBitmap(m_pForestBitmap, D2D1::RectF(0, 0, rtSize.width, rtSize.height));
			// 나무 그리기
			m_pRenderTarget->DrawBitmap(m_pTreesBitmap, D2D1::RectF(0, 300, rtSize.width, rtSize.height - 100));

			D2D1_SIZE_F bitmapSize = m_pGroundBitmap->GetSize();
			// 지면 그리기
			m_pRenderTarget->DrawBitmap(m_pGroundBitmap, D2D1::RectF(0, rtSize.height - 100, rtSize.width, rtSize.height));
			
			// 돌 그리기
			if (fighter->getState() == Fighter::StateID::STATE_ATTACK) {
				static D2D1_POINT_2F stonePosition = D2D1::Point2F(-100, rtSize.height - 530);
				static float v = -10;
				m_pRenderTarget->DrawBitmap(m_pStoneBitmap, D2D1::RectF(stonePosition.x, stonePosition.y, stonePosition.x + 100, stonePosition.y + 100));
				stonePosition.x += 10;
				v += 0.25;
				stonePosition.y += v;
				if (stonePosition.x > rtSize.width- 200)
				{
					Attack = false;
					stonePosition = D2D1::Point2F(-100, rtSize.height - 530);
					v = -10;
					houseHP -= 20;
					fighter->issueEvent(Fighter::EVENT_TERMINATE);
				}
			}

			//집 그리기
			if (houseHP > 80) {
				m_pRenderTarget->DrawBitmap(m_pHouseBitmap[0], D2D1::RectF(rtSize.width - 500, rtSize.height - 530, rtSize.width - 20, rtSize.height - 50));
			}
			else if (houseHP > 60) {
				m_pRenderTarget->DrawBitmap(m_pHouseBitmap[1], D2D1::RectF(rtSize.width - 500, rtSize.height - 530, rtSize.width - 20, rtSize.height - 50));
			}
			else if (houseHP > 40) {
				m_pRenderTarget->DrawBitmap(m_pHouseBitmap[2], D2D1::RectF(rtSize.width - 500, rtSize.height - 530, rtSize.width - 20, rtSize.height - 50));
			}
			else if (houseHP > 20) {
				m_pRenderTarget->DrawBitmap(m_pHouseBitmap[3], D2D1::RectF(rtSize.width - 500, rtSize.height - 530, rtSize.width - 20, rtSize.height - 50));
			}
			else if (houseHP > 0) {
				m_pRenderTarget->DrawBitmap(m_pHouseBitmap[4], D2D1::RectF(rtSize.width - 500, rtSize.height - 530, rtSize.width - 20, rtSize.height - 50));
			}
			else {
				sceneNo = SCENE_PLAY_TO_ENDING;
			}

			D2D1_POINT_2F ground = D2D1::Point2F(0, rtSize.height - 90);

			fighter->update();
			// NPC: FIGHTER
			if (fighter->getState() == Fighter::StateID::STATE_STOP|| 
				fighter->getState() == Fighter::StateID::STATE_TALK||
				fighter->getState() == Fighter::StateID::STATE_ATTACK)
			{
				if (fighter->getFaced() == RIGHT)
				{
					DrawCharacter(m_pRenderTarget, m_pFighterIdleRBitmap, fighter->getFrame(), fighter->getPosition(), ground);
					fighter->setFrame(fighter->getFrame() + 1);
				}
				else // fighter->getFaced() == LEFT
				{
					DrawCharacter(m_pRenderTarget, m_pFighterIdleLBitmap, fighter->getFrame(), fighter->getPosition(), ground);
					fighter->setFrame(fighter->getFrame() + 1);
				}
				if (fighter->getFrame() / 3 >= FIGHTER_IDLE_NUM) fighter->setFrame(0);
			}
			else if (fighter->getState() == Fighter::StateID::STATE_WALK|| 
				fighter->getState() == Fighter::StateID::STATE_MOVE)
			{
				if (fighter->getFaced() == RIGHT)
				{
					DrawCharacter(m_pRenderTarget, m_pFighterWalkRBitmap, fighter->getFrame(), fighter->getPosition(), ground);
					fighter->setFrame(fighter->getFrame() + 1);
				}
				else // fighter->getFaced() == LEFT
				{
					DrawCharacter(m_pRenderTarget, m_pFighterWalkLBitmap, fighter->getFrame(), fighter->getPosition(), ground);
					fighter->setFrame(fighter->getFrame() + 1);
				}
				if (fighter->getFrame() / 3 >= FIGHTER_WALK_NUM) fighter->setFrame(0);
			}

			// NPC: ELF
			if (ELFState == IDLE)
			{
				if (ELFFaced)
				{
					DrawCharacter(m_pRenderTarget, m_pElfIdleRBitmap, ELFFrame++, elfMove, ground);
				}
				else
				{
					DrawCharacter(m_pRenderTarget, m_pElfIdleLBitmap, ELFFrame++, elfMove, ground);
				}
				if (ELFFrame / 3 >= ELF_IDLE_NUM)
				{
					ELFFrame = 0;
					if (rand() % 2 == 0)
					{
						ELFState = IDLE;
					}
					else
					{
						ELFState = WALK;
					}
				}
			}
			else if (ELFState == WALK)
			{
				if (ELFFaced)
				{
					DrawCharacter(m_pRenderTarget, m_pElfWalkRBitmap, ELFFrame++, elfMove, ground);
					if (elfMove.x < 800)
					{
						elfMove.x += 3;
					}
					else { ELFFaced = LEFT; }
				}
				else
				{
					DrawCharacter(m_pRenderTarget, m_pElfWalkLBitmap, ELFFrame++, elfMove, ground);
					if (elfMove.x > 600)
					{
						elfMove.x -= 3;
					}
					else { ELFFaced = RIGHT; }
				}
				if (ELFFrame / 3 >= ELF_WALK_NUM)
				{
					ELFFrame = 0;
					if (rand() % 2 == 0)
					{
						ELFState = IDLE;
					}
					else
					{
						ELFState = WALK;
					}
				}
			}

			// 캐릭터 그리기
			if (SORCERESSState == IDLE)
			{
				soundManager->stop(4);
				if (SORCERESSFaced)
				{
					DrawCharacter(m_pRenderTarget, m_pSorceressIdleRBitmap, SORCERESSFrame++, playerMove, ground);
				}
				else
				{
					DrawCharacter(m_pRenderTarget, m_pSorceressIdleLBitmap, SORCERESSFrame++, playerMove, ground);
				}
				if (SORCERESSFrame / 3 >= SORCERESS_IDLE_NUM) SORCERESSFrame = 0;
			}
			else if (SORCERESSState == WALK)
			{
				soundManager->play(4);
				if (SORCERESSFaced)
				{
					DrawCharacter(m_pRenderTarget, m_pSorceressWalkRBitmap, SORCERESSFrame++, playerMove, ground);
					if (playerMove.x < 800)
					{
						playerMove.x += 3;
					}
				}
				else
				{
					DrawCharacter(m_pRenderTarget, m_pSorceressWalkLBitmap, SORCERESSFrame++, playerMove, ground);
					if (playerMove.x > -250)
					{
						playerMove.x -= 3;
					}
				}
				if (SORCERESSFrame / 3 >= SORCERESS_WALK_NUM) SORCERESSFrame = 0;
			}

			if (fighter->getPosition().x + 100 > playerMove.x && fighter->getPosition().x - 100 < playerMove.x)
			{
				bitmapSize = m_pChatIconBitmap->GetSize();
				m_pRenderTarget->DrawBitmap(m_pChatIconBitmap,
					D2D1::RectF(
						300 - bitmapSize.width / 2 + fighter->getPosition().x,
						ground.y - bitmapSize.height - 230,
						300 + bitmapSize.width / 2 + fighter->getPosition().x,
						ground.y - 230));
				fighter->setChatTrigger(1);
			}
			else { fighter->setChatTrigger(0); }

			if (elfMove.x + 100 > playerMove.x && elfMove.x - 100 < playerMove.x)
			{
				bitmapSize = m_pChatIconBitmap->GetSize();
				m_pRenderTarget->DrawBitmap(m_pChatIconBitmap,
					D2D1::RectF(
						300 - bitmapSize.width / 2 + elfMove.x,
						ground.y - bitmapSize.height - 230,
						300 + bitmapSize.width / 2 + elfMove.x,
						ground.y - 230));
				ELFChatTrigger = 1;
			}
			else { ELFChatTrigger = 0; }

			if (skewAngle1 > 15) { Bushes1Face = LEFT; }
			else if (skewAngle1 < -15) { Bushes1Face = RIGHT; }
			if (Bushes1Face == RIGHT) { skewAngle1 += 0.3f; }
			else if (Bushes1Face == LEFT) { skewAngle1 -= 0.3f; }

			if (skewAngle2 > 15) { Bushes2Face = LEFT; }
			else if (skewAngle2 < -15) { Bushes2Face = RIGHT; }
			if (Bushes2Face == RIGHT) { skewAngle2 += 0.3f; }
			else if (Bushes2Face == LEFT) { skewAngle2 -= 0.15f; }

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Skew(skewAngle1, 0, D2D1::Point2F(rtSize.width, rtSize.height)));
			m_pRenderTarget->DrawBitmap(m_pBushesBitmap, D2D1::RectF(-100, rtSize.height - 300, rtSize.width + 300, rtSize.height));
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Skew(skewAngle2, 0, D2D1::Point2F(rtSize.width, rtSize.height)));
			m_pRenderTarget->DrawBitmap(m_pBushesBitmap, D2D1::RectF(-300, rtSize.height - 400, rtSize.width + 100, rtSize.height), 0.7f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);

			// LightningBug
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			float length = m_Animation.GetValue(anim_time);

			D2D1_POINT_2F point;
			D2D1_POINT_2F tangent;

			// 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
			m_pPathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

			D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(50 + point.x, 50 + point.y);
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(((skewAngle1 + 85.0f) / 200.0f), ((skewAngle1 + 85.0f) / 200.0f));
			m_pRenderTarget->SetTransform(scale * translation);
			m_pRenderTarget->FillGeometry(m_pRectGeo, m_pYellowBitmapBrush, m_pRadialGradientBrush);
			if (sceneNo == SCENE_PLAY) {
				if (SUCCEEDED(hr))
				{
					m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
					if (opacity > 0.0)
					{
						opacity -= 0.01f;
					}

					m_pRenderTarget->PushLayer(
						D2D1::LayerParameters(
							D2D1::InfiniteRect(),
							NULL,
							D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
							D2D1::IdentityMatrix(),
							opacity,
							m_pBlackBrush,
							D2D1_LAYER_OPTIONS_NONE),
						pLayer
					);
					m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBlackBrush);

					m_pRenderTarget->PopLayer();
				}
				SAFE_RELEASE(pLayer);
			}
			if (opacity <= 0)
			{
				switch (ConversationDisplayed)
				{
				case TRUE:
					switch (convNo)
					{
					case CONV_PLAYER_01:
						DisplayConversation(m_pSorceressPortraitBitmap, rtSize, CONV_PLAYER_01);
						break;
					case CONV_FIGHTER_01:
						DisplayConversation(m_pFighterPortraitBitmap, rtSize, CONV_FIGHTER_01);
						break;
					case CONV_FIGHTER_02:
						DisplayConversation(m_pSorceressPortraitBitmap, rtSize, CONV_FIGHTER_02);
						break;
					case CONV_FIGHTER_03:
						DisplayConversation(m_pSorceressPortraitBitmap, rtSize, CONV_FIGHTER_03);
						break;
					case CONV_FIGHTER_04:
						DisplayConversation(m_pFighterPortraitBitmap, rtSize, CONV_FIGHTER_04);
						break;
					case CONV_FIGHTER_05:
						DisplayConversation(m_pFighterPortraitBitmap, rtSize, CONV_FIGHTER_05);
						break;
					case CONV_ELF_01:
						DisplayConversation(m_pElfPortraitBitmap, rtSize, CONV_ELF_01);
						break;
					case CONV_ELF_02:
						DisplayConversation(m_pElfPortraitBitmap, rtSize, CONV_ELF_02);
						break;
					case CONV_ELF_03:
						DisplayConversation(m_pSorceressPortraitBitmap, rtSize, CONV_ELF_03);
						break;
					case CONV_ELF_04:
						DisplayConversation(m_pSorceressPortraitBitmap, rtSize, CONV_ELF_04);
						break;
					case CONV_ELF_05:
						DisplayConversation(m_pElfPortraitBitmap, rtSize, CONV_ELF_05);
						break;
					case CONV_ELF_06:
						DisplayConversation(m_pElfPortraitBitmap, rtSize, CONV_ELF_06);
						break;
					default:
						break;
					}
					break;
				case FALSE:
					break;
				default:
					break;
				}
			}
			if (sceneNo == SCENE_PLAY_TO_ENDING)
			{
				m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //변환행렬을 항등행렬로.
				if (SUCCEEDED(hr))
				{
					if (opacity < 1.0)
					{
						opacity += 0.01f;
					}

					m_pRenderTarget->PushLayer(
						D2D1::LayerParameters(
							D2D1::InfiniteRect(),
							NULL,
							D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
							D2D1::IdentityMatrix(),
							opacity,
							m_pBlackBrush,
							D2D1_LAYER_OPTIONS_NONE),
						pLayer
					);
					m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBlackBrush);

					m_pRenderTarget->PopLayer();
				}
				SAFE_RELEASE(pLayer);
				if (opacity >= 1.0)
				{
					sceneNo = SCENE_ENDING;
				}
			}
		}
		if (sceneNo == SCENE_ENDING) {
			m_pRenderTarget->DrawBitmap(m_pEndingBackgroundBitmap, D2D1::RectF(0, 0, rtSize.width, rtSize.height));
			if (SUCCEEDED(hr))
			{
				m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
				if (opacity > 0.0)
				{
					opacity -= 0.01f;
				}

				m_pRenderTarget->PushLayer(
					D2D1::LayerParameters(
						D2D1::InfiniteRect(),
						NULL,
						D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
						D2D1::IdentityMatrix(),
						opacity,
						m_pBlackBrush,
						D2D1_LAYER_OPTIONS_NONE),
					pLayer
				);
				m_pRenderTarget->FillRectangle(D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBlackBrush);

				m_pRenderTarget->PopLayer();
			}
			SAFE_RELEASE(pLayer);
		}
		hr = m_pRenderTarget->EndDraw();  //그리기를 수행함. 성공하면 S_OK를 리턴함.

	// 애니메이션의 끝에 도달하면 다시 처음으로 되돌려서 반복되도록 함.
		if (anim_time >= m_Animation.GetDuration())
		{
			anim_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			anim_time += elapsedTime;
		}
	}

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
			}
			case WM_KEYDOWN:
			{
				switch (wParam) // 음소거 기능
				{
				case 'm':
				case 'M':
					if (volume > 0.0)
					{
						volume = 0.0;
						soundManager->volume(volume);
					}
					else 
					{
						volume = 1.0;
						soundManager->volume(volume);
					}
					break;
				}
				switch (sceneNo)
				{
				case SCENE_OPENING:
					switch (wParam)
					{
					case VK_RETURN:
						soundManager->stop(0);
						soundManager->play(2);
						sceneNo = SCENE_OPENING_TO_PLAY;
						ConversationDisplayed = TRUE;
						break;
					}
				case SCENE_PLAY:
					switch (ConversationDisplayed)
					{
					case TRUE:
						switch (convNo)
						{
						case CONV_PLAYER_01:
							if (PlayerMain1Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									ConversationDisplayed = FALSE;
									soundManager->reset(5);
									soundManager->reset(6);
									soundManager->reset(7);
									break;
								}
							}
							break;
						case CONV_FIGHTER_01:
							if (FighterMain1Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									convNo = CONV_FIGHTER_02;
									FighterMain1Len = ARRAYSIZE(sc_FighterMain1);
									break;
								}
							}
							break;
						case CONV_FIGHTER_02:
							if (FighterMain2Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									convNo = CONV_FIGHTER_03;
									FighterMain2Len = ARRAYSIZE(sc_FighterMain2);
									break;
								}
							}
							break;
						case CONV_FIGHTER_03:
							if (FighterMain3Len == 1)
							{
								switch (wParam)
								{
								case 'y':
								case 'Y':
									convNo = CONV_FIGHTER_04;
									FighterMain3Len = ARRAYSIZE(sc_FighterMain3);
									fighter->issueEvent(Fighter::EventID::EVENT_YES);
									break;
								case 'n':
								case 'N':
									convNo = CONV_FIGHTER_05;
									FighterMain3Len = ARRAYSIZE(sc_FighterMain3);
									fighter->issueEvent(Fighter::EventID::EVENT_NO);
									break;
								}
							}
							break;
						case CONV_FIGHTER_04:
							if (FighterMain4Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									ConversationDisplayed = FALSE;
									soundManager->reset(5);
									soundManager->reset(6);
									soundManager->reset(7);
									FighterMain4Len = ARRAYSIZE(sc_FighterMain4);
									break;
								}
							}
							break;
						case CONV_FIGHTER_05:
							if (FighterMain5Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									ConversationDisplayed = FALSE;
									soundManager->reset(5);
									soundManager->reset(6);
									soundManager->reset(7);
									FighterMain5Len = ARRAYSIZE(sc_FighterMain5);
									break;
								}
							}
							break;
						case CONV_ELF_01:
							switch (wParam)
							{
							case VK_RETURN:
								convNo = CONV_ELF_02;
								ElfMain1Len = ARRAYSIZE(sc_ElfMain1);
								break;
							}
							break;
						case CONV_ELF_02:
							switch (wParam)
							{
							case VK_RETURN:
								convNo = CONV_ELF_03;
								ElfMain2Len = ARRAYSIZE(sc_ElfMain2);
								break;
							}
							break;
						case CONV_ELF_03:
							switch (wParam)
							{
							case VK_RETURN:
								convNo = CONV_ELF_04;
								ElfMain3Len = ARRAYSIZE(sc_ElfMain3);
								break;
							}
							break;
						case CONV_ELF_04:
							switch (wParam)
							{
							case 'y':
							case 'Y':
								convNo = CONV_ELF_05;
								ElfMain4Len = ARRAYSIZE(sc_ElfMain4);
								break;
							case 'n':
							case 'N':
								convNo = CONV_ELF_06;
								ElfMain4Len = ARRAYSIZE(sc_ElfMain4);
								break;
							}
							break;
						case CONV_ELF_05:
							if (ElfMain5Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									ConversationDisplayed = FALSE;
									soundManager->reset(5);
									soundManager->reset(6);
									soundManager->reset(7);
									ElfMain5Len = ARRAYSIZE(sc_ElfMain5);
									break;
								}
							}
							break;
						case CONV_ELF_06:
							if (ElfMain6Len == 1)
							{
								switch (wParam)
								{
								case VK_RETURN:
									ConversationDisplayed = FALSE;
									soundManager->reset(5);
									soundManager->reset(6);
									soundManager->reset(7);
									ElfMain6Len = ARRAYSIZE(sc_ElfMain6);
									break;
								}
							}
							break;
						}
						break;
					case FALSE:
						switch (wParam)
						{
						case VK_LEFT:
							if (sceneNo == SCENE_PLAY && ConversationDisplayed == FALSE)
							{
								if (SORCERESSState == IDLE) { SORCERESSFrame = 0; }
								SORCERESSFaced = LEFT;
								SORCERESSState = WALK;
							}
							break;
						case VK_RIGHT:
							if (sceneNo == SCENE_PLAY && ConversationDisplayed == FALSE)
							{
								if (SORCERESSState == IDLE) { SORCERESSFrame = 0; }
								SORCERESSFaced = RIGHT;
								SORCERESSState = WALK;
							}
							break;
						case 'f':
						case 'F':
							if (fighter->getChatTrigger() == TRUE) {
								FighterMain1Len = ARRAYSIZE(sc_FighterMain1);
								convNo = CONV_FIGHTER_01;
								ConversationDisplayed = TRUE;
								SORCERESSState = IDLE;
								fighter->issueEvent(Fighter::EventID::EVENT_TALK);
							}
							else if (ELFChatTrigger == TRUE) {
								ElfMain1Len = ARRAYSIZE(sc_ElfMain1);
								convNo = CONV_ELF_01;
								ConversationDisplayed = TRUE;
								SORCERESSState = IDLE;
							}
							break;
						}
						break;
					}
					break;
				}
			}
			result = 0;
			wasHandled = true;
			break;
			case WM_KEYUP:
			{
				switch (wParam)
				{
				case VK_UP:
					volume += volume >= 1.0 ? 0.0 : 0.1;
					soundManager->volume(volume);
					break;
				case VK_DOWN:
					volume -= volume <= 0.0 ? 0.0 : 0.1;
					soundManager->volume(volume);
					break;
				case VK_LEFT:
					if (sceneNo == SCENE_PLAY && ConversationDisplayed == FALSE)
					{
						if (SORCERESSState == WALK) { SORCERESSFrame = 0; }
						SORCERESSFaced = LEFT;
						SORCERESSState = IDLE;
					}
					break;
				case VK_RIGHT:
					if (sceneNo == SCENE_PLAY && ConversationDisplayed == FALSE)
					{
						if (SORCERESSState == WALK) { SORCERESSFrame = 0; }
						SORCERESSFaced = RIGHT;
						SORCERESSState = IDLE;
					}
					break;
				case VK_SPACE:
					break;
				case 't':
				case 'T':
					break;
				}
			}
			result = 0;
			wasHandled = true;
			break;
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