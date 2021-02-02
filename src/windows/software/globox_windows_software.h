#ifndef H_GLOBOX_WINDOWS_SOFTWARE
#define H_GLOBOX_WINDOWS_SOFTWARE

#include "globox.h"
#include "windows/globox_windows.h"
// windows includes
#include <windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include "windows/globox_windows_symbols.h"

struct globox_windows_software
{
	uint32_t globox_software_buffer_width;
	uint32_t globox_software_buffer_height;

	int globox_software_alpha;

	// DirectX types
	IDXGIFactory2* globox_software_dxgi_factory;
	IDXGIDevice* globox_software_dxgi_device;
	IDXGISwapChain1* globox_software_swapchain;
	ID3D11Device* globox_software_d3d11_device;

	// DirectComposition types
	IDCompositionDevice* globox_software_dcomp_device;
	IDCompositionTarget* globox_software_dcomp_target;
	IDCompositionVisual* globox_software_dcomp_visual;

	// Direct2D types
	ID2D1Factory2* globox_software_d2d_factory;
	ID2D1Device1* globox_software_d2d_device;
};

#endif
