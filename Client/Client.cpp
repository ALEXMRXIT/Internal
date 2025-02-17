#include "framework.h"
#include "Engine.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WindowDescription wndDesc;
	ZeroMemory(&wndDesc, sizeof(WindowDescription));
	wndDesc.title = "Internal Engine - Beta Test v1.0.3 DirectX 11";
	wndDesc.hInstance = hInstance;
	wndDesc.fullScreen = false;
	wndDesc.nCmdShow = nCmdShow;
	engine.GetSupportedResolutions(DXGI_FORMAT_R10G10B10A2_UNORM);
	engine.InitWindowDevice(&wndDesc);
	engine.InitRenderDevice();

	int wParam = engine.messageWindow();
	engine.Release();

	return wParam;
}