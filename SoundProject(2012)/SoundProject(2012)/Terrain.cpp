#include "Terrain.h"


Terrain::Terrain(void)
{
	hmHeight = 0;
	hmWidth = 0;
}


Terrain::~Terrain(void)
{
}

void Terrain::CreateTerrain(ID3D10Device* g_pd3dDevice)
{
	int xPos = 128;
	int zPos = 128;
	int heightScaling = 12;

	//read heightmap values
	FIBITMAP *pHeightMap;// = FreeImage_Load(FIF_BMP, "Terrain_heightmap.bmp", BMP_DEFAULT);
	
	//pHeightMap = FreeImage_Load(FIF_BMP, "Pics/Terrain_heightmap.bmp", BMP_DEFAULT);
	pHeightMap = FreeImage_Load(FIF_BMP,"Pics/heightmap3.bmp",BMP_DEFAULT);
	
	
		
	if ( pHeightMap )
	{
		//get height and width & allocate memory
		hmHeight = FreeImage_GetHeight(pHeightMap);
		hmWidth = FreeImage_GetWidth(pHeightMap);
		
		pHeightData = new float[hmHeight * hmWidth];

		RGBQUAD color;	
		for (int r=0; r < hmHeight; r++)
		{
			for(int c=0; c < hmWidth; c++)
			{
				FreeImage_GetPixelColor(pHeightMap,c,r,&color);
				
				//scale height data 
				float divisor = 255 / heightScaling;
				pHeightData[r*hmWidth + c] = color.rgbRed / divisor - 3;
			}
		}

		FreeImage_Unload(pHeightMap);
	}

	//CREATE VERTICES
	//--------------------------------------------------------------------------------

	//create vertices for mesh
	unsigned int numVertices = hmHeight * hmWidth;
	LineVertex *pMeshVertices = new LineVertex[ numVertices ];
	float tC[2] = {0,1}, tCStepSize[2] = { 1.0f/hmWidth, 1.0f/hmHeight};

	//set positions
	float z = -zPos, x = -xPos;
	int cnt=0, wCnt = 0, hCnt = 0;	
	
	while (hCnt < hmHeight)
	{
		while (wCnt < hmWidth)
		{
			pMeshVertices[cnt].texCoord = D3DXVECTOR2( tC[0], tC[1] );			
			pMeshVertices[cnt++].pos = D3DXVECTOR3( x, pHeightData[cnt], z );
			tC[0] += tCStepSize[0];
			wCnt++;
			x += (float)((xPos))/(hmWidth-1);
		}

		//set vars
		/*
		x = -32;
		z += 64.0f/(hmWidth-1);
		*/
		
		x = (float)(-xPos);
		z += (float)((zPos))/(hmWidth-1);
		
		tC[0] = 0;
		tC[1] -= tCStepSize[1];
		wCnt = 0;
		hCnt++;
	}

	MeshVertices = new LineVertex[ numVertices ];
	for(int i = 0;i < numVertices;i++)
		MeshVertices[i] = pMeshVertices[i];
	

	//CALCULATE NORMALS
	//--------------------------------------------------------------------------------

	//calculate normals	
	for ( unsigned int i = 0; i < numVertices - hmWidth; i++ )
	{
		D3DXVECTOR3 N, v1, v2;
		
		//calculate per primitive normal - face 1		
		v1 = pMeshVertices[i+hmWidth].pos - pMeshVertices[i].pos;
		v2 = pMeshVertices[i+1].pos - pMeshVertices[i].pos;

		D3DXVec3Cross( &N, &v1, &v2);

		//add primitive normal to all primitive's vertices
		pMeshVertices[i].normal += N;
		pMeshVertices[i+1].normal += N;
		pMeshVertices[i+hmWidth].normal += N;

		//calculate per primitive normal - face 2	
		v1 = pMeshVertices[i+hmWidth].pos - pMeshVertices[i+1].pos;
		v2 = pMeshVertices[i+hmWidth+1].pos - pMeshVertices[i+1].pos;
		
		D3DXVec3Cross( &N, &v1, &v2);

		//add primitive normal to all primitive's vertices
		pMeshVertices[i+1].normal += N;
		pMeshVertices[i+hmWidth].normal += N;
		pMeshVertices[i+hmWidth+1].normal += N;

		//skip last vert
		if ( i % hmWidth == hmWidth - 2 ) i++;
	}
	
	for ( unsigned int i = 0; i < numVertices - hmWidth; i++ )
	{
		D3DXVec3Normalize(&pMeshVertices[i].normal,&pMeshVertices[i].normal); 
	}

	//CREATE INDICES
	//--------------------------------------------------------------------------------

	//set indices
	numIndices = (hmHeight-1) * (hmWidth-1) * 6;
	unsigned int *pMeshIndices = new unsigned int[numIndices];
	cnt = 0;

	for ( unsigned int i = 0; i < numVertices - hmWidth; i++ )
	{
		//add indices for 2 faces;
		pMeshIndices[cnt++] = i;
		pMeshIndices[cnt++] = i + hmWidth;
		pMeshIndices[cnt++] = i + 1;
		pMeshIndices[cnt++] = i + 1;
		pMeshIndices[cnt++] = i + hmWidth;
		pMeshIndices[cnt++] = i + hmWidth + 1;
		
		//skip last vert
		if ( i % hmWidth == hmWidth - 2 ) i++;
	}
	MeshIndices = new unsigned int[numIndices];
	for(int i = 0;i < numIndices;i++)
		MeshIndices[i] = pMeshIndices[i];

	//CREATE MESH
	//--------------------------------------------------------------------------------


	if ( FAILED( D3DX10CreateMesh(	g_pd3dDevice, 
									lineVertexLayout, 
									sizeof(lineVertexLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC), 
									"POS",
									numVertices, 
									numIndices/3, 
									D3DX10_MESH_32_BIT, 
									&pTerrainMesh) ) ) 
	{
	MessageBox(0, "Error compiling shader!", "Shader error!", 0);
	}
	
	//insert data into mesh and commit changes
	pTerrainMesh->SetVertexData(0, pMeshVertices);
	pTerrainMesh->SetIndexData(pMeshIndices, numIndices);
	pTerrainMesh->CommitToDevice();

	terrain_mfxDiffuseMapVar = g_pEffect->GetVariableByName("frame")->AsShaderResource();

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;
	ID3D10ShaderResourceView *pSRView = NULL;
	D3DX10CreateShaderResourceViewFromFile( g_pd3dDevice, "Pics/Terrain_texture.jpg", &loadInfo, NULL, &pTerrainTexture, NULL );

	//free memory
	delete[] pMeshVertices;
	delete[] pMeshIndices;

}

void Terrain::Draw(ID3D10Device* g_pd3dDevice)
{

	HRESULT hr = S_OK;


	UINT pHitCount;
	UINT pFaceIndex;
	float pU;
	float pV;
	float pDist;
	ID3D10Blob* ppAllHits;

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);

	D3DXVECTOR3 rayOrigin = Cam->getCameraPosition();
	D3DXVECTOR3 rayDir = D3DXVECTOR3(0,-1,0);

	D3DXMATRIX inverseW;
	D3DXMatrixInverse(&inverseW, 0, &world);
	D3DXVec3TransformCoord(&rayOrigin, &rayOrigin, &inverseW);
	D3DXVec3TransformNormal(&rayDir, &rayDir, &inverseW);
	

	if( SUCCEEDED( pTerrainMesh->Intersect(&rayOrigin,&rayDir,&pHitCount,&pFaceIndex,&pU,&pV,&pDist,&ppAllHits) ) )
	{
		float y;
		float y0,y1,y2;
		float v0,v1;
		if(pFaceIndex*3+2 < numIndices)
		{
			y0 = MeshVertices[MeshIndices[pFaceIndex*3]].pos.y;
			y1 = MeshVertices[MeshIndices[pFaceIndex*3 + 1]].pos.y;
			y2 = MeshVertices[MeshIndices[pFaceIndex*3 + 2]].pos.y;
			v0 = y0 - y1;
			v1 = y1 - y2;

			y = y0 + v0*pU + v1*pV;

			Cam->setY(y + 5);
		}

	}


	g_pTechRenderLine = g_pEffect->GetTechniqueByName("AVI");

	
	D3D10_PASS_DESC PassDesc;
	g_pTechRenderLine->GetPassByIndex(0)->GetDesc(&PassDesc);

	g_pd3dDevice->CreateInputLayout(lineVertexLayout,
		sizeof(lineVertexLayout) / sizeof(D3D10_INPUT_ELEMENT_DESC),
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&g_pVertexLayout );
	

	//calculate WVP matrix
	static float rotY = 0.0f;
	//rotY += 0.0005f;
	D3DXMATRIX  view, proj, wvp;
	D3DXMatrixRotationY(&world, rotY);


	// Set variables
	g_pEffect->GetVariableByName( "ProjectionMatrix" )->AsMatrix()->SetMatrix( (float*)&Cam->getProjectionMatrix());
	g_pEffect->GetVariableByName( "ViewMatrix" )->AsMatrix()->SetMatrix( (float*)&Cam->getViewMatrix());
	g_pEffect->GetVariableByName( "WorldMatrix" )->AsMatrix()->SetMatrix( (float*)&world);
	g_pEffect->GetVariableByName( "CameraPos" )->AsMatrix()->SetMatrix( (float*)&Cam->getCameraPosition());

	float *temp;
	temp = new float;
	*temp = D3DXMatrixDeterminant(&wvp);

	D3DXMatrixInverse(&wvp,temp,&wvp);

	D3DXMatrixTranspose(&wvp,&wvp);

	g_pEffect->GetVariableByName( "normalMatrix" )->AsMatrix()->SetMatrix( (float*)&wvp);
	


	// Set Input Assembler params
	UINT stride = sizeof(LineVertex);
	UINT offset = 0;
	g_pd3dDevice->IASetInputLayout( g_pVertexLayout );
	g_pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pd3dDevice->IASetVertexBuffers( 0, 1, &g_pVB, &stride, &offset );

	static float ClearColor[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
	//clear render target
	g_pd3dDevice->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

	//clear depth info
	g_pd3dDevice->ClearDepthStencilView( g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	// Render line using the technique g_pRenderTextured

	terrain_mfxDiffuseMapVar->SetResource(pTerrainTexture);

	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechRenderLine->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; p++ )
	{
		g_pTechRenderLine->GetPassByIndex( p )->Apply(0);
		pTerrainMesh->DrawSubset(0);

	}

	
	
	
}