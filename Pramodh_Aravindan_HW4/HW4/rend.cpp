/* CS580 Homework 4*/

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

//function declaration
short ctoi(float color);
void GzBuildXiw(GzRender *render);
void GzBuildXpi(GzRender *render);
void GzBuildXsp(GzRender *render);
void GzRasterize(GzRender *render,GzCoord *imageV,GzCoord *imageN,GzCoord *screenV);
void GzShade(GzRender *render,const GzCoord imageV,const GzCoord imageN,GzColor color);

//structure GzEdge to store start and end coordinates of triangle
typedef struct
{
	GzCoord start;
	GzCoord end;	
}GzEdge;


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

	/*  sx	0	0	0
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
		
	tempRender->ambientlight.color[RED] = 0.0f;
	tempRender->ambientlight.color[GREEN] = 0.0f;
	tempRender->ambientlight.color[BLUE] = 0.0f;

	tempRender->numlights = 0;
	tempRender->spec = DEFAULT_SPEC;
	tempRender->interp_mode = GZ_RGB_COLOR;
	
	GzColor ambient = DEFAULT_AMBIENT;
	GzColor	specular = DEFAULT_SPECULAR;
	GzColor diffuse = DEFAULT_DIFFUSE;	

	tempRender->Ka[RED] = ambient[RED];
	tempRender->Ka[GREEN] = ambient[GREEN];
	tempRender->Ka[BLUE] = ambient[BLUE];

	tempRender->Kd[RED] = diffuse[RED];
	tempRender->Kd[GREEN] = diffuse[GREEN];
	tempRender->Kd[BLUE] = diffuse[BLUE];

	tempRender->Ks[RED] = specular[RED];
	tempRender->Ks[GREEN] = specular[GREEN];
	tempRender->Ks[BLUE] = specular[BLUE];	
	
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

	render->flatcolor[RED] = 255;
	render->flatcolor[GREEN] = 0;
	render->flatcolor[BLUE] = 255;	
	
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

	//Open the renderer
	render->open = 1;

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

	if(render->matlevel == -1)
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
	render->matlevel++;

	//For Xsp and Xpi. i.e when the matrix level on the stack is 0 or 1
	if(render->matlevel == 0 || render->matlevel == 1)
	{
		//create an identity matrix 4 X 4
		GzMatrix idMatrix;

		idMatrix[0][0] = 1;
		idMatrix[1][0] = 0;
		idMatrix[2][0] = 0;
		idMatrix[3][0] = 0;

		idMatrix[0][1] = 0;
		idMatrix[1][1] = 1;
		idMatrix[2][1] = 0;
		idMatrix[3][1] = 0;

		idMatrix[0][2] = 0;
		idMatrix[1][2] = 0;
		idMatrix[2][2] = 1;
		idMatrix[3][2] = 0;

		idMatrix[0][3] = 0;
		idMatrix[1][3] = 0;
		idMatrix[2][3] = 0;
		idMatrix[3][3] = 1;

		//Push the identity matrix on top of the stack
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				render->Xnorm[render->matlevel][i][j] = idMatrix[i][j];
			}
		}		
	}
	else
	{
		//create a temp matrix that is the top of stack matrix without any translation element
		GzMatrix tempMatrix;

		tempMatrix[0][0] = matrix[0][0];
		tempMatrix[1][0] = matrix[1][0];
		tempMatrix[2][0] = matrix[2][0];
		tempMatrix[3][0] = matrix[3][0];

		tempMatrix[0][1] = matrix[0][1];
		tempMatrix[1][1] = matrix[1][1];
		tempMatrix[2][1] = matrix[2][1];
		tempMatrix[3][1] = matrix[3][1];

		tempMatrix[0][2] = matrix[0][2];
		tempMatrix[1][2] = matrix[1][2];
		tempMatrix[2][2] = matrix[2][2];
		tempMatrix[3][2] = matrix[3][2];

		tempMatrix[0][3] = 0;
		tempMatrix[1][3] = 0;
		tempMatrix[2][3] = 0;
		tempMatrix[3][3] = 1;

		//normalize the temp matrix
		float normalizer = 1/sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);

		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				tempMatrix[i][j] = tempMatrix[i][j] * normalizer;
			}
		}

		//Multiply the temp matrix with the top matrix on the stack and push it onto stack
		GzMatrix temp;
		GzMatrix result;

		//initialize matrix variables
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				temp[i][j] = render->Xnorm[render->matlevel-1][i][j];
				result[i][j] = 0;
			}
		}

		//4 X 4 matrix multiplication
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				for(int k = 0; k < 4; k++)
				{
					result[i][j] = result[i][j] + (temp[i][k] * tempMatrix[k][j]); 								
				}
			}
		}

		//transfer from result matrix to top of Xnorm matrix stack
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				render->Xnorm[render->matlevel][i][j] = result[i][j];
				result[i][j] = 0;
				temp[i][j] = 0;
			}
		}
	}
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
		switch(nameList[i])
		{
			case GZ_RGB_COLOR:
			{
				//Default flat shading
				GzColor* colorpointer = (GzColor*)(valueList[i]);			
				render->flatcolor[0] = colorpointer[0][RED];
				render->flatcolor[1] = colorpointer[0][GREEN];
				render->flatcolor[2] = colorpointer[0][BLUE];
				break;
			}
			case GZ_INTERPOLATE:
			{
				//Shading interpolation, phong or gourad
				int* interppointer = (int*)(valueList[i]);
				render->interp_mode = *interppointer;
				break;
			}
			case GZ_DIRECTIONAL_LIGHT:
			{
				//add a directional light to the render
				if(render->numlights < MAX_LIGHTS)
				{
					GzLight* light = (GzLight*)(valueList[i]);
					render->lights[render->numlights].color[RED] = light->color[RED];
					render->lights[render->numlights].color[GREEN] = light->color[GREEN];
					render->lights[render->numlights].color[BLUE] = light->color[BLUE];

					render->lights[render->numlights].direction[X] = light->direction[X];
					render->lights[render->numlights].direction[Y] = light->direction[Y];
					render->lights[render->numlights].direction[Z] = light->direction[Z];
			
					float normalizer = sqrt(render->lights[render->numlights].direction[X] * render->lights[render->numlights].direction[X] + render->lights[render->numlights].direction[Y] * render->lights[render->numlights].direction[Y] + render->lights[render->numlights].direction[Z] * render->lights[render->numlights].direction[Z]);
				
					for(int i = 0; i < 3; i++)
					{
						render->lights[render->numlights].direction[i] = (render->lights[render->numlights].direction[i])/normalizer;
					}
					render->numlights++;
				}
				break;
			}
			case GZ_AMBIENT_LIGHT:
			{
				//set ambient light
				GzLight* ambient = (GzLight*)(valueList[i]);

				render->ambientlight.color[RED] = ambient->color[RED];
				render->ambientlight.color[GREEN] = ambient->color[GREEN];
				render->ambientlight.color[BLUE] = ambient->color[BLUE];			
				break;
			}			
			case GZ_DISTRIBUTION_COEFFICIENT:
			{
				//specular power
				float* specular = (float*)(valueList[i]);
				render->spec = *specular;
				break;
			}
			case GZ_AMBIENT_COEFFICIENT:
			{
				//ambient reflection coefficient
				GzColor* ka = (GzColor*)(valueList[i]);

				render->Ka[RED] = ka[0][RED];
				render->Ka[GREEN] = ka[0][GREEN];
				render->Ka[BLUE] = ka[0][BLUE];
				break;
			}
			case GZ_DIFFUSE_COEFFICIENT:
			{
				//diffuse reflection coefficient
				GzColor* kd = (GzColor*)(valueList[i]);
		
				render->Kd[RED] = kd[0][RED];
				render->Kd[GREEN] = kd[0][GREEN];
				render->Kd[BLUE] = kd[0][BLUE];
				break;
			}
			case GZ_SPECULAR_COEFFICIENT:
			{
				//specular reflection coefficient
				GzColor* ks = (GzColor*)(valueList[i]);

				render->Ks[RED] = ks[0][RED];
				render->Ks[GREEN] = ks[0][GREEN];
				render->Ks[BLUE] = ks[0][BLUE];
				break;
			}
			
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
	{
		return GZ_FAILURE;
	}
	
	//Object space vertex and normal
	GzCoord *objectVertex = 0;
	GzCoord *objectNormal = 0;

	//Image space vertex and normal
	GzCoord *imageVertex = (GzCoord*)calloc(3,sizeof(GzCoord));
	GzCoord *imageNormal = (GzCoord*)calloc(3,sizeof(GzCoord));

	GzCoord *screenVertex = (GzCoord*)calloc(3,sizeof(GzCoord));	

	for(int i = 0; i < numParts; i++)
	{
		if(nameList[i] == GZ_NULL_TOKEN)
		{
			break;		
		}
		else if(nameList[i] == GZ_POSITION)
		{
			objectVertex = (GzCoord*)(valueList[i]);						

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
				for(int row = 0; row < 4; row++)
				{
					for(int col = 0; col < 4; col++)
					{
						temp[row] +=  topMatrix[0][row][col] * homoGzCoord[col];
					}
				}

				//Convert world vertex from homogenous coordinates to normal GzCoordinate by normalizing it
				for(int w = 0; w < 3; w++)
				{
					screenVertex[index][w] = temp[w] / temp[3];					
				}			
			}
			
			//checking if triangle has vertices behind view plane
			if(screenVertex[0][Z] < 0 || screenVertex[1][Z] < 0 || screenVertex[2][Z] < 0)
			{
				break;
			}
			//check world triangle coordinates. If a vertex lies completely outside then skip it
			else if((screenVertex[0][X] < 0 && screenVertex[1][X] < 0 && screenVertex[2][X] < 0) || (screenVertex[0][X] > render->display->xres && screenVertex[1][X] > render->display->xres && screenVertex[2][X] > render->display->xres)
			|| (screenVertex[0][Y] < 0 && screenVertex[1][Y] < 0 && screenVertex[2][Y] < 0) || (screenVertex[0][Y] > render->display->yres && screenVertex[1][Y] > render->display->yres && screenVertex[2][Y] > render->display->yres))
			{
				break;
			}
		}
		else if(nameList[i] == GZ_NORMAL)
		{
			objectNormal = (GzCoord*)(valueList[i]);
		}
		else if(nameList[i] == GZ_TEXTURE_INDEX)
		{
			break;
		}

	}
	
	if(objectVertex == nullptr || objectNormal == nullptr)
	{
		return GZ_FAILURE;
	}

	for(int index = 0; index < 3; index++)
	{
		//transforming object space vertex and normals into image space vertex and normals
		GzMatrix* topMatrix = (GzMatrix*)render->Xnorm[render->matlevel]; 		

		//perform homogenous matrix multiplication of objectVertex with Ximage matrix
		float temp1[4];
		float temp2[4];
		float homoGzCoord1[4];
		float homoGzCoord2[4];

		for(int u = 0; u < 3; u++)
		{
			homoGzCoord1[u] = objectVertex[index][u];					
			homoGzCoord2[u] = objectNormal[index][u];
		}
		
		homoGzCoord1[3] = 1;
		homoGzCoord2[3] = 1;

		for(int v = 0; v < 4; v++)
		{
			temp1[v] = 0;
			temp2[v] = 0;
		}
	
		/* Transforming the object vertex to world vertex by multiplying with top Ximage matrix on stack */
		for(int row = 0; row < 4; row++)
		{
			for(int col = 0; col < 4; col++)
			{
				temp1[row] +=  topMatrix[0][row][col] * homoGzCoord1[col];
				temp2[row] +=  topMatrix[0][row][col] * homoGzCoord2[col];
			}
		}

		//Convert world vertex from homogenous coordinates to normal GzCoordinate by normalizing it
		for(int w = 0; w < 3; w++)
		{
			imageVertex[index][w] = temp1[w] / temp1[3];
			imageNormal[index][w] = temp2[w] / temp2[3];
		}			
	}
			
	//Rasterize the triangle by passing its image vertices, image normal and screen vertices
	GzRasterize(render,imageVertex,imageNormal,screenVertex);
		 
	return GZ_SUCCESS;
}


/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void GzRasterize(GzRender *render,GzCoord *imageV, GzCoord *imageN,GzCoord *screenV)
{
	
	float xMin,xMax,yMin,yMax;
	float a1,b1,c1,a2,b2,c2,a3,b3,c3;
	float aX,bY,cZ,d;
	GzCoord v0,v1,v2;
	GzEdge edge[3];
	
	GzCoord tempCoord1, tempCoord2;

	// gourad shading values
	GzColor cpA,cpB,cpC,cpD;
	
	// phong shading values
	GzCoord imageVa, imageVb, imageVc, imageVd; //image vertices
	GzCoord imageNa, imageNb, imageNc, imageNd; //image normals

	
	//Find xmin,xmax,ymin and ymax to obtain bounding rectangle for the triangle
	xMin = min(screenV[0][X],min(screenV[1][X],screenV[2][X]));
	yMin = min(screenV[0][Y],min(screenV[1][Y],screenV[2][Y]));

	xMax = max(screenV[0][X],max(screenV[1][X],screenV[2][X]));
	yMax = max(screenV[0][Y],max(screenV[1][Y],screenV[2][Y]));

	xMin = max((int)(ceil(xMin)),0);
	yMin = max((int)(ceil(yMin)),0);

	xMax = min((int)(floor(xMax)),render->display->xres);
	yMax = min((int)(floor(yMax)),render->display->yres);

	
	// Setting vertices of the triangle and the edges of the triangle in anti-clockwise direction for processing

	v0[X] = screenV[0][X];
	v0[Y] = screenV[0][Y];
	v0[Z] = screenV[0][Z];

	v1[X] = screenV[1][X];
	v1[Y] = screenV[1][Y];
	v1[Z] = screenV[1][Z];

	v2[X] = screenV[2][X];
	v2[Y] = screenV[2][Y];
	v2[Z] = screenV[2][Z];

	//edge 0 v0 -> v2
	edge[0].start[X] = v0[X];
	edge[0].start[Y] = v0[Y];
	edge[0].start[Z] = v0[Z];
		
	edge[0].end[X] = v2[X];
	edge[0].end[Y] = v2[Y];
	edge[0].end[Z] = v2[Z];
	
	//edge 1 v2 -> v1
	edge[1].start[X] = v2[X];
	edge[1].start[Y] = v2[Y];
	edge[1].start[Z] = v2[Z];
	
	edge[1].end[X] = v1[X];
	edge[1].end[Y] = v1[Y];
	edge[1].end[Z] = v1[Z];
	
	//edge 2 v1 -> v0
	edge[2].start[X] = v1[X];
	edge[2].start[Y] = v1[Y];
	edge[2].start[Z] = v1[Z];
	
	edge[2].end[X] = v0[X];
	edge[2].end[Y] = v0[Y];
	edge[2].end[Z] = v0[Z];
	
	/* Computing Line Equation coefficients */

	a1 = edge[0].end[Y] - edge[0].start[Y];
	b1 = edge[0].start[X] - edge[0].end[X];
	c1 = (((edge[0].end[X] - edge[0].start[X]) * edge[0].start[Y]) - ((edge[0].end[Y] - edge[0].start[Y]) * edge[0].start[X]));

	a2 = edge[1].end[Y] - edge[1].start[Y];
	b2 = edge[1].start[X] - edge[1].end[X];
	c2 = (((edge[1].end[X] - edge[1].start[X]) * edge[1].start[Y]) - ((edge[1].end[Y] - edge[1].start[Y]) * edge[1].start[X]));

	a3 = edge[2].end[Y] - edge[2].start[Y];
	b3 = edge[2].start[X] - edge[2].end[X];
	c3 = (((edge[2].end[X] - edge[2].start[X]) * edge[2].start[Y]) - ((edge[2].end[Y] - edge[2].start[Y]) * edge[2].start[X]));
		
	/*	Finding Crossproduct of 2 edges to obtain the plane equation for the triangle	*/
	aX = (((edge[0].end[Y] - edge[0].start[Y]) * (edge[1].end[Z] - edge[1].start[Z])) - ((edge[0].end[Z] - edge[0].start[Z]) * (edge[1].end[Y] - edge[1].start[Y])));
	bY = -(((edge[0].end[X] - edge[0].start[X]) * (edge[1].end[Z] - edge[1].start[Z])) - ((edge[0].end[Z] - edge[0].start[Z]) * (edge[1].end[X] - edge[1].start[X])));
	cZ = (((edge[0].end[X] - edge[0].start[X]) * (edge[1].end[Y] - edge[1].start[Y])) - ((edge[0].end[Y] - edge[0].start[Y]) * (edge[1].end[X] - edge[1].start[X])));
	d = -(aX * screenV[0][X] + bY * screenV[0][Y] + cZ * screenV[0][Z]);
		
	/*	setting values for pixels which will be constant through the interpolation calculation	*/
	tempCoord1[X] = edge[0].end[X] - edge[0].start[X];
	tempCoord2[X] = edge[1].end[X] - edge[1].start[X];
	
	tempCoord1[Y] = edge[0].end[Y] - edge[0].start[Y];
	tempCoord2[Y] = edge[1].end[Y] - edge[1].start[Y];

	//For Gourad Shading
	if(render->interp_mode == GZ_COLOR)
	{	
		//calculate the color for each vertex and set up the interpolation plane coefficients
		GzColor vertexClr[3];
			
		GzShade(render,imageV[0],imageN[0],vertexClr[0]);
		GzShade(render,imageV[1],imageN[1],vertexClr[1]);
		GzShade(render,imageV[2],imageN[2],vertexClr[2]);		

		tempCoord1[Z] = vertexClr[2][0] - vertexClr[0][0];
		tempCoord2[Z] = vertexClr[1][0] - vertexClr[2][0];
												
		cpA[0] = tempCoord1[Y] * tempCoord2[Z] - tempCoord1[Z] * tempCoord2[Y];
		cpB[0] = tempCoord1[Z] * tempCoord2[X] - tempCoord1[X] * tempCoord2[Z];
		cpC[0] = tempCoord1[X] * tempCoord2[Y] - tempCoord1[Y] * tempCoord2[X];
		cpD[0] = -(cpA[0] * edge[0].start[X] + cpB[0] * edge[0].start[Y] + cpC[0] * vertexClr[0][0]);

		tempCoord1[Z] = 0;
		tempCoord2[Z] = 0;

		tempCoord1[Z] = vertexClr[2][1] - vertexClr[0][1];
		tempCoord2[Z] = vertexClr[1][1] - vertexClr[2][1];
												
		cpA[1] = tempCoord1[Y] * tempCoord2[Z] - tempCoord1[Z] * tempCoord2[Y];
		cpB[1] = tempCoord1[Z] * tempCoord2[X] - tempCoord1[X] * tempCoord2[Z];
		cpC[1] = tempCoord1[X] * tempCoord2[Y] - tempCoord1[Y] * tempCoord2[X];
		cpD[1] = -(cpA[1] * edge[0].start[X] + cpB[1] * edge[0].start[Y] + cpC[1] * vertexClr[0][1]);

		tempCoord1[Z] = 0;
		tempCoord2[Z] = 0;

		tempCoord1[Z] = vertexClr[2][2] - vertexClr[0][2];
		tempCoord2[Z] = vertexClr[1][2] - vertexClr[2][2];
												
		cpA[2] = tempCoord1[Y] * tempCoord2[Z] - tempCoord1[Z] * tempCoord2[Y];
		cpB[2] = tempCoord1[Z] * tempCoord2[X] - tempCoord1[X] * tempCoord2[Z];
		cpC[2] = tempCoord1[X] * tempCoord2[Y] - tempCoord1[Y] * tempCoord2[X];
		cpD[2] = -(cpA[2] * edge[0].start[X] + cpB[2] * edge[0].start[Y] + cpC[2] * vertexClr[0][2]);

		tempCoord1[Z] = 0;
		tempCoord2[Z] = 0;
			
	}
	
	/*	For gourad shading  */
	else if(render->interp_mode == GZ_NORMALS)
	{	
		/* calculate interpolation plane coefficient for planes and normals	*/
		for(int i = 0; i < 3; i++)
		{
			/* For vertices	*/
			tempCoord1[Z] = imageV[2][i] - imageV[0][i];
			tempCoord2[Z] = imageV[1][i] - imageV[2][i];						
					
			imageVa[i] = tempCoord1[Y] * tempCoord2[Z] - tempCoord1[Z] * tempCoord2[Y];
			imageVb[i] = tempCoord1[Z] * tempCoord2[X] - tempCoord1[X] * tempCoord2[Z];
			imageVc[i] = tempCoord1[X] * tempCoord2[Y] - tempCoord1[Y] * tempCoord2[X];
			imageVd[i] = -(imageVa[i] * edge[0].start[X] + imageVb[i] * edge[0].start[Y] + imageVc[i] * imageV[0][i]);

			/* For normals	*/
			tempCoord1[Z] = imageN[2][i] - imageN[0][i];
			tempCoord2[Z] = imageN[1][i] - imageN[2][i];
		
			imageNa[i] = tempCoord1[Y] * tempCoord2[Z] - tempCoord1[Z] * tempCoord2[Y];
			imageNb[i] = tempCoord1[Z] * tempCoord2[X] - tempCoord1[X] * tempCoord2[Z];
			imageNc[i] = tempCoord1[X] * tempCoord2[Y] - tempCoord1[Y] * tempCoord2[X];
			imageNd[i] = -(imageNa[i] * edge[0].start[X] + imageNb[i] * edge[0].start[Y] + imageNc[i] * imageN[0][i]);
		}				
	}
	

	for( int y = yMin; y <= yMax; y++ )
	{
		for( int x = xMin; x <= xMax; x++ )
		{			
			if((a1 * x + b1 * y + c1 > 0 && a2 * x + b2 * y + c2 > 0 && a3 * x + b3 * y + c3 > 0) || ((a1 * x + b1 * y + c1) <= 0 && (a2 * x + b2 * y + c2) <= 0 && (a3 * x + b3 * y + c3) <= 0))
			{
				GzDepth newZ = (GzDepth)(-( aX * x + bY * y  + d ) / cZ);

				// Check if the new value of z is in front of the camera
				if( newZ > 0 )
				{	
					GzIntensity r, g, b, a;
					GzDepth oldZ;
			
					GzGetDisplay( render->display, x, y, &r, &g, &b, &a, &oldZ);			

					//If the value of new z id greater than old z then it can be skipped because it will be hidden
					if( newZ < oldZ )
					{
						GzColor color;						
						
						/* For Phong shading	*/
						if(render->interp_mode == GZ_NORMALS) 
						{
							/* Perform biliniear interpolation, normalize it and calculate the color at the current pixel by calling GzShade*/
							GzCoord imageVi, imageNi;
							for(int i = 0; i < 3; i++)
							{
								imageVi[i] = -(imageVa[i] * x + imageVb[i] * y + imageVd[i]) / imageVc[i];							                               						                                   
								imageNi[i] = -(imageNa[i] * x + imageNb[i] * y  + imageNd[i]) / imageNc[i];							                                 						                                     
							}							
						
							float normalizer = sqrt(imageNi[X] * imageNi[X] + imageNi[Y] * imageNi[Y] + imageNi[Z] * imageNi[Z]);
							imageNi[X] = imageNi[X]/normalizer;
							imageNi[Y] = imageNi[Y]/normalizer;
							imageNi[Z] = imageNi[Z]/normalizer;
								
							GzShade(render,imageVi,imageNi,color);								
						}
						else if(render->interp_mode == GZ_COLOR)							
						{
							/*	Interpolate the colors for Gourad Shading at each vertex	*/
							color[RED] = -(cpA[0] * x + cpB[0] * y + cpD[0])/cpC[0];
							color[GREEN] = -(cpA[1] * x + cpB[1] * y  + cpD[1])/cpC[1];
							color[BLUE] = -(cpA[2] * x + cpB[2] * y  + cpD[2])/cpC[2];														
						}					
						else
						{
							/*	Default flat shading	*/
							color[RED] = render->flatcolor[RED];
							color[GREEN] = render->flatcolor[GREEN];
							color[BLUE] = render->flatcolor[BLUE];							
						}						
						/* Put the calculated values of the color onto the display	*/
						GzPutDisplay(render->display, x, y, ctoi(color[RED]),ctoi(color[GREEN]),ctoi(color[BLUE]),a,newZ);								  								 								  								  				
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

/* Shading procedure	*/
void GzShade(GzRender *render,const GzCoord imageV,const GzCoord imageN,GzColor color)
{
	GzColor Ks, Kd, Ka;
	int i,index;

	/* The color is calculated by the following formula
		Color = (Ks * sigma[lightIntensity(R.E)^s]) + (Kd * sigma[lightIntensity(N.L)]) + (Ka * Ia) */
	
	/*	Initialize the values of Ks and Kd to be 0	*/
	for(i = 0; i < 3; i++)
	{
		Ks[i] = 0;
		Kd[i] = 0;
	}
	
	/* Iterate over the number of lights present to calculate Ks and Kd	*/
	for(index = 0; index < render->numlights; index++)
	{
		
		GzCoord eye;
		GzCoord reflRay;
		GzCoord imageNormal;
		float NdotL,NdotE,RdotE;
		float normalizer;
	
		/* Setting eye direction as (0,0,-1) in image space */
		eye[X] = 0;
		eye[Y] = 0;
		eye[Z] = -1;
	
		/* calculate N.L and N.E	*/
		NdotE = imageN[X] * eye[X] + imageN[Y] * eye[Y] + imageN[Z] * eye[Z];		
		NdotL = imageN[X] * render->lights[index].direction[X] + imageN[Y] * render->lights[index].direction[Y] + imageN[Z] * render->lights[index].direction[Z];		
		
		//if both N.L and N.E are +ve then compute 
		if(NdotL > 0 && NdotE > 0)
		{
			for(i=0; i < 3; i++)
			{
				imageNormal[i] = imageN[i];
			}
		}
		//if both N.L and N.E are -ve then invert the normal and compute
		else if(NdotL < 0 && NdotE < 0)
		{			
			for(i=0; i<3; i++)
			{
				imageNormal[i] = -imageN[i];
			}						
			NdotL = -NdotL;
			NdotE = -NdotE;
		}
		//else if both N.L and N.E are (+ve,-ve) or (-ve,+ve) skip
		else if((NdotL < 0 && NdotE > 0)||(NdotL > 0 && NdotE < 0)) 
		{
			continue;
		}
		
		/* Reflected ray is calculated as R = 2(N.L)N - L
			Normalize the reflected ray	*/
		for(i = 0; i < 3; i++)
		{			
			reflRay[i] = ((2 * NdotL * imageNormal[i]) - (render->lights[index].direction[i]));			
		}

		normalizer = sqrt(reflRay[X] * reflRay[X] + reflRay[Y] * reflRay[Y] + reflRay[Z] * reflRay[Z]);
		for(i = 0; i < 3; i++)
		{
			reflRay[i] = reflRay[i] / normalizer;			
		}
				
		/* calculate R.E and make sure it's a positive value	*/

		RdotE = reflRay[X] * eye[X] + reflRay[Y] * eye[Y] + reflRay[Z] * eye[Z];
		
		if(RdotE < 0)
		{
			RdotE = 0;
		}

		/* scale and add the components of Ks and Kd with R.E ^ s and N.L respectively 	*/
		for(i = 0; i < 3; i++)
		{
			Ks[i] = Ks[i] + render->lights[index].color[i] * pow(RdotE,render->spec);
			Kd[i] = Kd[i] + render->lights[index].color[i] * NdotL;
		}
	}

	/* sum up the values of Ks, Kd and Ka to get the color value
		Color = (Ks * sigma[lightIntensity(R.E)^s]) + (Kd * sigma[lightIntensity(N.L)]) + (Ka * Ia) */
	for(i = 0; i < 3; i++)
	{
		Ks[i] = Ks[i] * render->Ks[i];
		Kd[i] = Kd[i] * render->Kd[i];
		Ka[i] = render->ambientlight.color[i] * render->Ka[i];
		color[i] = Ks[i] + Kd[i] + Ka[i];
	}
	
}

