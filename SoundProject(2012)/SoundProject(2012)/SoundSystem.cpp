#pragma once
#include "SoundSystem.hpp"

//#include "DSPfilters.h"


SoundSystem::SoundSystem()
{}

SoundSystem::~SoundSystem()
{
	result = fmodSystem->close();
    result = fmodSystem->release();
}

void SoundSystem::Initialize()
{
	result = FMOD::System_Create(&fmodSystem);
	result = fmodSystem->getVersion(&version);
	result = fmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	//ERRCHECK(result);
}

void SoundSystem::Update()
{

	fmodSystem->update();
}

