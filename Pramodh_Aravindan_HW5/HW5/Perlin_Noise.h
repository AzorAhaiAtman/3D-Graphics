#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include    "disp.h"
#include	"rend.h"
#include    "VectorFunctions.h"
#include    "tex_fun.h"
#include    "Application5.h"

#include<math.h>

float octave_noise(const float octaves,const float persistence,const float scale,const float x,const float y,const float z);                                                                                                  
float scaled_octave_noise(const float octaves,const float persistence,const float scale,const float loBound,const float hiBound,const float x,const float y,const float z);                                                                                                                                                                        
float raw_noise(const float x, const float y, const float z);
void calculateNoise(float *normalWithNoisePointer, float *normalPointer);