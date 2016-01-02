// SimpleViewer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "StdAfx.h"
#include <OpenNI.h>

#ifndef _CRT_SECURE_NO_DEPRECATE 
	#define _CRT_SECURE_NO_DEPRECATE 1
#endif


#include "GL/glut.h"
#include "GL/gl.h"
#include "GL/glext.h"
#include <stdlib.h>  
#include <iostream>  
#include <string> 
#include "OniSampleUtilities.h"

#include "OpenNI.h"  
#include "opencv2/core/core.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  




#define SIGN(x) ( (x)<0 ? -1:((x)>0?1:0 ) )
 
using namespace std;  
using namespace cv;  
using namespace openni;  



float xyzdata[480][640][3];
float texture[480][640][3];
int glWinWidth = 640, glWinHeight = 480;
int width=640, height=480;
double eyex, eyey, eyez, atx, aty, atz;  // eye* - �����λ�ã�at* - ע�ӵ�λ��
 
bool leftClickHold = false, rightClickHold = false;
int mx,my; 			// ��갴��ʱ�� OpenGL ���ڵ�����
int ry = 90, rx = 90;    // ��������ע�ӵ�Ĺ۲�Ƕ�
double mindepth, maxdepth;		// ������ݵļ�ֵ 
double radius = 6000.0;		// �������ע�ӵ�ľ���



typedef struct  
{  
    //unsigned short    bfType;  
    unsigned long    bfSize;  
    unsigned short    bfReserved1;  
    unsigned short    bfReserved2;  
    unsigned long    bfOffBits;  
} ClBitMapFileHeader;  
  
typedef struct  
{  
    unsigned long  biSize;   
    long   biWidth;   
    long   biHeight;   
    unsigned short   biPlanes;   
    unsigned short   biBitCount;  
    unsigned long  biCompression;   
    unsigned long  biSizeImage;   
    long   biXPelsPerMeter;   
    long   biYPelsPerMeter;   
    unsigned long   biClrUsed;   
    unsigned long   biClrImportant;   
} ClBitMapInfoHeader;  
  
typedef struct   
{  
    unsigned char rgbBlue; //����ɫ����ɫ����  
    unsigned char rgbGreen; //����ɫ����ɫ����  
    unsigned char rgbRed; //����ɫ�ĺ�ɫ����  
    unsigned char rgbReserved; //����ֵ  
} ClRgbQuad;  
  
typedef struct  
{  
    int width;  
    int height;  
    int channels;  
    unsigned char* imageData;  
}ClImage;  
  

ClImage* clLoadImage(char* path)  
{  
    ClImage* bmpImg;  
    FILE* pFile;  
    unsigned short fileType;  
    ClBitMapFileHeader bmpFileHeader;  
    ClBitMapInfoHeader bmpInfoHeader;  
    int channels = 1;  
    int width = 0;  
    int height = 0;  
    int step = 0;  
    int offset = 0;  
    unsigned char pixVal;  
    ClRgbQuad* quad;  
    int i, j, k;  
  
    bmpImg = (ClImage*)malloc(sizeof(ClImage));  
    pFile = fopen(path, "rb");  
    if (!pFile)  
    {  
        free(bmpImg);  
        return NULL;  
    }  
  
    fread(&fileType, sizeof(unsigned short), 1, pFile);  
    if (fileType == 0x4D42)  
    {  
        //printf("bmp file! \n");  
  
        fread(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);  
        /*printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"); 
        printf("bmp�ļ�ͷ��Ϣ��\n"); 
        printf("�ļ���С��%d \n", bmpFileHeader.bfSize); 
        printf("�����֣�%d \n", bmpFileHeader.bfReserved1); 
        printf("�����֣�%d \n", bmpFileHeader.bfReserved2); 
        printf("λͼ����ƫ���ֽ�����%d \n", bmpFileHeader.bfOffBits);*/  
  
        fread(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);  
        /*printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"); 
        printf("bmp�ļ���Ϣͷ\n"); 
        printf("�ṹ�峤�ȣ�%d \n", bmpInfoHeader.biSize); 
        printf("λͼ��ȣ�%d \n", bmpInfoHeader.biWidth); 
        printf("λͼ�߶ȣ�%d \n", bmpInfoHeader.biHeight); 
        printf("λͼƽ������%d \n", bmpInfoHeader.biPlanes); 
        printf("��ɫλ����%d \n", bmpInfoHeader.biBitCount); 
        printf("ѹ����ʽ��%d \n", bmpInfoHeader.biCompression); 
        printf("ʵ��λͼ����ռ�õ��ֽ�����%d \n", bmpInfoHeader.biSizeImage); 
        printf("X����ֱ��ʣ�%d \n", bmpInfoHeader.biXPelsPerMeter); 
        printf("Y����ֱ��ʣ�%d \n", bmpInfoHeader.biYPelsPerMeter); 
        printf("ʹ�õ���ɫ����%d \n", bmpInfoHeader.biClrUsed); 
        printf("��Ҫ��ɫ����%d \n", bmpInfoHeader.biClrImportant); 
        printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");*/  
  
        if (bmpInfoHeader.biBitCount == 8)  
        {  
            //printf("���ļ��е�ɫ�壬����λͼΪ�����ɫ\n\n");  
            channels = 1;  
            width = bmpInfoHeader.biWidth;  
            height = bmpInfoHeader.biHeight;  
            offset = (channels*width)%4;  
            if (offset != 0)  
            {  
                offset = 4 - offset;  
            }  
            //bmpImg->mat = kzCreateMat(height, width, 1, 0);  
            bmpImg->width = width;  
            bmpImg->height = height;  
            bmpImg->channels = 1;  
            bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*height);  
            step = channels*width;  
  
            quad = (ClRgbQuad*)malloc(sizeof(ClRgbQuad)*256);  
            fread(quad, sizeof(ClRgbQuad), 256, pFile);  
            free(quad);  
  
            for (i=0; i<height; i++)  
            {  
                for (j=0; j<width; j++)  
                {  
                    fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                    bmpImg->imageData[(height-1-i)*step+j] = pixVal;  
                }  
                if (offset != 0)  
                {  
                    for (j=0; j<offset; j++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                    }  
                }  
            }             
        }  
        else if (bmpInfoHeader.biBitCount == 24)  
        {  
            //printf("��λͼΪλ���ɫ\n\n");  
            channels = 3;  
            width = bmpInfoHeader.biWidth;  
            height = bmpInfoHeader.biHeight;  
  
            bmpImg->width = width;  
            bmpImg->height = height;  
            bmpImg->channels = 3;  
            bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*3*height);  
            step = channels*width;  
  
            offset = (channels*width)%4;  
            if (offset != 0)  
            {  
                offset = 4 - offset;  
            }  
  
            for (i=0; i<height; i++)  
            {  
                for (j=0; j<width; j++)  
                {  
                    for (k=0; k<3; k++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                        bmpImg->imageData[(height-1-i)*step+j*3+k] = pixVal;  
                    }  
                    //kzSetMat(bmpImg->mat, height-1-i, j, kzScalar(pixVal[0], pixVal[1], pixVal[2]));  
                }  
                if (offset != 0)  
                {  
                    for (j=0; j<offset; j++)  
                    {  
                        fread(&pixVal, sizeof(unsigned char), 1, pFile);  
                    }  
                }  
            }  
        }  
    }  
  
    return bmpImg;  
}  
  



void mouse(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			leftClickHold=true;
		}
		else
		{
			leftClickHold=false;
		}
	}
 
	if (button== GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			rightClickHold=true;
		}
		else
		{
			rightClickHold=false;
		}
	}
}
 
//////////////////////////////////////////////////////////////////////////
// ����˶���Ӧ����
void motion(int x, int y)
{
	int rstep = 5; 
	if(leftClickHold==true)
	{
		if( abs(x-mx) > abs(y-my) )
		{
			rx += SIGN(x-mx)*rstep;    
		}
		else
		{
			ry -= SIGN(y-my)*rstep;    
		}
 
		mx=x;
		my=y;
		glutPostRedisplay();
	}
 
	if(rightClickHold==true)
	{
		radius += SIGN(y-my)*100.0;
		radius = std::max( radius, 100.0 );
		mx=x;
		my=y;
		glutPostRedisplay();
	}
}
 
//////////////////////////////////////////////////////////////////////////
// ��άͼ����ʾ��Ӧ����
void renderScene(void) 
{
	// clear screen and depth buffer
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// Reset the coordinate system before modifying 
	glLoadIdentity();	
	// set the camera position
	atx = 0.0f;
	aty = 0.0f;
	atz = ( mindepth - maxdepth ) / 2.0f;
	eyex = atx + radius * sin( CV_PI * ry / 180.0f ) * cos( CV_PI * rx/ 180.0f ); 
	eyey = aty + radius * cos( CV_PI * ry/ 180.0f ); 
	eyez = atz + radius * sin( CV_PI * ry / 180.0f ) * sin( CV_PI * rx/ 180.0f );
	gluLookAt (eyex, eyey, eyez, atx, aty, atz, 0.0, 1.0, 0.0);
	glRotatef(0,0,1,0);
	glRotatef(-180,1,0,0);
 
	// �Ե������ݽ������ǻ�
	// �ο��ԣ�http://www.codeproject.com/KB/openGL/OPENGLTG.aspx
	// we are going to loop through all of our terrain's data points,
	// but we only want to draw one triangle strip for each set along the x-axis.
	for (int i = 0; i < height; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < width; j++)
		{
			// for each vertex, we calculate the vertex color, 
			// we set the texture coordinate, and we draw the vertex.
			/*
			   the vertexes are drawn in this order:
 
			   0  ---> 1
			          /
				    /
			      /
			   2  ---> 3
			*/
 
			// draw vertex 0
			glTexCoord2f(0.0f, 0.0f);
			glColor3f(texture[i][j][0]/255.0f, texture[i][j][1]/255.0f, texture[i][j][2]/255.0f);
			glVertex3f(xyzdata[i][j][0], xyzdata[i][j][1], xyzdata[i][j][2]);
 
			// draw vertex 1
			glTexCoord2f(1.0f, 0.0f);
			glColor3f(texture[i+1][j][0]/255.0f, texture[i+1][j][1]/255.0f, texture[i+1][j][2]/255.0f);
			glVertex3f(xyzdata[i+1][j][0], xyzdata[i+1][j][1], xyzdata[i+1][j][2]);
 
			// draw vertex 2
			glTexCoord2f(0.0f, 1.0f);
			glColor3f(texture[i][j+1][0]/255.0f, texture[i][j+1][1]/255.0f, texture[i][j+1][2]/255.0f);
			glVertex3f(xyzdata[i][j+1][0], xyzdata[i][j+1][1], xyzdata[i][j+1][2]);
 
			// draw vertex 3
			glTexCoord2f(1.0f, 1.0f);
			glColor3f(texture[i+1][j+1][0]/255.0f, texture[i+1][j+1][1]/255.0f, texture[i+1][j+1][2]/255.0f);
			glVertex3f(xyzdata[i+1][j+1][0], xyzdata[i+1][j+1][1], xyzdata[i+1][j+1][2]);
		}
		glEnd();
	}
	// enable blending
	glEnable(GL_BLEND);
 
	// enable read-only depth buffer
	glDepthMask(GL_FALSE);
 
	// set the blend function to what we use for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 
	// set back to normal depth buffer mode (writable)
	glDepthMask(GL_TRUE);
 
	// disable blending
	glDisable(GL_BLEND);
 
/* 	float x,y,z;
	// ����ͼ�����
	glPointSize(1.0); 
	glBegin(GL_POINTS);
	for (int i=0;i<height;i++){ 
		for (int j=0;j<width;j++){
			// color interpolation 
			glColor3f(texture[i][j][0]/255, texture[i][j][1]/255, texture[i][j][2]/255);
			x= xyzdata[i][j][0];
			y= xyzdata[i][j][1]; 
			z= xyzdata[i][j][2]; 
			glVertex3f(x,y,z); 
		}
	}
	glEnd(); */
 
	glFlush();
	glutSwapBuffers();
}
 
//////////////////////////////////////////////////////////////////////////
// ���ڱ仯ͼ���ع���Ӧ����
void reshape (int w, int h) 
{
	glWinWidth = w;
	glWinHeight = h;
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45, (GLfloat)w / (GLfloat)h, 1.0, 15000.0);	
	glMatrixMode (GL_MODELVIEW);
}
 
//////////////////////////////////////////////////////////////////////////
// ������ά��������
void load3dDataToGL(IplImage* xyz)
{
	CvScalar s;
	//accessing the image pixels
	for (int i=0;i<height;i++)
	{ 
		for (int j=0;j<width;j++)
		{
			s=cvGet2D(xyz,i,j);			// s.val[0] = x, s.val[1] = y, s.val[2] = z
			xyzdata[i][j][0] = s.val[0];
			xyzdata[i][j][1] = s.val[1];
			xyzdata[i][j][2] = s.val[2];
		}
	} 
}
 
//////////////////////////////////////////////////////////////////////////
// ����ͼ����������
void loadTextureToGL(IplImage* img)
{	
	CvScalar ss;
	//accessing the image pixels
	for (int i=0;i<height;i++)
	{ 
		for (int j=0;j<width;j++)
		{
			ss=cvGet2D(img,i,j);			// ss.val[0] = red, ss.val[1] = green, ss.val[2] = blue
			texture[i][j][2] = ss.val[0];
			texture[i][j][1] = ss.val[1];
			texture[i][j][0] = ss.val[2];
		}
	} 	
}

int main( int argc, char** argv )
{
    // ��ʼ��OpenNI����
    openni::OpenNI::initialize();
	
		// ��������Device�豸�����õ���Kinect��
	openni::Device devAnyDevice;
	if(devAnyDevice.open( openni::ANY_DEVICE )==STATUS_OK)
	{

		// �����������������
		openni::VideoStream streamDepth;
		streamDepth.create( devAnyDevice, openni::SENSOR_DEPTH );
		streamDepth.start();

		// ͬ���Ĵ������򿪲�ɫͼ��������
		openni::VideoStream streamColor;
		streamColor.create( devAnyDevice, openni::SENSOR_COLOR );
		streamColor.start();

		// ѭ����ȡ��������Ϣ��������VideoFrameRef��
		openni::VideoFrameRef frameDepth;
		openni::VideoFrameRef frameColor;

		if( devAnyDevice.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )  
		{  
			devAnyDevice.setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR );  
		}  


		for( int i = 0; i < 10; ++ i )
		{
			// ��ȡ������
			streamDepth.readFrame( &frameDepth );
			streamColor.readFrame( &frameColor );

			// ��ȡdata array
			const openni::DepthPixel* pDepth
				= (const openni::DepthPixel*)frameDepth.getData();
			const openni::RGB888Pixel* pColor
				= (const openni::RGB888Pixel*)frameColor.getData();



			// ��ʾ�����Ϣ�Ͷ�Ӧ�Ĳ�ɫR��G��B��ֵ
			int idx = frameDepth.getWidth() * ( frameDepth.getHeight() + 1 ) / 2;
			std::cout  << pDepth[idx]  << "( "
				<< (int)pColor[idx].r << ","
				<< (int)pColor[idx].g << ","
				<< (int)pColor[idx].b << ")"
				<< std::endl;
		}

		// �ر�������
		streamDepth.destroy();
		streamColor.destroy();

		// �ر��豸
		devAnyDevice.close();

		// ���ر�OpenNI
		openni::OpenNI::shutdown();

		return 0;
	}
	else
	{
		int i,j;
		
		ClImage* rgb = clLoadImage("1.bmp");  
		ClImage* dep = clLoadImage("2.bmp");  
		
		
		for(j=0;j<480;j++){
			for(i=0;i<640;i++){
				if(dep->imageData[i+j*640]<255){
					xyzdata[j][i][0]=(float)i;
					xyzdata[j][i][1]=(float)j;
					xyzdata[j][i][2]=(float)(dep->imageData[i+j*640]);
					texture[j][i][0]=(float)(rgb->imageData[(i+j*640)*3]);
					texture[j][i][1]=(float)(rgb->imageData[(i+j*640)*3+1]);
					texture[j][i][2]=(float)(rgb->imageData[(i+j*640)*3+2]);
				}
			}
		}   
		 



			//------------------OpenGL-------------------------
		   glutInit(&argc, argv);
		   glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
		   glutInitWindowPosition(100,100);
		   glutInitWindowSize(500,500);
		   glutCreateWindow("Cloud");
		   glutDisplayFunc(renderScene);
		   glutMouseFunc(mouse);
		   glutMotionFunc(motion);
		   glutReshapeFunc (reshape);
		   glutMainLoop();

		   return 0; 
	
	}





}