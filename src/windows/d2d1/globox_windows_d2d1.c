#include "globox.h"
#include "globox_error.h"
#include "windows/globox_windows.h"
// system includes
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
// windows includes
#include <windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dcommon.h>
#include "windows/globox_windows_symbols.h"
#include "windows/d2d1/globox_windows_d2d1.h"

void swapchain(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);

	HRESULT ok;

	DXGI_SAMPLE_DESC sample =
	{
		.Count = 1,
#if 0
		.Quality = 0,
#endif
	};

	DXGI_SWAP_CHAIN_DESC1 swapchain_info =
	{
		.Width = context->globox_software_buffer_width,
		.Height = context->globox_software_buffer_height,
		.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		.SampleDesc = sample,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		.AlphaMode = context->globox_software_alpha,
#if 0
		.Stereo = False,
		.Scaling = DXGI_SCALING_NONE,
		.Flags = 0,
#endif
	};

	ok = IDXGIFactory2_CreateSwapChainForComposition(
		context->globox_software_dxgi_factory,
		(IUnknown*) context->globox_software_dxgi_device,
		&swapchain_info,
		NULL,
		&(context->globox_software_swapchain));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_SWAPCHAIN_CREATE);
		return;
	}
}

void resize(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);

	// update bitmap info
	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	free(platform->globox_platform_argb);

	platform->globox_platform_argb =
		malloc(
			4
			* context->globox_software_buffer_width
			* context->globox_software_buffer_height);

	if (platform->globox_platform_argb == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	// create swap chain
	swapchain(globox);
}

void dcomp(struct globox* globox)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);

	// DirectX11 initialization for DirectComposition
	HRESULT ok;

	// create a factory
	ok = CreateDXGIFactory2(
		0,
		&IID_IDXGIFactory2,
		&(context->globox_software_dxgi_factory));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_FACTORY_CREATE);
		return;
	}

	// find a compatible adapter
	IDXGIAdapter* adapter;
	UINT i = 0;

	do
	{
		ok = IDXGIFactory2_EnumAdapters(
			context->globox_software_dxgi_factory,
			i,
			&adapter);

		// this error path includes the DXGI_ERROR_NOT_FOUND case
		// which occurs when reaching the end of the adapter list
		if (ok != S_OK)
		{
			if (ok == DXGI_ERROR_NOT_FOUND)
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_ADAPTERS_END);
			}
			else
			{
				globox_error_throw(
					globox,
					GLOBOX_ERROR_WINDOWS_ADAPTERS_LIST);
			}

			return;
		}

		// try creating the D3D11 device
		ok = D3D11CreateDevice(
			adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,  // don't provide D3D a software rasterizer
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			NULL,  // let D3D attempt to create feature levels
			0,     // don't require a specific feature level
			D3D11_SDK_VERSION,
			&(context->globox_software_d3d11_device),
			NULL,  // don't get the chosen feature level
			NULL); // don't get a device context handle

		// next adapter
		++i;
	}
	while(ok != S_OK);

	// get the DXGI device
	ok = ID3D11Device_QueryInterface(
		context->globox_software_d3d11_device,
		&IID_IDXGIDevice,
		(void**) &(context->globox_software_dxgi_device));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DXGI_DEVICE);
		return;
	}

	// create swap chain
	swapchain(globox);

	// create the DirectComposition device
	ok = DCompositionCreateDevice(
		context->globox_software_dxgi_device,
		&IID_IDCompositionDevice,
		(void**) &(context->globox_software_dcomp_device));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_DEVICE);
		return;
	}

	// create the DirectComposition target
	ok = IDCompositionDevice_CreateTargetForHwnd(
		context->globox_software_dcomp_device,
		platform->globox_platform_event_handle,
		TRUE,
		&(context->globox_software_dcomp_target));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_TARGET);
		return;
	}

	// create the visual object
	ok = IDCompositionDevice_CreateVisual(
		context->globox_software_dcomp_device,
		&(context->globox_software_dcomp_visual));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_VISUAL);
		return;
	}

	// create the Direct2D factory
	D2D1_FACTORY_OPTIONS options =
	{
		.debugLevel = D2D1_DEBUG_LEVEL_NONE,
	};

	ok = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&IID_ID2D1Factory2,
		&options,
		(void**) &(context->globox_software_d2d_factory));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_FACTORY);
		return;
	}

	// create the Direct2D device
	ok = ID2D1Factory2_CreateDevice(
		context->globox_software_d2d_factory,
		context->globox_software_dxgi_device,
		&(context->globox_software_d2d_device));

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_DEVICE);
		return;
	}
}

void globox_context_software_init(
	struct globox* globox,
	int version_major,
	int version_minor)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);

	platform->globox_windows_resize_callback = resize;
	platform->globox_windows_dcomp_callback = dcomp;

	context->globox_software_buffer_width = globox->globox_width;
	context->globox_software_buffer_height = globox->globox_height;

	platform->globox_platform_argb =
		malloc(
			4
			* context->globox_software_buffer_width
			* context->globox_software_buffer_height);

	if (platform->globox_platform_argb == NULL)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_ALLOC);
		return;
	}

	if (globox->globox_transparent == true)
	{
		context->globox_software_alpha = DXGI_ALPHA_MODE_PREMULTIPLIED;
	}
	else
	{
		context->globox_software_alpha = DXGI_ALPHA_MODE_IGNORE;
	}
}

void globox_context_software_create(struct globox* globox)
{
	// not needed
}

void globox_context_software_shrink(struct globox* globox)
{
	// not needed
}

void globox_context_software_free(struct globox* globox)
{
// TODO
#if 0
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);
#endif
}

void globox_context_software_copy(
	struct globox* globox,
	int32_t x,
	int32_t y,
	uint32_t width,
	uint32_t height)
{
	// alias for readability
	struct globox_platform* platform = &(globox->globox_platform);
	struct globox_windows_software* context = &(platform->globox_windows_software);

	HRESULT ok;

	// get a D2D device context
	ID2D1DeviceContext* device_context;

	ok = ID2D1Device1_CreateDeviceContext(
		context->globox_software_d2d_device,
		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		&device_context);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_DEVICE_CONTEXT);
		return;
	}

	// get a swap chain surface
	IDXGISurface2* surface;

	ok = IDXGISwapChain1_GetBuffer(
		context->globox_software_swapchain,
		0,
		&IID_IDXGISurface2,
		(void**) &surface);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_SWAPCHAIN_SURFACE);
		return;
	}

	// get the surface bitmap
	ID2D1Bitmap1* bitmap;

	D2D1_PIXEL_FORMAT format =
	{
		.format = DXGI_FORMAT_B8G8R8A8_UNORM,
		.alphaMode = context->globox_software_alpha,
	};

	D2D1_BITMAP_PROPERTIES1 properties =
	{
		.dpiX = 96.0f,
		.dpiY = 96.0f,
		.colorContext = NULL,
		.pixelFormat = format,
		.bitmapOptions =
			D2D1_BITMAP_OPTIONS_TARGET
			| D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
	};

	ok = ID2D1DeviceContext_CreateBitmapFromDxgiSurface(
		device_context,
		surface,
		&properties,
		&bitmap);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_SURFACE_BITMAP);
		return;
	}

	// copy the buffer to the bitmap
	D2D1_RECT_U rect =
	{
		.left = 0,
		.top = 0,
		.right = context->globox_software_buffer_width,
		.bottom = context->globox_software_buffer_height,
	};

	ok = ID2D1Bitmap_CopyFromMemory(
		(ID2D1Bitmap*) bitmap,
		&rect,
		platform->globox_platform_argb,
		context->globox_software_buffer_width * 4);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_COPY);
		return;
	}

	// present
	ok = IDXGISwapChain1_Present(
		context->globox_software_swapchain,
		1,
		0);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_D2D_PRESENT);
		return;
	}

	// bind surface to visual
	ok = IDCompositionVisual_SetContent(
		context->globox_software_dcomp_visual,
		(void*) context->globox_software_swapchain);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_BIND);
		return;
	}

	// set visual as root
	ok = IDCompositionTarget_SetRoot(
		context->globox_software_dcomp_target,
		context->globox_software_dcomp_visual);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_SET_ROOT);
		return;
	}

	// commit
	ok = IDCompositionDevice_Commit(
		context->globox_software_dcomp_device);

	if (ok != S_OK)
	{
		globox_error_throw(
			globox,
			GLOBOX_ERROR_WINDOWS_DCOMP_COMMIT);
		return;
	}

	globox->globox_redraw = false;
}

// getters
