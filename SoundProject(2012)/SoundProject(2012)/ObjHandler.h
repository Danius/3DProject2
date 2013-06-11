#pragma once
#include "stdafx.h"
#include "Object.h"
#include <list>
#include "Loader.h"
#include "SoundSystem.hpp"
#include <thread>

//extern ID3D10Device* g_pd3dDevice;
//extern ID3D10Effect* g_pEffect;
//extern SoundSystem* soundSystem;

class ObjHandler
{
private:
	
	std::vector<Object*> objList;
	Object* Hydralisk;
	Object* Objects;
	Object* Bunker;
	Loader* load;
public:
	ObjHandler(ID3D10Effect* FX);
	~ObjHandler(void);
	void Update(float dt);
	void Draw();
};

