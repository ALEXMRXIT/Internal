#include "framework.h"
#include "Engine.h"
#include "debug.h"

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
	wndDesc.fullScreen = true;
	wndDesc.nCmdShow = nCmdShow;
	engine.GetSupportedResolutions();
	engine.InitWindowDevice(&wndDesc);
	engine.InitRenderDevice();

	int wParam = engine.messageWindow();
	engine.Release();

	return wParam;
}