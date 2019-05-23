#pragma warning(disable : 4996)
#include "SimpleGame.h"
#include "Animation.h"
// playerState
unsigned int ELFState = IDLE;
unsigned int ELFFrame = 0;
unsigned int ELFFaced = LEFT;

unsigned int FIGHTERState = WALK;
unsigned int FIGHTERFrame = 0;
unsigned int FIGHTERFaced = LEFT;

unsigned int SORCERESSState = IDLE;
unsigned int SORCERESSFrame = 0;
unsigned int SORCERESSFaced = RIGHT;

unsigned int Bushes1Face = RIGHT;
unsigned int Bushes2Face = RIGHT;

D2D1_POINT_2F playerMove = D2D1::Point2F(0, 0);
D2D1_POINT_2F elfMove = D2D1::Point2F(600, 0);
D2D1_POINT_2F fighterMove = D2D1::Point2F(500, 0);

float skewAngle1 = 3;
float skewAngle2 = -8;


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
	m_pForestBitmap(NULL),
	m_pTreesBitmap(NULL),
	m_pBushesBitmap(NULL),
	m_pYellowBitmap(NULL),
	m_pYellowBitmapBrush(NULL),
	m_pRectGeo(NULL),
	m_pRadialGradientBrush(NULL),
	m_pElfIdleLBitmap(),
	m_pElfIdleRBitmap(),
	m_pElfWalkLBitmap(),
	m_pElfWalkRBitmap(),
	m_pFighterIdleLBitmap(),
	m_pFighterIdleRBitmap(),
	m_pFighterWalkLBitmap(),
	m_pFighterWalkRBitmap(),
	m_pSorceressIdleLBitmap(),
	m_pSorceressIdleRBitmap(),
	m_pSorceressWalkLBitmap(),
	m_pSorceressWalkRBitmap()
{
}

SimpleGame::~SimpleGame()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);

	SAFE_RELEASE(m_pWICFactory);

	SAFE_RELEASE(m_pGroundBitmap);
	SAFE_RELEASE(m_pForestBitmap);
	SAFE_RELEASE(m_pTreesBitmap);
	SAFE_RELEASE(m_pBushesBitmap);

	// LightningBug
	SAFE_RELEASE(m_pYellowBitmap);
	SAFE_RELEASE(m_pYellowBitmapBrush);
	SAFE_RELEASE(m_pRectGeo);
	SAFE_RELEASE(m_pRadialGradientBrush);

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

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfIdleLBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfWalkLBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterIdleLBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterWalkLBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_IDLE_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressIdleLBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_WALK_L\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressWalkLBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfIdleRBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < ELF_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\ELF_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pElfWalkRBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterIdleRBitmap[i]);
			}
		}
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < FIGHTER_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\FIGHTER_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pFighterWalkRBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_IDLE_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressIdleRBitmap[i]);
			}
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < SORCERESS_WALK_NUM; i++)
			{
				wchar_t path[100];
				wsprintf(path, L".\\Image\\gif\\SORCERESS_WALK_R\\%d.png", i);
				hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, path, 0, 0, &m_pSorceressWalkRBitmap[i]);
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
	SafeRelease(&m_pForestBitmap);
	SafeRelease(&m_pTreesBitmap);
	SafeRelease(&m_pBushesBitmap);
	SafeRelease(&m_pYellowBitmap);
	SafeRelease(&m_pYellowBitmapBrush);
	SafeRelease(&m_pRadialGradientBrush);
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleLBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkLBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleLBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkLBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleLBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkLBitmap[i]);
	}
	for (int i = 0; i < ELF_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pElfIdleRBitmap[i]);
	}
	for (int i = 0; i < ELF_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pElfWalkRBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterIdleRBitmap[i]);
	}
	for (int i = 0; i < FIGHTER_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pFighterWalkRBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_IDLE_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressIdleRBitmap[i]);
	}
	for (int i = 0; i < SORCERESS_WALK_NUM; i++)
	{
		SAFE_RELEASE(m_pSorceressWalkRBitmap[i]);
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

		// 배경 그리기
		m_pRenderTarget->DrawBitmap(m_pForestBitmap, D2D1::RectF(0, 0, rtSize.width, rtSize.height));
		// 나무 그리기
		m_pRenderTarget->DrawBitmap(m_pTreesBitmap, D2D1::RectF(0, 300, rtSize.width, rtSize.height-100));

		D2D1_SIZE_F bitmapSize = m_pGroundBitmap->GetSize();
		// 지면 그리기
		m_pRenderTarget->DrawBitmap(m_pGroundBitmap, D2D1::RectF(0, rtSize.height-100, rtSize.width, rtSize.height));

		D2D1_POINT_2F ground = D2D1::Point2F(0, rtSize.height - 90);

		// NPC: FIGHTER
		if (FIGHTERState == IDLE)
		{
			if (FIGHTERFaced)
			{
				bitmapSize = m_pFighterIdleRBitmap[FIGHTERFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pFighterIdleRBitmap[FIGHTERFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + fighterMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + fighterMove.x,
						ground.y));
			}
			else
			{
				bitmapSize = m_pFighterIdleLBitmap[FIGHTERFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pFighterIdleLBitmap[FIGHTERFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + fighterMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + fighterMove.x,
						ground.y));
			}
			if (FIGHTERFrame / 3 >= FIGHTER_IDLE_NUM) FIGHTERFrame = 0;
		}
		else if (FIGHTERState == WALK)
		{
			if (FIGHTERFaced)
			{
				bitmapSize = m_pFighterWalkRBitmap[FIGHTERFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pFighterWalkRBitmap[FIGHTERFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + fighterMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + fighterMove.x,
						ground.y));
				if (fighterMove.x < 800)
				{
					fighterMove.x += 1;
				}
				else { FIGHTERFaced = LEFT; }
			}
			else
			{
				bitmapSize = m_pFighterWalkLBitmap[FIGHTERFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pFighterWalkLBitmap[FIGHTERFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + fighterMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + fighterMove.x,
						ground.y));
				if (fighterMove.x > -200)
				{
					fighterMove.x -= 1;
				}
				else { FIGHTERFaced = RIGHT; }
			}
			if (FIGHTERFrame / 3 >= FIGHTER_WALK_NUM) FIGHTERFrame = 0;
		}

		// NPC: ELF
		if (ELFState == IDLE)
		{
			if (ELFFaced)
			{
				bitmapSize = m_pElfIdleRBitmap[ELFFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pElfIdleRBitmap[ELFFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + elfMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + elfMove.x,
						ground.y));
			}
			else
			{
				bitmapSize = m_pElfIdleLBitmap[ELFFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pElfIdleLBitmap[ELFFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + elfMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + elfMove.x,
						ground.y));
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
				bitmapSize = m_pElfWalkRBitmap[ELFFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pElfWalkRBitmap[ELFFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + elfMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + elfMove.x,
						ground.y));
				if (elfMove.x < 800)
				{
					elfMove.x += 3;
				}
				else { ELFFaced = LEFT; }
			}
			else
			{
				bitmapSize = m_pElfWalkLBitmap[ELFFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pElfWalkLBitmap[ELFFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + elfMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + elfMove.x,
						ground.y));
				if (elfMove.x > -200)
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
			if (SORCERESSFaced)
			{
				bitmapSize = m_pSorceressIdleRBitmap[SORCERESSFrame/3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pSorceressIdleRBitmap[SORCERESSFrame++/3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + playerMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + playerMove.x,
						ground.y));
			}
			else
			{
				bitmapSize = m_pSorceressIdleLBitmap[SORCERESSFrame/3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pSorceressIdleLBitmap[SORCERESSFrame++/3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + playerMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + playerMove.x,
						ground.y));
			}
			if (SORCERESSFrame/3 >= SORCERESS_IDLE_NUM) SORCERESSFrame = 0;
		}
		else if (SORCERESSState == WALK)
		{
			if (SORCERESSFaced)
			{
				bitmapSize = m_pSorceressWalkRBitmap[SORCERESSFrame / 3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pSorceressWalkRBitmap[SORCERESSFrame++ / 3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + playerMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + playerMove.x,
						ground.y));
				if (playerMove.x < 800)
				{
					playerMove.x += 3;
				}
			}
			else
			{
				bitmapSize = m_pSorceressWalkLBitmap[SORCERESSFrame/3]->GetSize();
				m_pRenderTarget->DrawBitmap(m_pSorceressWalkLBitmap[SORCERESSFrame++/3],
					D2D1::RectF(
						300 - bitmapSize.width / 2 + playerMove.x,
						ground.y - bitmapSize.height,
						300 + bitmapSize.width / 2 + playerMove.x,
						ground.y));
				if (playerMove.x > -250)
				{
					playerMove.x -= 3;
				}
			}
			if (SORCERESSFrame/3 >= SORCERESS_WALK_NUM) SORCERESSFrame = 0;
		}

		if (skewAngle1 > 15) { Bushes1Face = LEFT; }
		else if (skewAngle1 < -15) { Bushes1Face = RIGHT; }
		if (Bushes1Face==RIGHT) { skewAngle1 += 0.3f; }
		else if (Bushes1Face == LEFT) { skewAngle1 -= 0.3f; }

		if (skewAngle2 > 15) { Bushes2Face = LEFT; }
		else if (skewAngle2 < -15) { Bushes2Face = RIGHT; }
		if (Bushes2Face == RIGHT) { skewAngle2 += 0.3f; }
		else if (Bushes2Face == LEFT) { skewAngle2 -= 0.15f; }

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Skew(skewAngle1, 0, D2D1::Point2F(rtSize.width, rtSize.height)));
		m_pRenderTarget->DrawBitmap(m_pBushesBitmap, D2D1::RectF(-100, rtSize.height - 300, rtSize.width + 300, rtSize.height));
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Skew(skewAngle2, 0, D2D1::Point2F(rtSize.width, rtSize.height)));
		m_pRenderTarget->DrawBitmap(m_pBushesBitmap, D2D1::RectF(-300, rtSize.height - 400, rtSize.width + 100, rtSize.height),0.7, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		
		// LightningBug
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		static float anim_time = 0.0f;

		float length = m_Animation.GetValue(anim_time);

		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		// 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
		m_pPathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(50+point.x, 50 + point.y);
		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(((skewAngle1+85.0f)/200.0f), ((skewAngle1 + 85.0f) / 200.0f));
		m_pRenderTarget->SetTransform(scale* translation);
		m_pRenderTarget->FillGeometry(m_pRectGeo, m_pYellowBitmapBrush, m_pRadialGradientBrush);

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
				switch (wParam)
				{
				case VK_UP:
					break;
				case VK_DOWN:
					break;
				case VK_LEFT:
					if (SORCERESSState == IDLE) { SORCERESSFrame = 0; }
					SORCERESSFaced = LEFT;
					SORCERESSState = WALK;
					break;
				case VK_RIGHT:
					if (SORCERESSState == IDLE) { SORCERESSFrame = 0; }
					SORCERESSFaced = RIGHT;
					SORCERESSState = WALK;
					break;
				case VK_SPACE:
					break;
				case 'q':
				case 'Q':
				case 'w':
				case 'W':
				case 'e':
				case 'E':
				case 'r':
				case 'R':
				case 'a':
				case 'A':
				case 's':
				case 'S':
				case 'd':
				case 'D':
				case 'f':
				case 'F':
				case 'm':
				case 'M':
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
					break;
				case VK_DOWN:
					break;
				case VK_LEFT:
					if (SORCERESSState == WALK) { SORCERESSFrame = 0; }
					SORCERESSFaced = LEFT;
					SORCERESSState = IDLE;
					break;
				case VK_RIGHT:
					if (SORCERESSState == WALK) { SORCERESSFrame = 0; }
					SORCERESSFaced = RIGHT;
					SORCERESSState = IDLE;
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