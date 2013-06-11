#include "Main.h"

//Projekt i Tillämpad Ljudteknik, DV1428
//Fredrik Johannesson och Martin Säll

//B - Toggle echo on/of
//V - Decrease echo delay (min 0 ms)
//C - Increase echo delay (max 5000 ms)
//M - Toggle low-pass filter on/off

HRESULT InitDevice()
{
	soundSystem = new SoundSystem();
	soundSystem->Initialize();
	
	backgroundSound->Initialize(soundSystem->fmodSystem, "Sounds/StarcraftBackgroundTerran.mp3");
	backgroundSound->Play();
	
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( g_hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	D3D10_DRIVER_TYPE driverTypes[] = 
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, 
			D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice );
		if( SUCCEEDED( hr ) )
			break;
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D10Texture2D* pBackBuffer;
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;


	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
	if( FAILED(hr) )
		return hr;


	g_pd3dDevice->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports( 1, &vp );


	//Create vertex buffer to hold maxAmount of particles
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( LineVertex) * 4 ;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	if( FAILED( g_pd3dDevice->CreateBuffer( &bd, 0, &g_pVB ) ) )
		MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );



	//Init shader effect
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	if(FAILED(D3DX10CreateEffectFromFile(	"World.fx",
											NULL,
											NULL,
											"fx_4_0",
											dwShaderFlags,
											0,
											g_pd3dDevice,
											NULL,
											NULL,
											&g_pEffect,
											NULL,
											NULL)))
	{
		MessageBox(0, "Error compiling shader!", "Shader error!", 0);
	}

	g_pTechRenderLine = g_pEffect->GetTechniqueByName("AVI");

	
	D3D10_PASS_DESC PassDesc;
	g_pTechRenderLine->GetPassByIndex(0)->GetDesc(&PassDesc);

	//Create Input Layout (== Vertex Declaration)
	g_pd3dDevice->CreateInputLayout(lineVertexLayout,
		sizeof(lineVertexLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC),
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pVertexLayout );

	/*ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	mfxDiffuseMapVar = g_pEffect->GetVariableByName("frame")->AsShaderResource();
*/
	g_pEffect->GetVariableByName("LightPos")->AsVector()->SetFloatVector(D3DXVECTOR3(1,50,1));
	g_pEffect->GetVariableByName("Ambient")->SetRawValue(D3DXVECTOR3(0.1,0,0),0.0,4.0);

	
	
	//Creates my hightmap lanscape
	MapTerrain->CreateTerrain(g_pd3dDevice);

	objHandler = new ObjHandler(g_pEffect);
	return S_OK;
}

void Update(float deltaTime)	
{
	objHandler->Update(deltaTime);
	backgroundSound->SetStereoratio(0.1f,0.1f);
	//HRESULT hr = S_OK;

	////ps->Update();
	///*UINT pHitCount;
	//UINT pFaceIndex;
	//float pU;
	//float pV;
	//float pDist;
	//ID3D10Blob* ppAllHits;

	//D3DXMATRIX world;
	//D3DXMatrixIdentity(&world);*/

	//D3DXVECTOR3 rayOrigin = Cam->getCameraPosition();
	//D3DXVECTOR3 rayDir = D3DXVECTOR3(0,-1,0);

	//D3DXMATRIX inverseW;
	//D3DXMatrixInverse(&inverseW, 0, &world);
	//D3DXVec3TransformCoord(&rayOrigin, &rayOrigin, &inverseW);
	//D3DXVec3TransformNormal(&rayDir, &rayDir, &inverseW);
	//

	//if( SUCCEEDED( pTerrainMesh->Intersect(&rayOrigin,&rayDir,&pHitCount,&pFaceIndex,&pU,&pV,&pDist,&ppAllHits) ) )
	//{
	//	float y;
	//	float y0,y1,y2;
	//	float v0,v1;
	//	y0 = MeshVertices[MeshIndices[pFaceIndex*3]].pos.y;
	//	y1 = MeshVertices[MeshIndices[pFaceIndex*3 + 1]].pos.y;
	//	y2 = MeshVertices[MeshIndices[pFaceIndex*3 + 2]].pos.y;
	//	v0 = y0 - y1;
	//	v1 = y1 - y2;

	//	y = y0 + v0*pU + v1*pV;

	//	Cam->setY(y + 5);

	//}
	

	
}

HRESULT Render()
{
	MapTerrain->Draw(g_pd3dDevice);
	objHandler->Draw();
	

	if(FAILED(g_pSwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( InitDevice() ) )
		return 0;

	RegisterInputDevices(g_hWnd);

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	
	Cam->setPerspectiveProjectionLH( 60.0f, (float)(WIDTH / HEIGHT), 0.1f, 500.0f );
	Cam->setPositionAndView( 0.0f, 10.0f, 0.0f, 0.0f, 0.0f );
	Cam->setMovementSpeed(10);

	

	// Main message loop
	MSG msg = {0};
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//render
			Update(dt);
			Render();
			Cam->update();
			prevTimeStamp = currTimeStamp;
		}
	}

	CleanupDevice();

	return (int) msg.wParam;
}
//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "Sound";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, 1024, 768 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow(	"Sound",
							"Sound World",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL,
							NULL,
							hInstance,
							NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}
//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_INPUT:
		{
			HandleRawInput( hWnd, (HRAWINPUT&) lParam );
		}
		break;	

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if( g_pd3dDevice ) g_pd3dDevice->ClearState();

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pVB);
}