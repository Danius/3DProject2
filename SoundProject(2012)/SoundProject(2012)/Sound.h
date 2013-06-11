#pragma once
#include "SoundSystem.hpp"
#include "ButtonInput.h"
	
extern ButtonInput* buttonInput;

static float previous;
static float a;
static float b;
static float f;
static float SR;

class Sound
{
public:
	Sound();
	~Sound();
	void Initialize(FMOD::System*, char*);
	void Play();
	void Update();
	void CalculateSoundLevel(float[3], float[3], float[3],float[3], float);
	//Insert filterfunktion
	void crossProduct(float[], float[], float[]);
	void normalize(float[], float[]) ;
	void SetStereoratio(float,float);//0-1.0f

	void SwitchSounds();

	

private:
	

	FMOD::System *fmodSystem;
	FMOD::Sound *sound1;
	FMOD::DSP *customDSP;
	FMOD::DSP *highDSP;
	FMOD::DSP *echoDSP;
	FMOD::ChannelGroup *mastergroup;
    FMOD::Channel *channel;
	FMOD::Channel *channelEcho;
	FMOD_RESULT  result;

	int echoDelayMS;
	bool temp;
	bool echoActive;
	unsigned int index;

	
};