#ifndef _STDAFX__H
#define _STDAFX__H

#include <windows.h>
#include <D3D10.h>
#include <D3DX10.h>
#include "FreeImage\FreeImage.h"
#include <string>
#include <vector>
#include "Vertex.h"


#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }
#define PI (3.14159265358979323846f)
const int WIDTH = 1024;
const int HEIGHT = 768;

D3DX10INLINE float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
D3DX10INLINE float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

template <typename T>
D3DX10INLINE T Min(const T&a, const T&b)
{
	return a < b? a : b;
}

#endif
