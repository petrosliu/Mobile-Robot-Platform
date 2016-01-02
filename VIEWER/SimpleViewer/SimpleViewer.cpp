// SimpleViewer.cpp : 定义控制台应用程序的入口点。
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
double eyex, eyey, eyez, atx, aty, atz;  // eye* - 摄像机位置，at* - 注视点位置
 
bool leftClickHold = false, rightClickHold = false;
int mx,my; 			// 鼠标按键时在 OpenGL 窗口的坐标
int ry = 90, rx = 90;    // 摄像机相对注视点的观察角度
double mindepth, maxdepth;		// 深度数据的极值 
double radius = 6000.0;		// 摄像机与注视点的距离



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
    unsigned char rgbBlue; //该颜色的蓝色分量  
    unsigned char rgbGreen; //该颜色的绿色分量  
    unsigned char rgbRed; //该颜色的红色分量  
    unsigned char rgbReserved; //保留值  
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
        printf("bmp文件头信息：\n"); 
        printf("文件大小：%d \n", bmpFileHeader.bfSize); 
        printf("保留字：%d \n", bmpFileHeader.bfReserved1); 
        printf("保留字：%d \n", bmpFileHeader.bfReserved2); 
        printf("位图数据偏移字节数：%d \n", bmpFileHeader.bfOffBits);*/  
  
        fread(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);  
        /*printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n"); 
        printf("bmp文件信息头\n"); 
        printf("结构体长度：%d \n", bmpInfoHeader.biSize); 
        printf("位图宽度：%d \n", bmpInfoHeader.biWidth); 
        printf("位图高度：%d \n", bmpInfoHeader.biHeight); 
        printf("位图平面数：%d \n", bmpInfoHeader.biPlanes); 
        printf("颜色位数：%d \n", bmpInfoHeader.biBitCount); 
        printf("压缩方式：%d \n", bmpInfoHeader.biCompression); 
        printf("实际位图数据占用的字节数：%d \n", bmpInfoHeader.biSizeImage); 
        printf("X方向分辨率：%d \n", bmpInfoHeader.biXPelsPerMeter); 
        printf("Y方向分辨率：%d \n", bmpInfoHeader.biYPelsPerMeter); 
        printf("使用的颜色数：%d \n", bmpInfoHeader.biClrUsed); 
        printf("重要颜色数：%d \n", bmpInfoHeader.biClrImportant); 
        printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");*/  
  
        if (bmpInfoHeader.biBitCount == 8)  
        {  
            //printf("该文件有调色板，即该位图为非真彩色\n\n");  
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
            //printf("该位图为位真彩色\n\n");  
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
// 鼠标运动响应函数
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
// 三维图像显示响应函数
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
 
	// 对点云数据进行三角化
	// 参考自：http://www.codeproject.com/KB/openGL/OPENGLTG.aspx
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
	// 绘制图像点云
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
// 窗口变化图像重构响应函数
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
// 载入三维坐标数据
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
// 载入图像纹理数据
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
    // 初始化OpenNI环境
    openni::OpenNI::initialize();
	
		// 声明并打开Device设备，我用的是Kinect。
	openni::Device devAnyDevice;
	if(devAnyDevice.open( openni::ANY_DEVICE )==STATUS_OK)
	{

		// 创建并打开深度数据流
		openni::VideoStream streamDepth;
		streamDepth.create( devAnyDevice, openni::SENSOR_DEPTH );
		streamDepth.start();

		// 同样的创建并打开彩色图像数据流
		openni::VideoStream streamColor;
		streamColor.create( devAnyDevice, openni::SENSOR_COLOR );
		streamColor.start();

		// 循环读取数据流信息并保存在VideoFrameRef中
		openni::VideoFrameRef frameDepth;
		openni::VideoFrameRef frameColor;

		if( devAnyDevice.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )  
		{  
			devAnyDevice.setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR );  
		}  


		for( int i = 0; i < 10; ++ i )
		{
			// 读取数据流
			streamDepth.readFrame( &frameDepth );
			streamColor.readFrame( &frameColor );

			// 获取data array
			const openni::DepthPixel* pDepth
				= (const openni::DepthPixel*)frameDepth.getData();
			const openni::RGB888Pixel* pColor
				= (const openni::RGB888Pixel*)frameColor.getData();



			// 显示深度信息和对应的彩色R、G、B数值
			int idx = frameDepth.getWidth() * ( frameDepth.getHeight() + 1 ) / 2;
			std::cout  << pDepth[idx]  << "( "
				<< (int)pColor[idx].r << ","
				<< (int)pColor[idx].g << ","
				<< (int)pColor[idx].b << ")"
				<< std::endl;
		}

		// 关闭数据流
		streamDepth.destroy();
		streamColor.destroy();

		// 关闭设备
		devAnyDevice.close();

		// 最后关闭OpenNI
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