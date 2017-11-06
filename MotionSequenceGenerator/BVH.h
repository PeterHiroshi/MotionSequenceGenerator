#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <GL\glut.h>

using namespace std;

#define PI 3.1415926535898

struct BVH_Joint;

//channel enum
enum  Channel_Enum
{
	Xposition, Yposition, Zposition,
	Zrotation, Yrotation, Xrotation
};

//the structure of color
struct Color
{
	float r, g, b;
	float alpha;
};

//the structure of channel
struct Channel
{
	int channel_index;

	BVH_Joint* joint;
	Channel_Enum type;
};

//the basic structure of joint
struct BVH_Joint
{
	string joint_name;
	int joint_index;

	BVH_Joint* parent;
	vector<BVH_Joint*> children;
	double offset[3] = { 0.0, 0.0, 0.0 };
	vector<Channel*> channels;
	bool has_site_info;
	double site[3] = { 0.0, 0.0, 0.0 };
};

//the object structure of BVH
class BVH
{
public:
	BVH();
	BVH(const char* fileName);
	~BVH();

	void LoadFromFile(const char* fileName);

	//for formate testing
	static void ShowJoints(BVH_Joint* joint, int depth);
	void ShowAllStructure();

	//render figure
	void FigureRender(int frame_num, double bone_radius, double head_radius, float scale = 1.0f);
	
	double GetFrameTime() const { return this->frame_time; }
	int GetFrameNum() const { return this->frame_number; }
	string GetFileName() const { return this->file_name; }

	bool FileLoaded = false;

private:
	BVH_Joint* root;
	string file_name;
	string motion_name;
	double frame_time;
	int frame_number;
	vector<Channel*> channels;
	vector<double> motion;

	static void DeleteJoint(BVH_Joint*& joint);
	void FigureRender(const BVH_Joint* joint, int motion_index0_per_row, float scale, double bone_radius, double head_radius);
	void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, double bone_radius, Color bone_color = { 0.0f, 1.0f, 0.0f, 0.0f },
		Color joints_color = { 1.0f, 0.0f, 0.0f, 0.0f });
};