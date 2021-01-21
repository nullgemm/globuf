#ifndef H_GLOBOX_WINDOWS_SYMBOLS
#define H_GLOBOX_WINDOWS_SYMBOLS

#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <dcomptypes.h>

#include <d2d1.h>
#include <d2d1_1.h>

// IDXGIDevice helper function
HRESULT WINAPI DCompositionCreateDevice(
	IDXGIDevice* dxgiDevice,
	REFIID iid,
	void** dcompositionDevice);

// method accessors
// IDCompositionDevice
#define IDCompositionDevice_Commit(This) \
	((This)->lpVtbl->Commit(This))
#define IDCompositionDevice_CreateTargetForHwnd(This, hwnd, topmost, target) \
	((This)->lpVtbl->CreateTargetForHwnd(This, hwnd, topmost, target))
#define IDCompositionDevice_CreateVisual(This, visual) \
	((This)->lpVtbl->CreateVisual(This, visual))
#define IDCompositionDevice_CreateSurface(This, width, height, format, mode, surface) \
	((This)->lpVtbl->CreateSurface(This, width, height, format, mode, surface))
// IDCompositionSurface
#define IDCompositionSurface_BeginDraw(This, rect, iid, object, offset) \
	((This)->lpVtbl->BeginDraw(This, rect, iid, object, offset))
#define IDCompositionSurface_EndDraw(This) \
	((This)->lpVtbl->EndDraw(This))
// IDCompositionVisual
#define IDCompositionVisual_SetOffsetX(This, offset) \
	((This)->lpVtbl->SetOffsetX(This, offset))
#define IDCompositionVisual_SetOffsetY(This, offset) \
	((This)->lpVtbl->SetOffsetY(This, offset))
#define IDCompositionVisual_SetContent(This, content) \
	((This)->lpVtbl->SetContent(This, content))
// IDCompositionTarget
#define IDCompositionTarget_SetRoot(This, visual) \
	((This)->lpVtbl->SetRoot(This, visual))

// ID2D1Factory2
#define ID2D1Factory2_CreateDevice(This, dxgiDevice, d2dDevice1) \
	((This)->lpVtbl->CreateDevice(This, dxgiDevice, d2dDevice1))
// ID2D1Device1
#define ID2D1Device1_CreateDeviceContext(This, options, deviceContext) \
	((This)->lpVtbl->CreateDeviceContext(This, options, deviceContext))

// ID2D1DeviceContext
#define ID2D1DeviceContext_CreateBitmapFromDxgiSurface(This, surface, properties, bitmap) \
	((This)->lpVtbl->CreateBitmapFromDxgiSurface(This, surface, properties, bitmap))

#if defined(GLOBOX_COMPILER_MSVC)
// ID2D1Bitmap
#define ID2D1Bitmap_GetPixelSize(This) \
	((This)->lpVtbl->GetPixelSize(This))
#define ID2D1Bitmap_CopyFromMemory(This, rect, data, pitch) \
	((This)->lpVtbl->CopyFromMemory(This, rect, data, pitch))
#endif

// interface IDs
EXTERN_C const IID IID_IDCompositionDevice;
EXTERN_C const IID IID_IDCompositionSurface;
EXTERN_C const IID IID_IDCompositionVisual;
EXTERN_C const IID IID_IDCompositionTarget;

EXTERN_C const IID IID_ID2D1Factory2;
EXTERN_C const IID IID_ID2D1Device1;
EXTERN_C const IID IID_ID2D1DeviceContext;
EXTERN_C const IID IID_ID2D1Bitmap;

// structures
enum ACCENT_STATE
{
	ACCENT_ENABLE_BLURBEHIND = 3,
};

enum WINDOWCOMPOSITIONATTRIB
{
	WCA_ACCENT_POLICY = 19,
};

struct ACCENT_POLICY
{
	enum ACCENT_STATE AccentState;
	DWORD AccentFlags;
	DWORD GradientColor;
	DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
	enum WINDOWCOMPOSITIONATTRIB dwAttrib;
	PVOID pvData;
	SIZE_T cbData;
};

union NAMED_INPUT_UNION
{
	MOUSEINPUT mi;
	KEYBDINPUT ki;
	HARDWAREINPUT hi;
};

struct NAMED_INPUT
{
	DWORD type;
	union NAMED_INPUT_UNION data;
};

typedef interface IDCompositionDevice IDCompositionDevice;
typedef interface IDCompositionSurface IDCompositionSurface;
typedef interface IDCompositionVisual IDCompositionVisual;
typedef interface IDCompositionTarget IDCompositionTarget;

typedef interface ID2D1Factory2 ID2D1Factory2;
typedef interface ID2D1Device1 ID2D1Device1;

#if defined(GLOBOX_COMPILER_MSVC)
typedef interface ID2D1DeviceContext ID2D1DeviceContext;
typedef interface ID2D1Bitmap ID2D1Bitmap;
#endif

// virtual tables
// DirectComposition
typedef struct IDCompositionDeviceVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// IDCompositionDevice methods
	HRESULT (STDMETHODCALLTYPE* Commit)(
		IDCompositionDevice* This);

	HRESULT (STDMETHODCALLTYPE* WaitForCommitCompletion)(
		IDCompositionDevice* This);

	HRESULT (STDMETHODCALLTYPE* GetFrameStatistics)(
		IDCompositionDevice* This);

	HRESULT (STDMETHODCALLTYPE* CreateTargetForHwnd)(
		IDCompositionDevice* This,
		HWND hwnd,
		BOOL topmost,
		IDCompositionTarget** target);

	HRESULT (STDMETHODCALLTYPE* CreateVisual)(
		IDCompositionDevice* This,
		IDCompositionVisual** visual);

	HRESULT (STDMETHODCALLTYPE* CreateSurface)(
		IDCompositionDevice* This,
		UINT width,
		UINT height,
		DXGI_FORMAT format,
		DXGI_ALPHA_MODE mode,
		IDCompositionSurface** surface);

	END_INTERFACE
} IDCompositionDeviceVtbl;

typedef struct IDCompositionSurfaceVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// IDCompositionSurface methods
	HRESULT (STDMETHODCALLTYPE* BeginDraw)(
		IDCompositionSurface* This,
		const RECT* rect,
		REFIID iid,
		void** object,
		POINT* offset);

	HRESULT (STDMETHODCALLTYPE* EndDraw)(
		IDCompositionSurface* This);

	END_INTERFACE
} IDCompositionSurfaceVtbl;

typedef struct IDCompositionVisualVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// IDCompositionVisual methods
	void (STDMETHODCALLTYPE* func3)();
	// overridden method
	HRESULT (STDMETHODCALLTYPE* SetOffsetX)(
		IDCompositionVisual* This,
		float offset);
	void (STDMETHODCALLTYPE* func4)();
	// overridden method
	HRESULT (STDMETHODCALLTYPE* SetOffsetY)(
		IDCompositionVisual* This,
		float offset);
	void (STDMETHODCALLTYPE* func5)();
	void (STDMETHODCALLTYPE* func6)();
	void (STDMETHODCALLTYPE* func7)();
	void (STDMETHODCALLTYPE* func8)();
	void (STDMETHODCALLTYPE* func9)();
	void (STDMETHODCALLTYPE* func10)();
	void (STDMETHODCALLTYPE* func11)();
	void (STDMETHODCALLTYPE* func12)();

	HRESULT (STDMETHODCALLTYPE* SetContent)(
		IDCompositionVisual* This,
		IUnknown* content);

	END_INTERFACE
} IDCompositionVisualVtbl;

typedef struct IDCompositionTargetVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// IDCompositionTarget
	HRESULT (STDMETHODCALLTYPE* SetRoot)(
		IDCompositionTarget* This,
		IDCompositionVisual* visual);

	END_INTERFACE
} IDCompositionTargetVtbl;

// Direct2D
typedef struct ID2D1Factory2Vtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// ID2D1Factory2 methods
	void (STDMETHODCALLTYPE* func4)();
	void (STDMETHODCALLTYPE* func5)();
	void (STDMETHODCALLTYPE* func6)();
	void (STDMETHODCALLTYPE* func7)();
	void (STDMETHODCALLTYPE* func8)();
	void (STDMETHODCALLTYPE* func9)();
	void (STDMETHODCALLTYPE* func10)();
	void (STDMETHODCALLTYPE* func11)();
	void (STDMETHODCALLTYPE* func12)();
	void (STDMETHODCALLTYPE* func13)();
	void (STDMETHODCALLTYPE* func14)();
	void (STDMETHODCALLTYPE* func15)();
	void (STDMETHODCALLTYPE* func16)();
	void (STDMETHODCALLTYPE* func17)();

	HRESULT (STDMETHODCALLTYPE* CreateDevice)(
		ID2D1Factory2* This,
		IDXGIDevice* dxgiDevice,
		ID2D1Device1** d2dDevice1);

	END_INTERFACE
} ID2D1Factory2Vtbl;

typedef struct ID2D1Device1Vtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// I methods
	void (STDMETHODCALLTYPE* func3)();
	void (STDMETHODCALLTYPE* func4)();

	// I methods
	void (STDMETHODCALLTYPE* func5)();
	void (STDMETHODCALLTYPE* func6)();
	void (STDMETHODCALLTYPE* func7)();
	void (STDMETHODCALLTYPE* func8)();
	void (STDMETHODCALLTYPE* func9)();
	void (STDMETHODCALLTYPE* func10)();

	// I methods
	void (STDMETHODCALLTYPE* func11)();

	// ID2D1Device1 methods
	HRESULT (STDMETHODCALLTYPE* CreateDeviceContext)(
		ID2D1Device1* This,
		D2D1_DEVICE_CONTEXT_OPTIONS options,
		ID2D1DeviceContext** deviceContext);

	END_INTERFACE
} ID2D1Device1Vtbl;

#if defined(GLOBOX_COMPILER_MSVC)
typedef struct ID2D1DeviceContextVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// I methods
	void (STDMETHODCALLTYPE* func3)();
	void (STDMETHODCALLTYPE* func4)();
	void (STDMETHODCALLTYPE* func5)();
	void (STDMETHODCALLTYPE* func6)();
	void (STDMETHODCALLTYPE* func7)();

	// I methods
	void (STDMETHODCALLTYPE* func8)();
	void (STDMETHODCALLTYPE* func9)();
	void (STDMETHODCALLTYPE* func10)();
	void (STDMETHODCALLTYPE* func11)();
	void (STDMETHODCALLTYPE* func12)();
	void (STDMETHODCALLTYPE* func13)();
	void (STDMETHODCALLTYPE* func14)();
	void (STDMETHODCALLTYPE* func15)();
	void (STDMETHODCALLTYPE* func16)();
	void (STDMETHODCALLTYPE* func17)();
	void (STDMETHODCALLTYPE* func18)();
	void (STDMETHODCALLTYPE* func19)();
	void (STDMETHODCALLTYPE* func20)();
	void (STDMETHODCALLTYPE* func21)();
	void (STDMETHODCALLTYPE* func22)();
	void (STDMETHODCALLTYPE* func23)();
	void (STDMETHODCALLTYPE* func24)();
	void (STDMETHODCALLTYPE* func25)();
	void (STDMETHODCALLTYPE* func26)();
	void (STDMETHODCALLTYPE* func27)();
	void (STDMETHODCALLTYPE* func28)();
	void (STDMETHODCALLTYPE* func29)();
	void (STDMETHODCALLTYPE* func30)();
	void (STDMETHODCALLTYPE* func31)();
	void (STDMETHODCALLTYPE* func32)();
	void (STDMETHODCALLTYPE* func33)();
	void (STDMETHODCALLTYPE* func34)();
	void (STDMETHODCALLTYPE* func35)();
	void (STDMETHODCALLTYPE* func36)();
	void (STDMETHODCALLTYPE* func37)();
	void (STDMETHODCALLTYPE* func38)();
	void (STDMETHODCALLTYPE* func39)();
	void (STDMETHODCALLTYPE* func40)();
	void (STDMETHODCALLTYPE* func41)();
	void (STDMETHODCALLTYPE* func42)();
	void (STDMETHODCALLTYPE* func43)();
	void (STDMETHODCALLTYPE* func44)();
	void (STDMETHODCALLTYPE* func45)();
	void (STDMETHODCALLTYPE* func46)();
	void (STDMETHODCALLTYPE* func47)();
	void (STDMETHODCALLTYPE* func48)();
	void (STDMETHODCALLTYPE* func49)();
	void (STDMETHODCALLTYPE* func50)();
	void (STDMETHODCALLTYPE* func51)();
	void (STDMETHODCALLTYPE* func52)();
	void (STDMETHODCALLTYPE* func53)();
	void (STDMETHODCALLTYPE* func54)();
	void (STDMETHODCALLTYPE* func55)();
	void (STDMETHODCALLTYPE* func56)();
	void (STDMETHODCALLTYPE* func57)();
	void (STDMETHODCALLTYPE* func58)();
	void (STDMETHODCALLTYPE* func59)();
	void (STDMETHODCALLTYPE* func60)();

	// I methods
	void (STDMETHODCALLTYPE* func61)();

	// ID2D1DeviceContext methods
	HRESULT (STDMETHODCALLTYPE* CreateBitmapFromDxgiSurface)(
		ID2D1DeviceContext* This,
		IDXGISurface2* surface,
		D2D1_BITMAP_PROPERTIES1* properties,
		ID2D1Bitmap1** bitmap);

	END_INTERFACE
} ID2D1DeviceContextVtbl;

typedef struct ID2D1BitmapVtbl
{
	BEGIN_INTERFACE

	// IUnknown methods
	void (STDMETHODCALLTYPE* func0)();
	void (STDMETHODCALLTYPE* func1)();
	void (STDMETHODCALLTYPE* func2)();

	// I methods
	void (STDMETHODCALLTYPE* func3)();

	// ID2D1Bitmap1 methods
	void (STDMETHODCALLTYPE* func4)();
	HRESULT (STDMETHODCALLTYPE* GetPixelSize)(
		ID2D1Bitmap* This);
	void (STDMETHODCALLTYPE* func5)();
	void (STDMETHODCALLTYPE* func6)();
	void (STDMETHODCALLTYPE* func7)();
	void (STDMETHODCALLTYPE* func8)();
	HRESULT (STDMETHODCALLTYPE* CopyFromMemory)(
		ID2D1Bitmap* This,
		D2D1_RECT_U* rect,
		void* data,
		UINT32 pitch);

	END_INTERFACE
} ID2D1BitmapVtbl;
#endif

// interfaces
interface IDCompositionDevice
{
	CONST_VTBL struct IDCompositionDeviceVtbl* lpVtbl;
};

interface IDCompositionSurface 
{
	CONST_VTBL struct IDCompositionSurfaceVtbl* lpVtbl;
};

interface IDCompositionVisual
{
	CONST_VTBL struct IDCompositionVisualVtbl* lpVtbl;
};

interface IDCompositionTarget
{
	CONST_VTBL struct IDCompositionTargetVtbl* lpVtbl;
};

interface ID2D1Factory2
{
	CONST_VTBL struct ID2D1Factory2Vtbl* lpVtbl;
};

interface ID2D1Device1
{
	CONST_VTBL struct ID2D1Device1Vtbl* lpVtbl;
};

#if defined(GLOBOX_COMPILER_MSVC)
interface ID2D1DeviceContext
{
	CONST_VTBL struct ID2D1DeviceContextVtbl* lpVtbl;
};

interface ID2D1Bitmap
{
	CONST_VTBL struct ID2D1BitmapVtbl* lpVtbl;
};
#endif

#endif
