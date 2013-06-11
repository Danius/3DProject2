

struct LineVertex
{
	D3DXVECTOR3 pos;	
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texCoord;

};

// Define our vertex data layout
const D3D10_INPUT_ELEMENT_DESC lineVertexLayout[] =
{
	{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,24,D3D10_INPUT_PER_VERTEX_DATA, 0}
};

struct PARTICLE_VERTEX
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 vel;
    float Timer;
    UINT Type;
};

const D3D10_INPUT_ELEMENT_DESC fireworkLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TIMER", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

const int fireworksNumElements = sizeof(fireworkLayout)/sizeof(fireworkLayout[0]);

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;

	Vertex () : normal( D3DXVECTOR3(0,0,0) )
	{			
	}
	
	Vertex( D3DXVECTOR3 p, D3DXVECTOR3 n ) : pos(p), normal(n)
	{		
	}	

	Vertex( D3DXVECTOR3 p, D3DXVECTOR2 t, D3DXVECTOR3 n ) : pos(p), normal(n), texC(t)
	{
	}	
};
