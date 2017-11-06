#pragma once

//includes for application

#ifdef	WIN32
#include <windows.h>
#endif

//opengl
#include <GL\glut.h>

//opencv
#include <opencv2/opencv.hpp>

//other includes
#include <iostream>
#include <io.h>
#include <fstream>
#include "BVH.h"

using namespace std;
using namespace cv;

//global settings
extern const string BVH_ROOT_PATH;

extern const unsigned int DISPLAYMODE;
extern const GLbitfield CLEAR_MASK;
extern const Color BACKGROUND_COLOR;
extern int win_width;
extern int win_height;
extern const int WINDOW_POS_X;
extern const int WINDOW_POS_Y;
extern const char* TITLE;
extern const double RANGE; //the range of clipping plane
extern const double Z_NEAR;
extern const double Z_FAR;
extern const float BONE_SCALE;
extern const double BONE_RADIUS;
extern const double HEAD_RADIUS;
extern const float ANIMATION_RATE;
extern const float ANIMATION_TIME_TOP;
extern const int LIST_COL_NUM;

extern const string SAVED_IMAGE_LIST_ROOT_PATH;

////for test
//extern const string SAVED_IMAGE_LIST_PATH;

//functions for opengl
void EnvironmentInit(void);
void Display(void);
void Reshape(int new_width, int new_height);

void Keyboard(unsigned char key, int x, int y);

void Idle(void);

//other functions
Mat SaveAsImage(void);
void SaveImageList(string path, const vector<Mat>& matList, int rowNum);
void LoadFilePathList(string rootPath, vector<string>& list);
void LoadNextBVH();
void PathConvert(const string& srcPath, const string& dstRoot, string& dstPath, const string& suffix);
void X_Rotation(float angle);
void Y_Rotation(float angle);
void Z_Rotation(float angle);
void Rot_Restore();

enum Rotation_Enum
{
	y_rot_0,
	y_rot_30, y_rot_60,
	y_rot_m30, y_rot_m60
};

//#define TEST_MODE
//#define NORMAL_ANIMATION_MODE