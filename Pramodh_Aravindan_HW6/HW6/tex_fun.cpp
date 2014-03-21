/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include <math.h>

GzColor	*image;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color, int depth, int max_depth)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* Text the bounds of U and V. Make sure they are within [0,1] */
  if( u < 0 ){u = 0;}
  if( u > 1 ){u = 1;}
  if( v < 0 ){v = 0;}
  if( v > 1 ){v = 1;}

  // compute texel index. Note that the maximum texel X is (xs - 1) and the maximum texel Y is (xy - 1)
  float texelX = u * (xs - 1);
  float texelY = v * (ys - 1);

  // find the texture corner values
  float minX = floor(texelX);
  float maxX = ceil(texelX);
  float minY = floor(texelY);
  float maxY = ceil(texelY);

  //get colors from four cell corners
  GzColor c0, c1, c2, c3;
  
  c0[RED] = image[(int)(minX + minY * xs)][RED];
  c0[GREEN] = image[(int)(minX + minY * xs)][GREEN];
  c0[BLUE] = image[(int)(minX + minY * xs)][BLUE];

  c1[RED] = image[(int)(maxX + minY * xs)][RED];
  c1[GREEN] = image[(int)(maxX + minY * xs)][GREEN];
  c1[BLUE] = image[(int)(maxX + minY * xs)][BLUE];

  c2[RED] = image[(int)(minX + maxY * xs)][RED];
  c2[GREEN] = image[(int)(minX + maxY * xs)][GREEN];
  c2[BLUE] = image[(int)(minX + maxY * xs)][BLUE];

  c3[RED] = image[(int)(maxX + maxY * xs)][RED];
  c3[GREEN] = image[(int)(maxX + maxY * xs)][GREEN];
  c3[BLUE] = image[(int)(maxX + maxY * xs)][BLUE];

  // perform bilinear interpolation
  GzColor colorTopRow, colorBottomRow;
  float left_wt = maxX - texelX;
  float top_wt = maxY - texelY;
  for( i = 0; i < 3; i++ )
  {
	  // interpolate color component from top to bottom
	  colorTopRow[i] = ( left_wt * c0[i] ) + ( ( 1 - left_wt ) * c1[i] );
	  colorBottomRow[i] = ( left_wt * c2[i] ) + ( ( 1 - left_wt ) * c3[i] );

	  // interpolate between top and bottom
	  color[i] = ( top_wt * colorTopRow[i] ) + ( ( 1 - top_wt ) * colorBottomRow[i] );
  }

  return GZ_SUCCESS;

}


/* Procedural texture function */
//Generates a random sequence of colors using a julia set
int ptex_fun(float u, float v, GzColor color, int depth, int max_depth)
{
	float cRe, cIm;
	float newRe, newIm, oldRe, oldIm;
	float zoom = 5;
	int maxIterations = 300;

	cRe = -0.7;
	cIm = 0.27015;

	newRe = 1.5 * (u - 512 / 2) / (0.5 * zoom * 512);
    newIm = (v - 512 / 2) / (0.5 * zoom * 512);
        
	//i represents the number of iterations
        
	int i;
    
    for(i = 0; i < maxIterations; i++)
    {  
		//to hold the value of the previous iteration 
		oldRe = newRe;
		oldIm = newIm;
		//the actual iteration, the real and imaginary part are calculated  
		newRe = oldRe * oldRe - oldIm * oldIm + cRe;
		newIm = 2 * oldRe * oldIm + cIm;
		//if the point is outside the circle with radius 2: stop   
		if((newRe * newRe + newIm * newIm) > 4) break;
  }
        
	//assigning colors based on some random function
	color[RED] = i % 256 * sin(0.5 * depth/max_depth);
	color[GREEN] = (255 - i % 256) * cos(0.5 * depth/max_depth);
	color[BLUE] = (255 * (i < maxIterations)) * sin(0.5 * depth/max_depth);
		
	return GZ_SUCCESS;
}

