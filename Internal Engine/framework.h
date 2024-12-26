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
#include <list>
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <stdarg.h>
#include <dbghelp.h>
#include <psapi.h>
#include <unordered_map>
#include <windowsx.h>
#include <ShellScalingAPI.h>

#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx11d.lib")
#else
#pragma comment(lib, "d3dx11.lib")
#endif
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "Shcore.lib")

#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <xnamath.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <d2d1.h>
#include <sstream>
#include <dwrite.h>
#include <dxerr.h>