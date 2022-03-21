#include "Game/Game.h"

namespace library
{
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/
    /*--------------------------------------------------------------------
      TODO: Initialize global variables (remove the comment)
    --------------------------------------------------------------------*/
    HINSTANCE hInstance = nullptr;
    HWND hWnd = nullptr;
    

    /*--------------------------------------------------------------------
      Forward declarations
    --------------------------------------------------------------------*/

    /*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Function: WindowProc
      Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth
      Args:     HWND hWnd
                  Handle to the window
                UINT uMsg
                  Message code
                WPARAM wParam
                  Additional data that pertains to the message
                LPARAM lParam
                  Additional data that pertains to the message
      Returns:  LRESULT
                  Integer value that your program returns to Windows
    -----------------------------------------------------------------F-F*/
    LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) 
    {
        switch (uMsg) {
            case WM_CLOSE: {
                HMENU hMenu;
                hMenu = GetMenu(hWnd);
                if (hMenu != NULL)
                {
                    DestroyMenu(hMenu);
                }
                DestroyWindow(hWnd);
                
                return 0;
            }
            
            case WM_DESTROY: 
                PostQuitMessage(0);
                break;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    /*--------------------------------------------------------------------
      TODO: Function definitions (remove this comment)
    --------------------------------------------------------------------*/

    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow) 
    {
        //Register class
        WNDCLASS wndClass;
        wndClass.style = CS_DBLCLKS;
        wndClass.lpfnWndProc = WindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = hInstance;
//        wndClass.hIcon = hIcon;
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = L"WindowClass";
    
        if (!RegisterClass(&wndClass)) 
        {
            DWORD dwError = GetLastError();
            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
                return HRESULT_FROM_WIN32(dwError);
        }

        //Create window
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        hWnd = CreateWindowEx(
            0,
            L"WindowClass",
            L"GameGraphicProgramming",
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,
            NULL,
            hInstance, //Instance handle
            NULL
        );

        if (hWnd == NULL)
        {
            return 0;
        }

        ShowWindow(hWnd, nCmdShow);

    }

    HRESULT InitDevice()
    {
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_11_1
        };

        UINT createDeviceFlags = 0;

        #if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        Microsoft::WRL::ComPtr<ID3D11Device>        device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

        UINT               numLevelsRequested = 1;
        D3D_FEATURE_LEVEL  featurelevel;

        HRESULT hr = D3D11CreateDevice(
            nullptr,                    // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
            nullptr,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,                // Set debug and Direct2D compatibility flags.
            levels,                     // List of feature levels this app can support.
            ARRAYSIZE(levels),          // Size of the list above.
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &device,                    // Returns the Direct3D device created.
            &featurelevel,            // Returns feature level of device created.
            &context                    // Returns the device immediate context.
        );

        if (FAILED(hr))
            return hr;

        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;      //multisampling setting
        desc.SampleDesc.Quality = 0;    //vendor-specific flag
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.OutputWindow = hWnd;

        // Create the DXGI device object to use in other factories, such as Direct2D.
        Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
        m_pd3dDevice.As(&dxgiDevice);

        // Create swap chain.
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));

            hr = factory->CreateSwapChain(
                m_pd3dDevice.Get(),
                &desc,
                &m_pDXGISwapChain
            );
        }

    }

}