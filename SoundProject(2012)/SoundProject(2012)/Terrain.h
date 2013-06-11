#pragma once
#include "stdafx.h"
#include "Camera.h"

extern Camera* Cam;
extern ID3D10EffectTechnique*	g_pTechRenderLine;
extern ID3D10Effect* g_pEffect;
extern ID3D10InputLayout* g_pVertexLayout;
extern ID3D10Buffer* g_pVB;
extern ID3D10RenderTargetView* g_pRenderTargetView;
extern ID3D10Texture2D* g_pDepthStencil;
extern ID3D10DepthStencilView* g_pDepthStencilView;

static ID3D10EffectShaderResourceVariable*	terrain_mfxDiffuseMapVar;

class Terrain
{
private:
	//terrain mesh
	ID3DX10Mesh*				pTerrainMesh;
	ID3D10ShaderResourceView*	pTerrainTexture;
	LineVertex *MeshVertices;
	unsigned int *MeshIndices;
	int hmHeight, hmWidth;
	float* pHeightData;
	unsigned int numIndices;

public:
	Terrain(void);
	~Terrain(void);
	void CreateTerrain(ID3D10Device* g_pd3dDevice);
	void Draw(ID3D10Device* g_pd3dDevice);
};

