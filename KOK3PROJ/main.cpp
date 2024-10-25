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
	wndDesc.title = "King of Kings 3";
	wndDesc.width = 800;
	wndDesc.height = 600;
	wndDesc.hInstance = hInstance;
	wndDesc.fullScreen = true;
	wndDesc.nCmdShow = nCmdShow;
	CHECK_ASSERT(engine.InitWindowDevice(&wndDesc), "Error Init window desc");
	CHECK_ASSERT(engine.InitRenderDevice(), "Error Init render desc");

	int wParam = engine.messageWindow();
	engine.Release();

	return wParam;
}