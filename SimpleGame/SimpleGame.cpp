#include "SimpleGame.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �� ���μ����� ����ϴ� ���� ������ �߻��ϸ� �� ���μ����� �����ϵ��� �����.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL))) {
		{
			SimpleGame app;
			if (SUCCEEDED(app.Initialize())) {  // SimpleGame Ŭ������ instance �ʱ�ȭ
				app.RunMessageLoop();  // �޽������� ����
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
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL)
{
}

SimpleGame::~SimpleGame()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
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

HRESULT SimpleGame::Initialize() {
	HRESULT hr = CreateDeviceIndependentResources(); //��ġ ������ �ڿ����� �ʱ�ȭ��

	if (SUCCEEDED(hr)) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;  wcex.lpfnWndProc = SimpleGame::WndProc;
		wcex.cbClsExtra = 0;  wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;  wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;  wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"Kina Roguelike";
		RegisterClassEx(&wcex); // ������ Ŭ������ �����
		FLOAT dpiX, dpiY;
		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY); //������ �ý��� DPI�� ����
		m_hwnd = CreateWindow( // �����츦 ������
			L"Kina Roguelike", L"Kina Roguelike",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
			NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}
	return hr;
}

HRESULT SimpleGame::CreateDeviceIndependentResources()
{
	// ��ġ ������ �ڿ����� ������
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

HRESULT SimpleGame::CreateDeviceResources()
{
	// ��ġ ������ �ڿ���, �ϳ��� ����Ÿ��, �ΰ��� �귯�ø� ������
	HRESULT hr = S_OK;

	if (!m_pRenderTarget) { // ����Ÿ���� �̹� ��ȿ�ϸ� �������� ����
		// Ŭ���̾�Ʈ ������ ũ�⸦ ����.
		RECT rc;  GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// D2D ����Ÿ���� ������.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size), //Ŭ���̾�Ʈ ������ ���� ũ���.
			&m_pRenderTarget);
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(  // ȸ�� �귯�� ����
				D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(  //�Ķ��� �귯�� ����
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
	}
	return hr;
}

void SimpleGame::DiscardDeviceResources()
{
	// CreateDeviceResources �Լ����� ������ ��� �ڿ����� �ݳ�
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
}

HRESULT SimpleGame::OnRender()
{
	HRESULT hr = S_OK;
	hr = CreateDeviceResources(); // �׻� ȣ��Ǹ�, ����Ÿ���� ��ȿ�ϸ� �ƹ��ϵ� ���� ����.
	if (SUCCEEDED(hr)) { //����Ÿ���� ��ȿ��.
		m_pRenderTarget->BeginDraw(); //�׸��� ����.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //��ȯ����� �׵���ķ�.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White)); // â�� Ŭ����.

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize(); //�׸��� ������ ũ�⸦ ����.

		// ��� ���ڸ� �׸�.
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);
		for (int x = 0; x < width; x += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f), D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height), m_pLightSlateGrayBrush, 0.5f);
		}
		for (int y = 0; y < height; y += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
				D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)), m_pLightSlateGrayBrush, 0.5f);
		}
		// ȭ�� �߰��� �� �簢���� �׸�.
		D2D1_RECT_F rectangle1 = D2D1::RectF(
			rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f,
			rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f);
		D2D1_RECT_F rectangle2 = D2D1::RectF(
			rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f,
			rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f);
		// ù��° �簢���� ���θ� ȸ�� �귯�÷� ä���� �׸�.
		m_pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush);
		// �ι�° �簢���� �ܰ����� �Ķ��� �귯�÷� �׸�.
		m_pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush);
		hr = m_pRenderTarget->EndDraw();  //�׸��⸦ ������. �����ϸ� S_OK�� ������.
	}

	if (hr == D2DERR_RECREATE_TARGET) { // ����Ÿ���� ������ؾ� ��.
		// �����߿� D3D ��ġ�� �ҽǵ� ���,
		hr = S_OK;
		DiscardDeviceResources(); // ��ġ ���� �ڿ����� �ݳ���.
		// ������ OnRender �Լ��� ȣ��� ���� �������.
	}
	return hr;
}

void SimpleGame::OnResize(UINT width, UINT height)
{
	// â�� resize�� ��쿡 ����Ÿ�ٵ� �̿� �µ��� resize��.
	if (m_pRenderTarget) {
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
		// Resize�Լ��� ������ ���� ������, ����ó������ �ʾ���.
		// �ֳ��ϸ�, �����ϴ�  ��쿡, ������ EndDraw ȣ�⿡���� ������ ���̹Ƿ�.
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
			{  UINT width = LOWORD(lParam);  // �� â�� ũ��
			UINT height = HIWORD(lParam);
			pSimpleGame->OnResize(width, height);  // â�� ũ�⸦ �ٽ� ����
			}
			result = 0;  wasHandled = true;  break;
			case WM_DISPLAYCHANGE:
			{  InvalidateRect(hwnd, NULL, FALSE); //WM_PAINT�� �߻���Ŵ
			}
			result = 0;  wasHandled = true;  break;
			case WM_PAINT:
			{  pSimpleGame->OnRender();  // â�� �׸�
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