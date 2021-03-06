#include <Windows.h>
#include <d3d11.h>
#include <d3dcommon.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		msg = WM_QUIT;
		break;
	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "My Window!");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "Kaboom!");
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
float FloatPulsatorTest(float changeDelta)
{
	static unsigned char count = 0;
	if (count % 100 == 0)
	{
		// Subtract ammount pulsed so far every mod 100 calls
		changeDelta = (-1 * changeDelta * count);
		count = 0;
	}
	count++;
	return changeDelta;
}

// Present back buffer.
void EndFrame(IDXGISwapChain *swapChain)
{
	swapChain->Present(1u, 0u);
}

void ClearRenderTargetToColor(float r, float g, float b, ID3D11RenderTargetView *rndrTargView, ID3D11DeviceContext *deviceContext)
{
	float clearToColor[] = { r, g, b, 1.0f };
	deviceContext->ClearRenderTargetView(rndrTargView, clearToColor);
}

int CALLBACK WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow)
{
	const char* pClassName = "myWindowClass";
	// register window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	// create window instance
	HWND hWnd = CreateWindowEx(
		0, pClassName,
		"Hardware Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480,
		nullptr, nullptr, hInstance, nullptr
	);

	ShowWindow(hWnd, SW_SHOW);

	DXGI_RATIONAL refreshRate = { 0 };
	refreshRate.Numerator = 0;
	refreshRate.Denominator = 0;

	DXGI_MODE_DESC modeDesc = { 0 };
	modeDesc.Width = 0;
	modeDesc.Height = 0;
	modeDesc.RefreshRate = refreshRate;
	modeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc = { 0 };
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapDesc = { 0 };
	swapDesc.BufferDesc = modeDesc;
	swapDesc.SampleDesc = sampleDesc;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1; // 1 front and 1 back buffer.
	swapDesc.OutputWindow = hWnd;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = 0;

	// handle creation flags for device context
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// Enable debug layer in debug mode.
		creationFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	IDXGISwapChain *swapChain;
	ID3D11Device *d3dDevice;
	ID3D11DeviceContext *d3dDeviceContext;

	ID3D11Resource *backBuffer = nullptr;
	ID3D11RenderTargetView *renderTarget = nullptr;

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags,
		featureLevels, 2, D3D11_SDK_VERSION, &swapDesc, &swapChain, &d3dDevice, NULL,
		&d3dDeviceContext);

	// Set up render target from back buffer
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer);
	d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);
	backBuffer->Release(); // Only needed to create render target view. Can be released after creation.

	MSG msg = { 0 };
	BOOL gResult;

	float pulseColor = 0.0f;
	//***********************'Game' loop*****************************
	while (msg.message != WM_QUIT)
	{
		// message pump
		while ((gResult = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//**************DRAWING TEST*****************

		pulseColor += FloatPulsatorTest(0.01f);

		ClearRenderTargetToColor(pulseColor, 0.1f, 0.5f, renderTarget, d3dDeviceContext);
		EndFrame(swapChain); // Flip Buffers
		//**************END TEST*******************
	}
	
	if (gResult == -1)
	{
		if (d3dDevice != nullptr)
		{
			d3dDevice->Release();
		}
		if (d3dDeviceContext != nullptr)
		{
			d3dDeviceContext->Release();
		}
		if (d3dDevice != nullptr)
		{
			swapChain->Release();
		}
		if (renderTarget != nullptr)
		{
			renderTarget->Release();
		}
		return -1;		
	}
	else
	{
		if (d3dDevice != nullptr)
		{
			d3dDevice->Release();
		}
		if (d3dDeviceContext != nullptr)
		{
			d3dDeviceContext->Release();
		}
		if (d3dDevice != nullptr)
		{
			swapChain->Release();
		}
		if (renderTarget != nullptr)
		{
			renderTarget->Release();
		}
		return msg.wParam;
	}	
}