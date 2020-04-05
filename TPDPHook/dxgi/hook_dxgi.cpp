#include <Windows.h>
#include <filesystem>
#include <mutex>
#include "hook_dxgi.h"
#include "../log.h"

typedef HRESULT(WINAPI *DXGICall)(REFIID riid, void **ppFactory);
typedef HRESULT(WINAPI *DXGICall2)(UINT Flags, REFIID riid, void **ppFactory);

DXGICall g_dxgi_func = nullptr;
DXGICall g_dxgi_func1 = nullptr;
DXGICall2 g_dxgi_func2 = nullptr;

std::once_flag dxgi_initialized;

static void init_dxgi()
{
	wchar_t buf[MAX_PATH] = { 0 };
	GetSystemDirectoryW(buf, MAX_PATH);
	std::filesystem::path syspath = buf;

	auto path = syspath / L"dxgi.dll";
	auto hmodule = LoadLibraryW(path.c_str());

	if(hmodule == nullptr)
		log_fatal(L"Could not load " + path.wstring());

	g_dxgi_func = (DXGICall)GetProcAddress(hmodule, "CreateDXGIFactory");
	if(g_dxgi_func == nullptr)
		log_fatal(L"CreateDXGIFactory missing");

	g_dxgi_func1 = (DXGICall)GetProcAddress(hmodule, "CreateDXGIFactory1");
	if(g_dxgi_func1 == nullptr)
		log_fatal(L"CreateDXGIFactory1 missing");

	g_dxgi_func2 = (DXGICall2)GetProcAddress(hmodule, "CreateDXGIFactory2");
	if(g_dxgi_func2 == nullptr)
		log_warn(L"CreateDXGIFactory2 missing");

	log_debug(L"DXGI initialized.");
}

extern "C" HRESULT WINAPI CreateDXGIFactory(REFIID riid, void **ppFactory)
{
	std::call_once(dxgi_initialized, &init_dxgi);

	log_debug(L"Redirecting CreateDXGIFactory.");

	return g_dxgi_func(riid, ppFactory);
}

extern "C" HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void **ppFactory)
{
	std::call_once(dxgi_initialized, &init_dxgi);

	log_debug(L"Redirecting CreateDXGIFactory1.");

	return g_dxgi_func1(riid, ppFactory);
}

extern "C" HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory)
{
	std::call_once(dxgi_initialized, &init_dxgi);

	auto msg = (g_dxgi_func2 != nullptr) ? L"Redirecting CreateDXGIFactory2." : L"Redirecting CreateDXGIFactory2 to CreateDXGIFactory1.";
	log_debug(msg);

	if(g_dxgi_func2 != nullptr)
		return g_dxgi_func2(Flags, riid, ppFactory);
	else
		return g_dxgi_func1(riid, ppFactory); // This probably works?
}
