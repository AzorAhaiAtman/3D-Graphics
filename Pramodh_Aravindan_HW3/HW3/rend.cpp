/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define WIREFRAME 0

void GzRasterize(GzRender *render,GzCoord *triVertex);
void GzBuildXsp(GzRender *render);
void GzBuildXpi(GzRender *render);
void GzBuildXiw(GzRender *render);
void line(float x0, float y0, float x1, float y1, GzRender *render);
short ctoi( float color );


int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value

	/*	1	0			0			0 
		0	cos(theta)	-sin(theta)	0
		0	sin(theta)	cos(theta)	0
		0	0			0			1	*/
	
	float theta = degree * 3.14 / 180;

	mat[0][0] = 1;
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;

	mat[0][1] = 0;
	mat[1][1] = cos(theta); 
	mat[2][1] = sin(theta);
	mat[3][1] = 0;

	mat[0][2] = 0;
	mat[1][2] = -sin(theta);
	mat[2][2] = cos(theta);
	mat[3][2] = 0;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;
	
	return GZ_SUCCESS;
}

int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	
	/*	cos(theta)		0	sin(theta)		0 
		0				1	0				0
		-sin(theta)		0	cos(theta)		0
		0				0	0				1	*/

	float theta = degree * 3.14 / 180;

	mat[0][0] = cos(theta);
	mat[1][0] = 0;
	mat[2][0] = -sin(theta);
	mat[3][0] = 0;

	mat[0][1] = 0;
	mat[1][1] = 1; 
	mat[2][1] = 0;
	mat[3][1] = 0;

	mat[0][2] = sin(theta);
	mat[1][2] = 0;
	mat[2][2] = cos(theta);
	mat[3][2] = 0;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	
	/*	cos(theta)		-sin(theta)	0		0 
		sin(theta)		cos(theta)	0		0
		0				0			1		0
		0				0			0		1	*/
	
	float theta = degree * 3.14 / 180;

	mat[0][0] = cos(theta);
	mat[1][0] = sin(theta);
	mat[2][0] = 0;
	mat[3][0] = 0;

	mat[0][1] = -sin(theta);
	mat[1][1] = cos(theta); 
	mat[2][1] = 0;
	mat[3][1] = 0;

	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = 1;
	mat[3][2] = 0;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	
	/*	1	0	0	tx
		0	1	0	ty
		0	0	1	tz
		0	0	0	1	*/

	mat[0][0] = 1;
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;

	mat[0][1] = 0;
	mat[1][1] = 1; 
	mat[2][1] = 0;
	mat[3][1] = 0;

	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = 1;
	mat[3][2] = 0;

	mat[0][3] = translate[X];
	mat[1][3] = translate[Y];
	mat[2][3] = translate[Z];
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	/* sx	0	0	0
		0	sy	0	0
		0	0	sz	0
		0	0	0	1	*/

	mat[0][0] = scale[X];
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;

	mat[0][1] = 0;
	mat[1][1] = scale[Y]; 
	mat[2][1] = 0;
	mat[3][1] = 0;

	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = scale[Z];
	mat[3][2] = 0;

	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 
	if(render == nullptr || display == nullptr)
	{
		return GZ_FAILURE;
	}

	//check render class
	if(renderClass != GZ_Z_BUFFER_RENDER)
	{
		return GZ_FAILURE;
	}

	//create a temporary render
	GzRender* tempRender = (GzRender*)calloc(1,sizeof(GzRender));

	if(tempRender == nullptr)
	{
		return GZ_FAILURE;
	}

	tempRender->renderClass = renderClass;
	tempRender->display = display;
	tempRender->open = 0;

	tempRender->camera.FOV = DEFAULT_FOV;

	tempRender->camera.lookat[X] = 0;
	tempRender->camera.lookat[Y] = 0;
	tempRender->camera.lookat[Z] = 0;
	
	tempRender->camera.position[X] = DEFAULT_IM_X;
	tempRender->camera.position[Y] = DEFAULT_IM_Y;
	tempRender->camera.position[Z] = DEFAULT_IM_X;

	tempRender->camera.worldup[X] = 0;
	tempRender->camera.worldup[Y] = 1;
	tempRender->camera.worldup[Z] = 0;
		
	//assign render to tempRender
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

int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 

	if(render == nullptr)
	{
		return GZ_FAILURE;
	}

	// Open the renderer
	render->open = 1;

	render->flatcolor[0] = 127;
	render->flatcolor[1] = 127;
	render->flatcolor[2] = 127;	
	
	// Initialize stack to be empty
	render->matlevel = -1;	
	
	//Build Xsp Matrix and push onto stack
	GzBuildXsp(render);	
	if(render->Xsp == nullptr)
	{
		return GZ_FAILURE;
	}
	GzPushMatrix(render,render->Xsp);			
	
	//Build Xpi Matrix and push onto stack
	GzBuildXpi(render);
	if(render->camera.Xpi == nullptr)
	{
		return GZ_FAILURE;
	}
	GzPushMatrix(render,render->camera.Xpi);
	
	//Build Xiw Matrix and push onto stack
	GzBuildXiw(render);	
	if(render->camera.Xiw == nullptr)
	{
		return GZ_FAILURE;
	}
	GzPushMatrix(render,render->camera.Xiw);
		
	return GZ_SUCCESS;
}


int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if(render == nullptr || camera == nullptr)
	{
		return GZ_FAILURE;
	}

	render->camera.FOV = camera->FOV;
	
	for(int i = 0; i < 3; i++)
	{
		render->camera.lookat[i] = camera->lookat[i];
		render->camera.position[i] = camera->position[i];
		render->camera.worldup[i] = camera->worldup[i];
	}

	for(int j = 0; j < 4; j++)
	{
		for(int k = 0; k < 4; k++)
		{
			render->camera.Xiw[j][k] = camera->Xiw[j][k];
			render->camera.Xpi[j][k] = camera->Xpi[j][k];
		}
	}
		
	return GZ_SUCCESS;	
}


int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if(render == nullptr || render->matlevel == MATLEVELS)
	{
		return GZ_FAILURE;
	}

	if(render->matlevel == -1) // If the stack is empty
	{
		short matIndex = render->matlevel + 1;

		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				render->Ximage[matIndex][i][j] = matrix[i][j];
			}
		}
	}
	else
	{
		GzMatrix temp1;
		GzMatrix temp2;
		GzMatrix result;
		
		short matIndex = render->matlevel;

		//initialize matrix variables
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				temp1[i][j] = render->Ximage[matIndex][i][j];
				temp2[i][j] = matrix[i][j];
				result[i][j] = 0;
			}
		}
		
		// 4 X 4 Matrix multiplication	
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				for(int k = 0; k < 4; k++)
				{
					result[i][j] = result[i][j] + (temp1[i][k] * temp2[k][j]);
				}
			}
		}
		
		matIndex++;

		//transfer from temp matrix to Ximage stack
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				render->Ximage[matIndex][i][j] = result[i][j];
				result[i][j] = 0;
				temp1[i][j] = 0;
				temp2[i][j] = 0;
			}
		}
				
	}
	//increase stack index
	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render == nullptr || render->matlevel == -1)
	{
		return GZ_FAILURE;
	}

	//decrease stack index
	render->matlevel--;

	return GZ_SUCCESS;
}

int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
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


int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 

	if(render == nullptr || nameList == nullptr || valueList == nullptr)
		return GZ_FAILURE;

	for(int i = 0; i < numParts; i++)
	{
		if(nameList[i] == GZ_POSITION)
		{
			//Get object space vertices from valueList
			GzCoord *objectVertex = (GzCoord*)(valueList[i]);			

			//create memory for world space vertices
			GzCoord *worldVertex = (GzCoord*)calloc(3,sizeof(GzCoord));

			for(int index = 0; index < 3; index++)
			{
				//transform object space vertex to world space vertex				
				//get the top matrix from the Ximage stack
				GzMatrix* topMatrix = (GzMatrix*)(render->Ximage[render->matlevel]);

				//perform homogenous matrix multiplication of objectVertex with Ximage matrix
				float temp[4];
				float homoGzCoord[4];

				for(int u = 0; u < 3; u++)
				{
					homoGzCoord[u] = objectVertex[index][u];					
				}
				homoGzCoord[3] = 1;

				for(int v = 0; v < 4; v++)
				{
					temp[v] = 0;
				}
	
				/* Transforming the object vertex to world vertex by multiplying with top Ximage matrix on stack */
				//homogenous matrix multiplication [4][4] X [4][1] Matrix
				for(int row = 0; row < 4; row++)
				{
					for(int col = 0; col < 4; col++)
					{
						temp[row] = temp[row] + (topMatrix[0][row][col] * homoGzCoord[col]);
					}
				}

				//Convert world vertex from homogenous coordinates to normal GzCoordinate by normalizing it
				for(int w = 0; w < 3; w++)
				{
					worldVertex[index][w] = temp[w] / temp[3];					
				}			
			}
			
			//checking if triangle has vertices behind view plane. If yes ignore triangle.
			if(worldVertex[0][Z] < 0 || worldVertex[1][Z] < 0 || worldVertex[2][Z] < 0)
			{
				continue;
			}
			//check world triangle coordinates. If a vertex lies completely outside then skip it
			else if((worldVertex[0][X] < 0 && worldVertex[1][X] < 0 && worldVertex[2][X] < 0) || (worldVertex[0][X] > render->display->xres && worldVertex[1][X] > render->display->xres && worldVertex[2][X] > render->display->xres)
			|| (worldVertex[0][Y] < 0 && worldVertex[1][Y] < 0 && worldVertex[2][Y] < 0) || (worldVertex[0][Y] > render->display->yres && worldVertex[1][Y] > render->display->yres && worldVertex[2][Y] > render->display->yres))
			{
				continue;
			}
			//Rasterize the triangle
			else
			{
				//Rasterize triangle using Linear Expression Evaluation
				GzRasterize(render,worldVertex);
			}

		} 
	} 

	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}


void GzRasterize(GzRender *render,GzCoord *triVertex)
{		

	//AfxMessageBox("Rasterize");				
		
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
		
		// First Edge (x1,y1) -> (x2,y2)
		a1 = y1 - y2;
		b1 = x2 - x1;
		c1 = ((x1 - x2) * y2) - ((y1 - y2) * x2);

		//Second Edge (x2,y2) -> (x3,y3)
		a2 = y2 - y3;
		b2 = x3 - x2;
		c2 = ((x2 - x3) * y3) - ((y2 - y3) * x3);

		//Third Edge (x3,y3) -> (x1,y1)
		a3 = y3 - y1;
		b3 = x1 - x3;
		c3 = ((x3 - x1) * y1) - ((y3 - y1) * x1);

		/*	Finding Crossproduct of 2 edges to obtain the plane equation for the triangle	*/
	
		aX = (((y1 - y2) * (z2 - z3)) - ((z1 - z2) * (y2 - y3)));
		bY = -(((x1 - x2) * (z2 - z3)) - ((z1 - z2) * (x2 - x3)));
		cZ = (((x1 - x2) * (y2 - y3)) - ((y1 - y2) * (x2 - x3)));
		d = -(aX * x1 + bY * y1 + cZ * z1);
				
		xStart = max(int(ceil((xMin))),0);
		yStart = max(int(ceil((yMin))),0);
	
		xEnd = min(int(floor((xMax))),render->display->xres);
		yEnd = min(int(floor((yMax))),render->display->yres);
				
		if(WIREFRAME)
		{
			line(x1,y1,x2,y2,render);
			line(x2,y2,x3,y3,render);
			line(x3,y3,x1,y1,render);
		}
		else{
		for(int y  = yStart; y <= yEnd; y++)
		{
			for(int x = xStart; x <= xEnd; x++)
			{				
				//check if x,y lies outside the line. If yes, skip triangle and move to next
				if (a1 * x + b1 * y + c1 < 0)
				{
						continue;
				}
				if(a2 * x + b2 * y + c2 < 0)
				{
						continue;
				}
				if(a3 * x + b3 * y + c3 < 0)
				{
						continue; 
				}													
					GzDepth newZ = (GzDepth)(-(aX * x + bY * y + d) / cZ);

					if(newZ > 0)
					{
						GzIntensity red,green,blue,alpha;
						GzDepth oldZ;				
					
						GzGetDisplay(render->display,x,y,&red,&green,&blue,&alpha,&oldZ);
				
						if(newZ < oldZ)
						{					
							GzPutDisplay(render->display,x,y,ctoi(render->flatcolor[0]),ctoi(render->flatcolor[1]),ctoi(render->flatcolor[2]),alpha,newZ);							
						}	
					}			
				}												
			
		}
		}
	}

void GzBuildXsp(GzRender *render)
{
	/*	xs/2	0		0		xs/2
		0		-ys/2	0		ys/2
		0		0		1/d		0
		0		0		0		1	*/
	
	float theta = (float)(render->camera.FOV * 3.14 / 180);	
	float d = (float)(1/tan(theta/2));

	float xs2 = (float)render->display->xres/2;
	float ys2 = (float)render->display->yres/2;

	render->Xsp[0][0] = xs2;
	render->Xsp[1][0] = 0;
	render->Xsp[2][0] = 0;
	render->Xsp[3][0] = 0;

	render->Xsp[0][1] = 0;
	render->Xsp[1][1] = -ys2;
	render->Xsp[2][1] = 0;
	render->Xsp[3][1] = 0;

	render->Xsp[0][2] = 0;
	render->Xsp[1][2] = 0;
	render->Xsp[2][2] = INT_MAX/d;
	render->Xsp[3][2] = 0;

	render->Xsp[0][3] = xs2;
	render->Xsp[1][3] = ys2;
	render->Xsp[2][3] = 0;
	render->Xsp[3][3] = 1;

}

void GzBuildXpi(GzRender *render)
{
	/*	1	0	0	0
		0	1	0	0
		0	0	1	0
		0	0	1/d	0	*/

	float theta = (float)(render->camera.FOV * 3.14 / 180);	
	float d = (float)(1/tan(theta/2));

	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[1][0] = 0;
	render->camera.Xpi[2][0] = 0;
	render->camera.Xpi[3][0] = 0;

	render->camera.Xpi[0][1] = 0;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[2][1] = 0;
	render->camera.Xpi[3][1] = 0;

	render->camera.Xpi[0][2] = 0;
	render->camera.Xpi[1][2] = 0;
	render->camera.Xpi[2][2] = 1;
	render->camera.Xpi[3][2] = 1/d;

	render->camera.Xpi[0][3] = 0;
	render->camera.Xpi[1][3] = 0;
	render->camera.Xpi[2][3] = 0;
	render->camera.Xpi[3][3] = 1;
}

void GzBuildXiw(GzRender *render)
{
	GzCoord cX,cY,cZ;
	GzCoord upcZXZ;

	//cZ vector = cI/|cI|
	for(int i = 0; i < 3; i++)
	{
		cZ[i] = (render->camera.lookat[i]) - (render->camera.position[i]);
	}
	//Normalize cZ vector
	float cz = sqrt(cZ[X] * cZ[X] + cZ[Y] * cZ[Y] + cZ[Z] * cZ[Z]);
	for(int i = 0; i < 3; i++)
	{
		cZ[i] = cZ[i]/cz;
	}
	//End of cZ vector
	
	
	//cY Vector = up' / |up'|
	//where up' = up - (up . Z)Z
	
	//calculating (up.cZ) (dot product)
	float upcZ = (render->camera.worldup[X] * cZ[X]) + (render->camera.worldup[Y] * cZ[Y]) + (render->camera.worldup[Z] * cZ[Z]);

	//scaling upcZ with cam i.e (up.cZ) * cZ
	for(int i = 0; i < 3; i++)
	{
		upcZXZ[i] = cZ[i] * upcZ;
	}

	//cY vector = up - (up.cZ)cZ
	for(int i = 0; i < 3; i++)
	{
		cY[i] = render->camera.worldup[i] - upcZXZ[i];
	}
	//Normalize cY vector
	float cy = sqrt(cY[X] * cY[X] + cY[Y] * cY[Y] + cY[Z] * cY[Z]);
	for(int i = 0; i < 3; i++)
	{
		cY[i] = cY[i]/cy;
	}
	//End of cY vector

	//cX vector = cY * cZ
	cX[X] = cY[Y] * cZ[Z] - cY[Z] * cZ[Y];
	cX[Y] = cY[Z] * cZ[X] - cY[X] * cZ[Z];
	cX[Z] = cY[X] * cZ[Y] - cY[Y] * cZ[X];
	//cX is already normalized
	//End of cX vector

	//calculating X.C, Y.C and Z.C for the Xiw matrix (dotproduct)
	float cXdotC = cX[X] * render->camera.position[X] + cX[Y] * render->camera.position[Y] + cX[Z] * render->camera.position[Z];
	float cYdotC = cY[X] * render->camera.position[X] + cY[Y] * render->camera.position[Y] + cY[Z] * render->camera.position[Z];
	float cZdotC = cZ[X] * render->camera.position[X] + cZ[Y] * render->camera.position[Y] + cZ[Z] * render->camera.position[Z];

	//Assigning values to the Xiw matrix
	/*	Xx	Xy	Xz	-X.C
		Yx	Yy	Yz	-Y.C
		Zx	Zy	Zz	-Z.C
		0	0	0	1
	*/
	
	render->camera.Xiw[0][0] = cX[X];
	render->camera.Xiw[1][0] = cY[X];
	render->camera.Xiw[2][0] = cZ[X];
	render->camera.Xiw[3][0] = 0;

	render->camera.Xiw[0][1] = cX[Y];
	render->camera.Xiw[1][1] = cY[Y];
	render->camera.Xiw[2][1] = cZ[Y];
	render->camera.Xiw[3][1] = 0;

	render->camera.Xiw[0][2] = cX[Z];
	render->camera.Xiw[1][2] = cY[Z];
	render->camera.Xiw[2][2] = cZ[Z];
	render->camera.Xiw[3][2] = 0;

	render->camera.Xiw[0][3] = -cXdotC;
	render->camera.Xiw[1][3] = -cYdotC;
	render->camera.Xiw[2][3] = -cZdotC;
	render->camera.Xiw[3][3] = 1;

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

