#pragma once
#include <fmod.hpp>
#include <string>
#include "stdafx.h"


class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();
	void Initialize();
	void Update();
	FMOD::System *fmodSystem;
	
private:
	
	FMOD_RESULT  result;
	unsigned int version;

};