#pragma once
#include <Windows.h>

extern "C"
{

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void **ppFactory);
HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void **ppFactory);
HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory);

}
