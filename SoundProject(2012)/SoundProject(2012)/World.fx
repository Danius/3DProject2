// Input and Output Structures
//-----------------------------------------------------------------------------------------

struct VSIn
{
	float3 Pos  : POS;
	float3 Normal : NORMAL;
	float2 TexCoords : TEXCOORD;
};

struct PSSceneIn
{
	float4 Pos  : SV_Position;
	float2 TexCoords : TEX;
	float LightingFactor : LightFactor;
};

Texture2D frame;
float3 LightPos;
float Ambient;


SamplerState samLinear
{
	//Filter texture bilinear
	Filter = MIN_MAG_MIP_LINEAR;
	
	//repeat texture in u and v when texcoords are > 1.0
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer cbEveryFrame
{
	matrix	ProjectionMatrix;
	matrix	ViewMatrix;
	matrix	WorldMatrix;
	matrix normalMatrix;
	float3 CameraPos;
};

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};


//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;

	float4x4 preViewProjection = mul(ViewMatrix, ProjectionMatrix);
	float4x4 preWorldViewProjection = mul(WorldMatrix, preViewProjection);
	
	output.Pos = mul( float4(input.Pos,1.0), preWorldViewProjection );
	output.TexCoords =  input.TexCoords;
	output.LightingFactor = Ambient;

	float3 tnorm = normalize(mul(WorldMatrix,input.Normal));

	output.LightingFactor += abs(dot(normalize(LightPos), tnorm)) * 2;

	//float4 posWorld = mul(float4(input.Pos,1.0), WorldMatrix);

	//float3 lightDir = normalize(posWorld - LightPos);

	//float3 h = normalize(normalize(CameraPos - posWorld) - lightDir);

	//output.LightingFactor += pow(abs(dot(h, tnorm)), 2);

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{
	return frame.Sample( samLinear, input.TexCoords ) * (input.LightingFactor);
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique10 AVI
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    
	    SetRasterizerState( NoCulling  );
	    SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}

