//////////////////
//  Space 2600  //
//////////////////
// Developed by Leonard Roll
//              Stephen Rose
//
// Version History: v0.1 main.cpp created, codded by -> Leonard & Stephen
//                  v0.2 DXBase.h & DXBase.cpp added, codded by -> Leonard
//                  v0.3 Model.h, Model.cpp & model_shaders.hlsl added, codded by -> Leonard 
//                  v0.4 Camera.h & Camera.cpp added, codded by -> Stephen
//                  v0.5 Input.h & Input.cpp added, codded by -> Stephen 
//                  v0.6 HUD.h, HUD.cpp added & HUD_Shaders, codded by -> Stephen
//					v0.7 Scene_Node.h, Scene_Node.cpp added, codded by -> Leonard

/////////////////
//  #Includes  //
/////////////////

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h>
#include <xnamath.h> 
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

/*Connecting the other classes*/
#include "DXBase.h"
#include "Input.h"
#include "camera.h" 
#include "Text2D.h" 
#include "model.h" 
#include "scene_node.h"

////////////////////////
//  Global Variables  //
////////////////////////

/*Instance controlling variables*/
HINSTANCE	            g_HInst = NULL;
HWND		            g_HWnd = NULL;

/*Direct3D 11 global variables*/
ID3D11Device*             g_pD3DDevice = NULL;
ID3D11DeviceContext*      g_pImmediateContext = NULL;
IDXGISwapChain*           g_p_SwapChain = NULL;
ID3D11RenderTargetView*   g_p_BackBufferRTView = NULL; 
ID3D11DepthStencilView*   g_pZBuffer; 

/*Main window name*/
char   g_WindowName[100] = "Space 2600";

/*Screen size variables*/
int    g_ScrWidth = 640;
int    g_ScrHeight = 480;

////////////////////////////
//	Forwards Declarations  //
////////////////////////////

/*Initialiser Functions*/
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////
//  Entry point to the program...  // <---- main game loop?
/////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Add game log here!??

	/*Displayed errors if initialisations have failed...*/
	if(FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}
	
	DXBase* Base = new DXBase(g_HInst,g_HWnd,g_ScrWidth,g_ScrHeight);

	if(FAILED(Base->InitialiseD3D()))
	{
		DXTRACE_MSG( "Failed to create Device" );
		return 0;
	}

	if(FAILED(Base->InitialiseGraphics())) // 03-01
	{
		DXTRACE_MSG( "Failed to initialise graphics" );
		return 0;
	}

	Input* input = new Input(Base->getCamera(),g_HInst,g_HWnd);

	if(FAILED(input->InitialiseInput(true))){
		DXTRACE_MSG("Failed to initialise input");
		return 0;
	}

	Base->setInput(input);

	/*Message loop control.*/
	MSG msg = {0};

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			/*When idle render frame.*/
			Base->RenderFrame();
		}
	}

	Base->ShutdownD3D();

	return (int) msg.wParam;
}

//////////////////////////
//  Window Initialiser  //
//////////////////////////

HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	/*Application title*/
	char Name[100] = "Space 2600";

	/* Register the class*/
	WNDCLASSEX wcex={0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = Name;

	/*Failure return*/
	if(!RegisterClassEx(&wcex))	
		return E_FAIL;

	/* Create  the window*/
	g_HInst = hInstance;
	RECT rc = {0, 0, g_ScrWidth, g_ScrHeight};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_HWnd = CreateWindow(Name, g_WindowName, WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
						 NULL);

	/*Failure return*/
	if(!g_HWnd) 
		return E_FAIL;

	/*Displays the created window*/
	ShowWindow(g_HWnd, nCmdShow);

	return S_OK;
}

////////////////////////////////
//  System Message Responses  //
////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch(message)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			if (g_p_SwapChain)
			{
				g_ScrWidth = LOWORD(lParam);
				g_ScrHeight = HIWORD(lParam);
				g_pImmediateContext->OMSetRenderTargets(0, 0, 0);

				/*Release outstanding references to the swap chain's buffers*/		
				g_p_BackBufferRTView->Release();

				HRESULT hr;

				/*Saves existing buffer count and format*/
				
				/*Automatically select the width and height to match the client rect for HWNDs*/
				hr = g_p_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                                            
				// <----------- Perform error handling here!

				/*Grab buffer and create a render-target-view*/
				ID3D11Texture2D* pBuffer;

				hr = g_p_SwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D), (void**) &pBuffer );
				
				// <------------ Perform error handling here!

				hr = g_pD3DDevice->CreateRenderTargetView(pBuffer, NULL, & g_p_BackBufferRTView);
				
				// <------------  Perform error handling here!
				
				pBuffer->Release();

					D3D11_TEXTURE2D_DESC tex2dDesc;

		
				/**/
				ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

				tex2dDesc.Width = g_ScrWidth;
				tex2dDesc.Height = g_ScrHeight;
				tex2dDesc.ArraySize = 1;
				tex2dDesc.MipLevels = 1;
				tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				tex2dDesc.SampleDesc.Count = 1; 
				tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

				ID3D11Texture2D *pZBufferTexture;
				hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

				/**/
				if(FAILED(hr)) return hr;

				// Create the depth buffer
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
				
				ZeroMemory(&dsvDesc, sizeof(dsvDesc));

				dsvDesc.Format = tex2dDesc.Format;
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

				g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
				
				pZBufferTexture->Release();

				g_pImmediateContext->OMSetRenderTargets(1, &g_p_BackBufferRTView, g_pZBuffer); //06-X

				/* Viewport creation*/
				D3D11_VIEWPORT vp;
				vp.Width = g_ScrWidth;
				vp.Height = g_ScrHeight;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				g_pImmediateContext->RSSetViewports( 1, &vp );

			}

		default:
			
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}