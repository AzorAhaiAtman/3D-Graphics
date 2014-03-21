#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define WIREFRAME 0

short ctoi(float color);
void rasterize(GzRender *render, GzPointer *valueList, GzToken i);
void line(float x0, float y0, float x1, float y1, GzRender *render);


int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes
- check for legal class GZ_Z_BUFFER_RENDER
*/
	if(render == nullptr || display == nullptr)
	{
		return GZ_FAILURE;
	}

	// create a temporary renderer
	GzRender* tempRender = (GzRender*)calloc(1,sizeof(GzRender));

	if(tempRender == nullptr)
	{
		return GZ_FAILURE;
	}

	tempRender->renderClass = renderClass;
	tempRender->display = display;
	tempRender->open = 0;

	//assign render to temprender
	*render = tempRender;

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if(render == nullptr)
	{
		return GZ_FAILURE;
	}

	render->display = NULL;
	free(render);
	render = NULL;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
	if(render == nullptr)
	{
		return GZ_FAILURE;
	}
	//set flatcolor to 127	
	render->flatcolor[0] = 127;
	render->flatcolor[1] = 127;
	render->flatcolor[2] = 127;	
	render->open = 1;

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{

/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for(int i = 0; i < numAttributes; i++)
	{
		//check if namelist corresponds to GZ_RGB_COLOR
		if(nameList[i] = GZ_RGB_COLOR)
		{

			GzColor* colorpointer = (GzColor*)(valueList[i]);

			render->flatcolor[0] = colorpointer[0][0];
			render->flatcolor[1] = colorpointer[0][1];
			render->flatcolor[2] = colorpointer[0][2];

		}
	}

	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
	for(int i = 0; i < numParts; i++){
	
		if(nameList[i] == GZ_NULL_TOKEN)
		{
		AfxMessageBox("Null Token");
		}
		else if(nameList[i] == GZ_POSITION)
		{
			//rasterize using Linear Expression Evaluation
			rasterize(render,valueList,i);			
		}
	}
	
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void rasterize(GzRender *render, GzPointer *valueList, GzToken i)
{
	//cast void pointer valueList to Gz Coordinates	
	GzCoord* triVertex = (GzCoord*)(valueList[i]);
		
		float x1 = triVertex[0][0];
		float x2 = triVertex[1][0];
		float x3 = triVertex[2][0];
		float y1 = triVertex[0][1];
		float y2 = triVertex[1][1];
		float y3 = triVertex[2][1];
		float z1 = triVertex[0][2];
		float z2 = triVertex[1][2];
		float z3 = triVertex[2][2];
		
		float xMin,yMin,xMax,yMax;
		float a1,b1,c1,a2,b2,c2,a3,b3,c3;
		float aX,bY,cZ,d;
		int xStart,yStart,xEnd,yEnd;
		
		/*	xMin, xMax, yMin, yMax are calculated for fixing the bounding box triangle	*/
		xMin = min(x1,min(x2,x3));
		xMax = max(x1,max(x2,x3));

		yMin = min(y1,min(y2,y3));
		yMax = max(y1,max(y2,y3));

		/*	Computing Line Equation coefficients */
		
		// First Edge (x1,y1) <- (x2,y2)
		a1 = y1 - y2;
		b1 = x2 - x1;
		c1 = ((x1 - x2) * y2) - ((y1 - y2) * x2);

		//Second Edge (x2,y2) <- (x3,y3)
		a2 = y2 - y3;
		b2 = x3 - x2;
		c2 = ((x2 - x3) * y3) - ((y2 - y3) * x3);

		//Third Edge (x3,y3) <- (x1,y1)
		a3 = y3 - y1;
		b3 = x1 - x3;
		c3 = ((x3 - x1) * y1) - ((y3 - y1) * x1);

		/*	Finding Crossproduct of 2 edges to obtain the plane equation for the triangle	*/
	
		aX = (((y1 - y2) * (z2 - z3)) - ((z1 - z2) * (y2 - y3)));
		bY = -(((x1 - x2) * (z2 - z3)) - ((z1 - z2) * (x2 - x3)));
		cZ = (((x1 - x2) * (y2 - y3)) - ((y1 - y2) * (x2 - x3)));
		d = -(aX * x1 + bY * y1 + cZ * z1);
				
		xStart = int(ceil((xMin)));
		yStart = int(ceil((yMin)));
	
		xEnd = int(floor((xMax)));
		yEnd = int(floor((yMax)));
				
		if(WIREFRAME)
		{
			line(x1,y1,x2,y2,render);
			line(x2,y2,x3,y3,render);
			line(x3,y3,x1,y1,render);
		}
		else
		{

			for(int y  = yStart; y <= yEnd; y++)
			{
				for(int x = xStart; x <= xEnd; x++)
				{
					if((a1 * x + b1 * y + c1) >= 0 && (a2 * x + b2 * y + c2) >= 0 && (a3 * x + b3 * y + c3) >= 0  || ((a1 * x + b1 * y + c1) < 0 && (a2 * x + b2 * y + c2) < 0 && (a3 * x + b3 * y + c3) < 0))
					{
						//Interpolate new Z
						GzDepth newZ = (GzDepth)(-(aX * x + bY * y + d) / cZ);

						if(newZ > 0)//If new Z lies in front of the screen
						{
							GzIntensity red,green,blue,alpha;
							GzDepth oldZ;				
					
							GzGetDisplay(render->display,x,y,&red,&green,&blue,&alpha,&oldZ);
				
							if(newZ < oldZ)//compare new Z with old Z
							{					
								GzPutDisplay(render->display,x,y,ctoi(render->flatcolor[0]),ctoi(render->flatcolor[1]),ctoi(render->flatcolor[2]),alpha,newZ);
							}	
						}			
					}												
				}
			}
		}	
}

void line(float x0, float y0, float x1, float y1, GzRender *render)
{
	float dx = x1 - x0;
	float dy = y1 - y0;
	float m = dy/dx;
	float x = x0, y = y0;

	for(x = x0; x <= x1; x++)
	{
		GzIntensity red, green, blue, alpha; 
		GzDepth oldZ;
		GzGetDisplay(render->display,x,y,&red,&green,&blue,&alpha,&oldZ);
		GzPutDisplay(render->display,x,y,ctoi(render->flatcolor[0]),ctoi(render->flatcolor[1]),ctoi(render->flatcolor[2]),alpha,oldZ);
		y = y + m;
	}
}

