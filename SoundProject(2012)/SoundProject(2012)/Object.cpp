#include "Object.h"


Object::Object(char* soundFileName)
{
	D3DXQuaternionIdentity(&Rotation);
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&translation);
	tick = 0;
	objectSound = new Sound();
	objectSound->Initialize(soundSystem->fmodSystem, soundFileName);
	spawned = false;
}


Object::~Object(void)
{
}

void Object::addData(Vertex iData)
{
	Data.push_back(iData);
}

void Object::lastFace()
{
	Data.push_back(Data.at(Data.size()-3));								
	Data.push_back(Data.at(Data.size()-2));
}

void Object::getData(std::vector<Vertex> *in)
{
	for(int i = 0;i < Data.size();i++)
	{
		in->push_back(Data.at(i));
	}
}

void Object::InitBuffers(ID3D10Effect* FX,char file[256] )
{
	object_pEffect = FX;
	object_pTechRenderLine = object_pEffect->GetTechniqueByName("AVI");
	object_mfxDiffuseMapVar = object_pEffect->GetVariableByName("frame")->AsShaderResource();

	D3D10_BUFFER_DESC bd;

	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * Data.size();
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	if( FAILED( g_pd3dDevice->CreateBuffer( &bd, 0, &Buffer ) ) )
		MessageBox( 0, "Unable to create Vertex Buffer", "VB Error", 0 );

	D3DX10_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;
	D3DX10CreateShaderResourceViewFromFile( g_pd3dDevice, file, &loadInfo, NULL, &Texture, NULL );

	fData = new Vertex[Data.size()];

	Buffer->Map( D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >(&fData) );

	for(int f = 0;f < Data.size();f++)
	{
		fData[f] = Data.at(f);
	}

	Buffer->Unmap();
}

int Object::getBufferSize()
{
	return Data.size();
}

bool Object::Update(float dt,float mx,float my,float mz)
{
	D3DXMatrixIdentity(&world);
	//D3DXQuaternionIdentity(&Rotation);
	if(!spawned)
	{
		objectSound->Play();
		spawned = true;
	}

	objectSound->Update();
	objectSound->CalculateSoundLevel(getPos(),Cam->getCameraPosition(),Cam->getCameraDirection(),Cam->getCameraUp(),30);
	

	//D3DXQUATERNION qTmp;
	//D3DXQuaternionRotationYawPitchRoll(&qTmp, rotX, rotY, rotZ);

	//Rotation *= qTmp;

	//D3DXMatrixRotationQuaternion(&world, &Rotation);
	
	pos[0] += mx*dt;
	pos[1] += my*dt;
	pos[2] += mz*dt;

	D3DXMatrixTranslation(&translation,pos[0],pos[1],pos[2]);


	world = world *translation;
	return false;
}

void Object::setWorld()
{
	world = translation;
}

void Object::setData(float moveX,float moveY,float moveZ,float scale)
	{
		for(int i = 0;i < Data.size();i++)
		{
			Data.at(i).pos.x = Data.at(i).pos.x*scale;// + moveX*10;
			Data.at(i).pos.y = Data.at(i).pos.y*scale;// + moveY*10;
			Data.at(i).pos.z = Data.at(i).pos.z*scale;// + moveZ*10;
		}
		
		D3DXMatrixTranslation(&translation,moveX*10,moveY*10,moveZ*10);
		
		frameMove = D3DXVECTOR3(moveX*10,moveY*10,moveZ*10);
		rotX = (rand() % 100) ;
		rotY = (rand() % 100) ;
		rotZ = (rand() % 100) ;
		rotX /= 1000000;
		rotY /= 1000000;
		rotZ /= 1000000;
	}

void Object::Draw()
{
	g_pd3dDevice->IASetInputLayout( g_pVertexLayout );
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D10_TECHNIQUE_DESC techDesc;
	object_pTechRenderLine->GetDesc( &techDesc );
	g_pd3dDevice->IASetVertexBuffers( 0, 1, &Buffer, &stride, &offset );
	object_mfxDiffuseMapVar->SetResource(Texture);
	object_pEffect->GetVariableByName( "WorldMatrix" )->AsMatrix()->SetMatrix( (float*)&world);
	for( UINT p = 0; p < techDesc.Passes; p++ )
	{
		object_pTechRenderLine->GetPassByIndex( p )->Apply(0);
		
	}
	g_pd3dDevice->Draw(getBufferSize(), 0);
}

void Object::setPos(float* in)
{
	this->pos = new float[3];
	this->pos[0] = in[0];
	this->pos[1] = in[1];
	this->pos[2] = in[2];
}