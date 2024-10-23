#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx11d.lib")
#else
#pragma comment(lib, "d3dx11.lib")
#endif
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment(lib, "d3dx10.lib")

#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <xnamath.h>