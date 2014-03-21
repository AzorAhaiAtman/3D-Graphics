#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include    "disp.h"
#include	"rend.h"
#include    "ToonShader.h"

#define XRES 256
#define YRES 256

int quantize4(int color);
int quantize5(int color);
GzIntensity clampGzIntensityValue(GzIntensity var);

void toonShading(GzDisplay *display)
{
	//section 1
	short Gx[3][3] = {	1, 0, -1,
						2, 0, -2,
						1, 0, -1
				   };
	short Gy[3][3] = { 	1, 2, 1,
						0, 0, 0,
						-1, -2, -1
				   };
	short *psI = (short*)malloc(sizeof(short) * XRES * YRES);
	short *rsI = (short*)malloc(sizeof(short) * XRES * YRES);
	short *rsI2 = (short*)malloc(sizeof(short) * XRES * YRES);
	
	for (int q=0; q<XRES; q++)
	{
		psI[0 + q*XRES] = 0;
		psI[XRES-1 + q*XRES] = 0;
	}
	for (int p=0; p<YRES; p++)
	{
		psI[p + 0] = 0;
		psI[p + YRES-1*XRES] = 0;
	}

	//section2
	for (int p=1; p<XRES; p++)
		for (int q=1; q<YRES; q++)
		{
			float grey = ((0.3*display->fbuf[(p-1)+(q-1)*display->xres].red) + (0.59*display->fbuf[(p-1)+(q-1)*display->xres].green) + (0.11*display->fbuf[(p-1)+(q-1)*display->xres].blue))/4095;
			psI[p+q*XRES] = grey*4095;
		}

	//section3
	for (int p=0; p<XRES; p++)
		for (int q=0; q<YRES; q++)
		{
			rsI[p+q*XRES] = (Gx[0][0]*psI[p+q*XRES]) + (Gx[0][1]*psI[p+(q+1)*XRES]) + (Gx[0][2]*psI[p+(q+2)*XRES]) + 
				(Gx[1][0]*psI[p+1+q*XRES]) + (Gx[1][1]*psI[p+1+(q+1)*XRES]) + (Gx[1][2]*psI[p+1+(q+2)*XRES]) + 
				(Gx[2][0]*psI[p+2+q*XRES]) + (Gx[2][1]*psI[p+2+(q+1)*XRES]) + (Gx[2][2]*psI[p+2+(q+2)*XRES]);
		}
	//section4
	for (int p=0; p<XRES; p++)
		for (int q=0; q<YRES; q++)
		{
			rsI2[p+q*XRES] = (Gy[0][0]*psI[p+q*XRES]) + (Gy[0][1]*psI[p+(q+1)*XRES]) + (Gy[0][2]*psI[p+(q+2)*XRES]) + 
				(Gy[1][0]*psI[p+1+q*XRES]) + (Gy[1][1]*psI[p+1+(q+1)*XRES]) + (Gy[1][2]*psI[p+1+(q+2)*XRES]) + 
				(Gy[2][0]*psI[p+2+q*XRES]) + (Gy[2][1]*psI[p+2+(q+1)*XRES]) + (Gy[2][2]*psI[p+2+(q+2)*XRES]);
		}

	for (int p=0; p<XRES; p++)
		for (int q=0; q<YRES; q++)
		{
			if(clampGzIntensityValue(sqrt(float((rsI[p+q*XRES]*rsI[p+q*XRES]) +  (rsI2[p+q*XRES]*rsI2[p+q*XRES])))) > 3000)
			{
				display->fbuf[(p)+(q)*display->xres].red= 4095-clampGzIntensityValue(sqrt(float((rsI[p+q*XRES]*rsI[p+q*XRES]) +  (rsI2[p+q*XRES]*rsI2[p+q*XRES]))));
				display->fbuf[(p)+(q)*display->xres].green= 4095-clampGzIntensityValue(sqrt(float((rsI[p+q*XRES]*rsI[p+q*XRES]) +  (rsI2[p+q*XRES]*rsI2[p+q*XRES]))));
				display->fbuf[(p)+(q)*display->xres].blue= 4095-clampGzIntensityValue(sqrt(float((rsI[p+q*XRES]*rsI[p+q*XRES]) +  (rsI2[p+q*XRES]*rsI2[p+q*XRES]))));
			}
			else
			{
				display->fbuf[(p)+(q)*display->xres].red= quantize5(display->fbuf[(p)+(q)*display->xres].red);		//or use quantize4
				display->fbuf[(p)+(q)*display->xres].green= quantize4(display->fbuf[(p)+(q)*display->xres].green);
				display->fbuf[(p)+(q)*display->xres].blue= quantize5(display->fbuf[(p)+(q)*display->xres].blue);
			}
		}
}

GzIntensity clampGzIntensityValue(GzIntensity var)
{
	/* Make sure var lies between 0 and 4095 */

	if(var < 0)
		return 0;
	else if(var > 4095)
		return 4095;
	else 
		return var;
}

int quantize5(int color)
{
	if(color>0 && color<820)
		color = 820;
	else if(color>820 && color<1640)
		color = 1640;
	else if(color>1640 && color<2458)
		color = 2458;
	else if(color>2458 && color<3278)
		color = 3278;
	else if(color>3278 && color<4095)
		color = 4095;

	return color;
}

int quantize4(int color)
{
	if(color>0 && color<1000)
		color = 1000;
	else if(color>1000 && color<2000)
		color = 2000;
	else if(color>2000 && color<3000)
		color = 3000;
	else if(color>3000 && color<4095)
		color = 4095;

	return color;
}