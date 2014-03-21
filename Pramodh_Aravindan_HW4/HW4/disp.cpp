/*   CS580 HW 4  */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

int rgb_max;

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/
	if(framebuffer == nullptr)
		return GZ_FAILURE;

	*framebuffer = (char*)calloc(width*height,sizeof(GzPixel));
	
	if(*framebuffer == nullptr)
		return GZ_FAILURE;


	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/
	if(display == nullptr)
		return GZ_FAILURE;
	
	GzDisplay* tempDisplay = (GzDisplay*)calloc(1,sizeof(GzDisplay));
//	GzDisplay* tempDisplay = new GzDisplay;
	if(tempDisplay == nullptr)
		return GZ_FAILURE;

	tempDisplay->dispClass = dispClass;
	tempDisplay->xres = xRes;
	tempDisplay->yres = yRes;
	tempDisplay->open = 1;
	tempDisplay->fbuf = (GzPixel*)calloc(xRes*yRes,sizeof(GzPixel));

	if(tempDisplay->fbuf == nullptr)
		return GZ_FAILURE;

//	tempDisplay = NULL;
//	free(tempDisplay);
	
	*display = tempDisplay;

	
//	delete(tempDisplay);

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	if(display == nullptr)
		return GZ_FAILURE;

	free(display->fbuf);
	display->fbuf = NULL;
	
//	delete(display->fbuf);

	free(display);
	display = NULL;

//	delete(display);

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */
	if(display == nullptr||xRes == nullptr||yRes == nullptr||dispClass == nullptr)
		return GZ_FAILURE;

	*xRes = display->xres;
	*yRes = display->yres;
	*dispClass = display->dispClass;


	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	
	if(display == nullptr)
		return GZ_FAILURE;

	for(int i = 0; i < display->xres; i++)
	{
		for(int j = 0; j < display->yres; j++)
		{
			display->fbuf[ARRAY(j,i)].red = 255;
			display->fbuf[ARRAY(j,i)].blue = 255;
			display->fbuf[ARRAY(j,i)].green = 255;
			display->fbuf[ARRAY(j,i)].alpha = 100;
			display->fbuf[ARRAY(j,i)].z = INT_MAX;
		}
	}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */

	if(display == nullptr)
		return GZ_FAILURE;

	if(i<0||j<0||i>=display->xres||j>=display->yres)
		return GZ_SUCCESS;

	rgb_max = 4096;
	
	//r = int(r/16);
	//g = int(g/16);
	//b = int(b/16);

	if(r > rgb_max)
		r = rgb_max;
	if(g > rgb_max)
		g = rgb_max;
	if(b > rgb_max)
		b = rgb_max;

	display->fbuf[ARRAY(i,j)].red = r;
	display->fbuf[ARRAY(i,j)].green = g;
	display->fbuf[ARRAY(i,j)].blue = b;
	display->fbuf[ARRAY(i,j)].alpha = a;
	display->fbuf[ARRAY(i,j)].z = z;

	
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{

	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
		
	if(display == nullptr||r == nullptr||g == nullptr||b == nullptr||a == nullptr||z == nullptr)
		return GZ_FAILURE;

	if(i<0||j<0||i>=display->xres||j>=display->yres)
		return GZ_FAILURE;


	*r = display->fbuf[ARRAY(i,j)].red;
	*g = display->fbuf[ARRAY(i,j)].green;
	*b = display->fbuf[ARRAY(i,j)].blue;
	*a = display->fbuf[ARRAY(i,j)].alpha;
	*z = display->fbuf[ARRAY(i,j)].z;

	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	//AfxMessageBox("Flushing Display to File");

	if(outfile == nullptr || display == nullptr)
		return GZ_FAILURE;

	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);

	for(int i = 0; i < display->xres*display->yres; i++)
	{					
		fprintf(outfile, "%c%c%c", display->fbuf[i].red >> 4,display->fbuf[i].green >> 4,display->fbuf[i].blue >> 4);		
	}

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/

	if(framebuffer == nullptr || display == nullptr)
		return GZ_FAILURE;

	for(int i = 0; i < display->xres * display->yres; i++)
	{										
			int red,green,blue;
						
			char* blue_address = &(framebuffer[3 * i]);			
			char* green_address = blue_address + 1;
			char* red_address = green_address + 1; 
			
			red = display->fbuf[i].red >> 4;
			green = display->fbuf[i].green >> 4;
			blue = display->fbuf[i].blue >> 4;
			
			*blue_address = blue;			
			*red_address = red;
			*green_address = green;
			
		
	} 

	return GZ_SUCCESS;
}
