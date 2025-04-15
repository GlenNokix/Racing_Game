#include <GL/glut.h>
#include<math.h>
#include<stdio.h>
#include <memory.h>
#include<stdlib.h>
#include <string> 
#include<iostream>
#include <windows.h>
#include <gl/gl.h>
#include "GLAUX.H"


using namespace std;

#define BMP_Header_Length 54 
#define M_PI 3.1415926
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;   // Z 旋转量
float turn;
float moveX,smoveX,amoveX, smoveX1, amoveX1 =0;
float moveZ, smoveZ, amoveZ, smoveZ1, amoveZ1 = 0;
float v = 1,a=1;
float d[] = { 0.0,0.0,0.0 };//位移
int daxis = 2;//平移坐标轴
float angle;//旋转角
int aaxis = 2;//旋转坐标轴
GLsizei wh = 500, ww = 500;//初始窗口大小
char r[10];
char w[10];
int car_color = 1;
int lun_color = 0;
int kan = 0;
int x1 = 0, z1 = 0, x2 = 0, z2 = 0, x3 = 0, z3 = 0,x4 = 0, z4 = 0;
GLfloat colors[8][3] = { {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0},
	{1.0, 1.0, 1.0} };
GLfloat vertices[][3] = { {-1.0,-0.3,1.0},{1.0,-0.3,1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0},
	{-1.0,-0.3,-1.0},{1.0,-0.3,-1.0},{1.0,1.0,-1.0},{-1.0,1.0,-1.0} };

GLuint Textures[6];
GLint imageWidth = 0;
GLint imageHeight = 0;
GLuint texGround;
GLuint texSky1;
GLuint texSky2;
GLuint texSky3;
GLuint texSky4;
GLuint texSky5;
GLuint texSky6;
GLuint texRoad;
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// 打开文件，如果失败，返回    
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0) {
		printf("nm");
		return 0;
	}
	

	// 读取文件中图象的宽度和高度    
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// 计算每行像素所占字节数，并根据此数据计算总像素字节数    
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// 根据总像素字节数分配内存    
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// 读取像素数据    
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放    
	// 若图像宽高超过了OpenGL规定的最大值，也缩放    
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 1024;
				const GLint new_height = 1024; // 规定缩放后新的大小为边长的正方形    
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// 计算每行需要的字节数和总字节数    
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// 分配内存    
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// 进行像素缩放    
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height    
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// 分配一个新的纹理编号    
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// 绑定新的纹理，载入纹理并设置纹理参数    
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复    
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
	GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels); */

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定    
	free(pixels);
	return texture_ID;
}



void chelun()
{
	glColor3fv(colors[lun_color]);
	glTranslated(-20.0f, -15.0f, 15.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, -30.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(40.0f, 0.0f, 0.0f);
	glutSolidTorus(2, 5, 5, 100);
	glTranslated(0.0f, 0.0f, 30.0f);
	glutSolidTorus(2, 5, 5, 100);


}
void car()
{   
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, 0.0f, 15.0f);
	glVertex3f(-40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, -15.0f);
	glVertex3f(40.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-40.0f, -15.0f, 15.0f);
	glVertex3f(-40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, -15.0f);
	glVertex3f(40.0f, -15.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-10.0f, 10.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, 15.0f);
	glVertex3f(-20.0f, 0.0f, -15.0f);
	glVertex3f(-10.0f, 10.0f, -15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(20.0f, 10.0f, 15.0f);
	glVertex3f(20.0f, 10.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, -15.0f);
	glVertex3f(25.0f, 0.0f, 15.0f);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-30.0f, -15.0f, 15.0f);
	glVertex3f(-30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, -15.0f);
	glVertex3f(30.0f, -15.0f, 15.0f);
	glEnd();

	chelun();
}

void envirn()
{
    //公路
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0); // 清除屏幕和深度缓存
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-500.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-250.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-250.0, -20.0, -50.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-500.0, -20.0, -50.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-250.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0, -20.0, -50.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-250.0, -20.0, -50.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(250.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(250.0, -20.0, -50.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0, -20.0, -50.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(250.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(500.0, -20.0, 50.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(500.0, -20.0, -50.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(250.0, -20.0, -50.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-500.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-250.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-250.0, -20.0, 450.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-500.0, -20.0, 450.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-250.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0, -20.0, 450.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-250.0, -20.0, 450.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(250.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(250.0, -20.0, 450.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0, -20.0, 450.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texRoad);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(250.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(500.0, -20.0, 550.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(500.0, -20.0, 450.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(250.0, -20.0, 450.0);
	glEnd();
	//圆形
	glBegin(GL_LINE_STRIP);
	for (int i = 2000; i <= 6000; i++)
	{
		glVertex3f(200 * cos( 2*M_PI / 8000 * i)-500,-20.0, 200 * sin( 2*M_PI / 8000 * i)+250);
		glVertex3f(300 * cos(2*M_PI / 8000 * i) - 500, -20.0, 300 * sin(2*M_PI / 8000 * i) + 250);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= 2000; i++)
	{   
		glVertex3f(200 * cos(2 * M_PI / 8000 * i) + 500, -20.0, 200 * sin(2 * M_PI / 8000 * i) + 250);
		glVertex3f(300 * cos(2 * M_PI / 8000 * i) + 500, -20.0, 300 * sin(2 * M_PI / 8000 * i)+ 250);

	}		
	for (int i = 6000; i <8000; i++)
	{
		glVertex3f(200 * cos(2 * M_PI / 8000 * i) + 500, -20.0, 200 * sin(2 * M_PI / 8000 * i) + 250);
		glVertex3f(300 * cos(2 * M_PI / 8000 * i) + 500, -20.0, 300 * sin(2 * M_PI / 8000 * i) + 250);

	}
	glEnd();
	//路标
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 0.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glVertex3f(0.0, 20.0, -50.0);
	glVertex3f(0.0, 20.0, 0.0);
	glVertex3f(0.0, 40.0, 0.0);
	glVertex3f(0.0, 40.0, -50.0);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glVertex3f(0.0, -20.0, -20.0);
	glVertex3f(0.0, -20.0, -30.0);
	glVertex3f(0.0, 20.0, -30.0);
	glVertex3f(0.0, 20.0, -20.0);
	glEnd();
	//草地
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texGround);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-700.0, -20.0, 450.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(700.0, -20.0, 450.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(700.0, -20.0, 50.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-700.0, -20.0, 50.0);
	glEnd();

	glColor3f(0.2,0.2,1.0);//天空前
	glBindTexture(GL_TEXTURE_2D, texSky1);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(0.0f, 0.0f);
    glVertex3f(5000.0, -5000.0, -5000.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5000.0, -5000.0, 5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, 5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, -5000.0);
	glEnd();
    glColor3f(0.2,0.2,1.0);//天空后
	glBindTexture(GL_TEXTURE_2D, texSky2);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, -5000.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, 5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-5000.0, 5000.0, 5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-5000.0, 5000.0, -5000.0);
	glEnd();
	glColor3f(0.2,0.2,1.0);//天空左
	glBindTexture(GL_TEXTURE_2D, texSky3);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, -5000.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5000.0, -5000.0, -5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, -5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-5000.0, 5000.0, -5000.0);
	glEnd();
	glColor3f(0.2, 0.2, 1.0);//天空右
	glBindTexture(GL_TEXTURE_2D, texSky4);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(5000.0, -5000.0, 5000.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, 5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-5000.0, 5000.0, 5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, 5000.0);
	glEnd();
	glColor3f(0.2, 0.2, 1.0);//天空下
	glBindTexture(GL_TEXTURE_2D, texSky5);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, -5000.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-5000.0, -5000.0, 5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(5000.0, -5000.0, 5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(5000.0, -5000.0, -5000.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texSky6);//天空上
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5000.0, 5000.0, -5000.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-5000.0, 5000.0, 5000.0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, 5000.0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(5000.0, 5000.0, -5000.0);
	glEnd();
	
}



void myReshape(int w, int h)
{
	GLsizei p;
	p = w > h ? h : w;
	glViewport(0,0,p,p);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
	// 定义裁剪区域（根据窗口的纵横比，并使用正投影）
	if (w <= h) {
		glOrtho(-10000.0, 10000.0, -10000 *aspectRatio, 10000 * aspectRatio, 1000.0, -1000.0);
	}
	else {
		glOrtho(-10000.0 * aspectRatio, 10000.0 * aspectRatio, -10000.0, 10000.0, 1000.0, -1000.0);
	}

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Luncolor_menu(int index)
{
	lun_color = index;
	glutPostRedisplay();
}

void Carcolor_menu(int index)
{
	car_color = index;
	glutPostRedisplay();
}

void main_menu(int index)
{
	switch (index)
	{
	case(0): {
		exit(0);
		break;
	}

	}
}

void myKey(unsigned char key, int x, int y)
{
	if (key == 's' || key == 'S')
	{
		moveZ -= v * sin(angle * M_PI / 180.0);
		moveX -= v * cos(angle * M_PI / 180.0);

	}//前进
	if (key == 'w' || key == 'W')
	{
		moveZ += v * sin(angle * M_PI / 180.0);
		moveX += v * cos(angle * M_PI / 180.0);
	}//后退
	if (key == 'a' || key == 'A') {
		angle -= 2.0;x2++;
		if (x1 <= x2) {
			smoveX = -(1 - cos(angle * M_PI / 180.0)) * 23.0;
			smoveZ = 23.0 * sin(angle * M_PI / 180.0);
			amoveX = -(1 - cos(angle * M_PI / 180.0)) * 30.0;
			amoveZ = 30.0 * sin(angle * M_PI / 180.0);
			smoveX1 = -(1 - cos(angle * M_PI / 180.0)) * (-120.0);
			smoveZ1 = (-120.0) * sin(angle * M_PI / 180.0);
			amoveX1 = -(1 - cos(angle * M_PI / 180.0));
			amoveZ1 = sin(angle * M_PI / 180.0);

		}
		else {
			smoveX = -(1 - cos((angle - 2.0) * M_PI / 180.0)) * 23.0;
			smoveZ = 23.0 * sin((angle - 2.0) * M_PI / 180.0);
			amoveX = -(1 - cos((angle - 2.0) * M_PI / 180.0)) * 30.0;
			amoveZ = 30.0 * sin((angle - 2.0) * M_PI / 180.0);x1--;
			smoveX1 = -(1 - cos((angle - 2.0) * M_PI / 180.0)) * (-120.0);
			smoveZ1 = (-120.0) * sin((angle - 2.0) * M_PI / 180.0);
			amoveX1 = -(1 - cos((angle - 2.0) * M_PI / 180.0));
			amoveZ1 = sin((angle - 2.0) * M_PI / 180.0);
		}
		
	}//左转
	if (key == 'd' || key == 'D') {
		angle += 2.0;x1++;
		if (x1 >= x2) {
			smoveX = -(1 - cos(angle * M_PI / 180.0)) * 23.0;
			smoveZ = 23.0 * sin(angle * M_PI / 180.0);
			amoveX = -(1 - cos(angle * M_PI / 180.0)) * 30.0;
			amoveZ = 30.0 * sin(angle * M_PI / 180.0);
			smoveX1 = -(1 - cos(angle * M_PI / 180.0)) * (-120.0);
			smoveZ1 = (-120.0) * sin(angle * M_PI / 180.0);
			amoveX1 = -(1 - cos(angle * M_PI / 180.0)) ;
			amoveZ1 =  sin(angle * M_PI / 180.0);
		}
		else {
			smoveX = -(1 - cos((angle-2.0) * M_PI / 180.0)) * 23.0;
			smoveZ = 23.0 * sin((angle - 2.0) * M_PI / 180.0);
			amoveX = -(1 - cos((angle - 2.0) * M_PI / 180.0)) * 30.0;
			amoveZ = 30.0 * sin((angle - 2.0) * M_PI / 180.0);x2--;
			smoveX1 = -(1 - cos((angle - 2.0) * M_PI / 180.0)) * (-120.0);
			smoveZ1 = (-120.0) * sin((angle - 2.0) * M_PI / 180.0);
			amoveX1 = -(1 - cos((angle - 2.0) * M_PI / 180.0));
			amoveZ1 = sin((angle - 2.0) * M_PI / 180.0);
		}
		
	}//右转
	if (key == 'q' || key == 'Q')
	{
		if(v>0) v--;
	}//减速
	if (key == 'e' || key == 'E')
	{
		v++;
	}//加速
	if (key == 't' || key == 'T')
	{   
		kan++;
		kan = kan % 4;
	}//加速
	if (key == 'x' || key == 'X')
	{
		glEnable(GL_LIGHTING);   //开关:使用光
		glEnable(GL_LIGHT0);     //打开0#灯;
	}
	if (key == 'c' || key == 'C')
	{
		glDisable(GL_LIGHTING);   //开关:使用光
	}
	glutPostRedisplay();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1, 0.1, 50000);
	if (kan == 0) {
		gluLookAt(0.0, 2000.0, 250.0, 0.0, -20.0, 250.0, 1.0, 0.0, 0.0);
	}
	if(kan==1) {
		gluLookAt(-120.0 + moveX + smoveX1, 0.0, 0.0 + moveZ+smoveZ1, 0.0 + moveX + amoveX1, -20.0, 0.0 + moveZ+ amoveZ1, 0.0, 1.0, 0.0);
	}
	if (kan == 2) {
		gluLookAt(23.0+moveX+smoveX, 5.0, 0.0+ moveZ+smoveZ, 30.0+ moveX + amoveX, 5.0, 0.0+ moveZ +amoveZ ,0.0, 6.0, 0.0);
	}
	if (kan == 3) {
		gluLookAt(-120 + moveX , 20.0, 0.0 + moveZ , 0.0 + moveX , -20.0, 0.0 + moveZ , 0.0, 6.0, 0.0);
	}
	envirn();
	glTranslatef(moveX, 0, moveZ);
	glRotatef(-angle, 0, 1, 0);//绕Y轴旋转angle度
	
	glColor3fv(colors[car_color]);
	car();
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };  //镜面反射参数
	GLfloat mat_shininess[] = { 50000.0 };               //高光指数
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 0.0 };   //灯位置(1,1,1), 最后1-开关
	GLfloat Light_Model_Ambient[] = { 0.2, 0.2, 0.2, 0.0 }; //环境光参数
	//灯光设置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //散射光属性
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  //镜面反射光
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //环境光参数

	//glEnable(GL_LIGHTING);   //开关:使用光
	//glEnable(GL_LIGHT0);     //打开0#灯

	glFlush();
	glutSwapBuffers();


}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("赛车");

	glutDisplayFunc(display);

	glutReshapeFunc(myReshape);
	glEnable(GL_TEXTURE_2D);
	//加载纹理    
	texSky1 = load_texture("front.bmp");
	texSky2 = load_texture("back.bmp");
	texSky3 = load_texture("left.bmp");
	texSky4 = load_texture("right.bmp");
	texSky5 = load_texture("bottom.bmp");
	texSky6 = load_texture("top.bmp");
	texRoad = load_texture("road.bmp");
	texGround = load_texture("ground.bmp");
	//myinit();
	glutKeyboardFunc(myKey);
	glEnable(GL_DEPTH_TEST);

	

	int c_menu;
	c_menu = glutCreateMenu(Carcolor_menu);
	glutAddMenuEntry("black", 0);
	glutAddMenuEntry("red", 1);
	glutAddMenuEntry("green", 2);
	glutAddMenuEntry("blue", 3);
	glutAddMenuEntry("ching", 4);
	glutAddMenuEntry("carmine", 5);
	glutAddMenuEntry("yellow", 6);
	glutAddMenuEntry("white", 7);
	int l_menu;
	l_menu = glutCreateMenu(Luncolor_menu);
	glutAddMenuEntry("black", 0);
	glutAddMenuEntry("red", 1);
	glutAddMenuEntry("green", 2);
	glutAddMenuEntry("blue", 3);
	glutAddMenuEntry("ching", 4);
	glutAddMenuEntry("carmine", 5);
	glutAddMenuEntry("yellow", 6);
	glutAddMenuEntry("white", 7);
	glutCreateMenu(main_menu);
	glutAddSubMenu("wheel colours", l_menu);
	glutAddSubMenu("body colours", c_menu);
	glutAddMenuEntry("cancel", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}

