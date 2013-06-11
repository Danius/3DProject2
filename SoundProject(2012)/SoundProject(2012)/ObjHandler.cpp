#include "ObjHandler.h"

ObjHandler::ObjHandler(ID3D10Effect* FX)
{
	load = new Loader();


	objList.push_back(new Object("Sounds/ZerglingSoundTroll.mp3"));
	objList.push_back(new Object("Sounds/NomNomNom.mp3"));
	objList.push_back(new Object("Sounds/StereoHearts.mp3"));
	objList.push_back(new Object("Sounds/raven_poe.mp3"));
	
	load->LoadObject("Object files\\Bunker\\Bunker.obj",0,0,0,5,objList.at(0),1,-1,1);
	load->LoadObject("Object files\\Bunker\\Bunker.obj",0,-40,0,5,objList.at(1),1,-1,1);
	load->LoadObject("Object files\\Bunker\\Bunker.obj",0,-80,0,5,objList.at(2),1,-1,1);
	load->LoadObject("Object files\\Bunker\\Bunker.obj",-40,-40,0,5,objList.at(3),1,-1,1);

	objList.at(0)->InitBuffers(FX,"Pics/Terrain_texture.jpg");
	objList.at(1)->InitBuffers(FX,"Pics/Terrain_texture.jpg");
	objList.at(2)->InitBuffers(FX,"Pics/Terrain_texture.jpg");
	objList.at(3)->InitBuffers(FX,"Pics/Terrain_texture.jpg");
}


ObjHandler::~ObjHandler(void)
{
}

void ObjHandler::Update(float dt)
{

	objList.at(0)->Update(dt,0,0,0);
	objList.at(1)->Update(dt,0,0,0);
	objList.at(2)->Update(dt,0,0,0);
	objList.at(3)->Update(dt,0,0,0);
}

void ObjHandler::Draw()
{
	objList.at(0)->Draw();
	objList.at(1)->Draw();
	objList.at(2)->Draw();
	objList.at(3)->Draw();
}