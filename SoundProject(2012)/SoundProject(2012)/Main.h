#pragma comment(lib,"FreeImage/FreeImage")
//#pragma comment(lib,"fmod")


#include "stdafx.h"
#include "InputManager.h"
#include "Camera.h"
#include "Terrain.h"
#include "Sound.h"
#include "ObjHandler.h"
#include "ButtonInput.h"



	// Global Variables
	HINSTANCE				g_hInst					= NULL;  
	HWND					g_hWnd					= NULL;
	D3D10_DRIVER_TYPE       g_driverType			= D3D10_DRIVER_TYPE_NULL;
	IDXGISwapChain*         g_pSwapChain			= NULL;
	ID3D10Device*           g_pd3dDevice			= NULL;
	ID3D10RenderTargetView* g_pRenderTargetView		= NULL;
	ID3D10Texture2D*        g_pDepthStencil			= NULL;
	ID3D10DepthStencilView* g_pDepthStencilView		= NULL;

	//buffer data
	ID3D10InputLayout*		g_pVertexLayout;
	ID3D10Buffer*			g_pVB;

	//shader variables
	ID3D10Effect*			g_pEffect;
	ID3D10Effect*			g_particleEffect;
	ID3D10EffectTechnique*	g_pTechRenderLine;

	Camera* Cam = new Camera();
	Terrain* MapTerrain = new Terrain();
	SoundSystem* soundSystem;// = new SoundSystem();
	Sound* backgroundSound = new Sound();
	ButtonInput* buttonInput = new ButtonInput();


	ObjHandler* objHandler;

	//--------------------------------------------------------------------------------------
	// Forward declarations
	//--------------------------------------------------------------------------------------
	HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
	LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT				InitDevice();
	HRESULT				Render();
	void				Update(float deltaTime);
	void				CleanupDevice();

