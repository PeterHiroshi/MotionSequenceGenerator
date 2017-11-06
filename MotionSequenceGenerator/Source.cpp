#include "Header.h"

//global settings
////for test
const string BVH_ROOT_PATH = "bvh_files\\FOR_MY_TEST";

//const string BVH_ROOT_PATH = "bvh_files";

////for test
//const string PATH = "bvh_files\\FOR_MY_TEST\\b0003.bvh";
//const string PATH2 = "bvh_files\\FOR_MY_TEST\\b0002.bvh";

const unsigned int DISPLAYMODE = GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL;
const GLbitfield CLEAR_MASK = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
const Color BACKGROUND_COLOR = { 1.0f, 1.0f, 1.0f };
int win_width = 512;
int win_height = 512;
const int WINDOW_POS_X = 50;
const int WINDOW_POS_Y = 50;
const char* TITLE = "MotionSequenceGenerator";
const double RANGE = 30.0;
const double Z_NEAR = 100.0;
const double Z_FAR = -100.0;

//for bone rendering
const float BONE_SCALE = 0.7f;
const double BONE_RADIUS = 0.65;
const double HEAD_RADIUS = BONE_RADIUS*3.0;

//for animation
const float ANIMATION_RATE = 0.005f;
const float ANIMATION_TIME_TOP = 5.0f;

//for motion image list
const int LIST_COL_NUM = 25; //must bigger than 1

//BVH structure
BVH* bvh = NULL;

//varibles
int frame_num = 0;
float animation_time = 0.0f;
bool animation_on = true;
vector<string> bvh_file_name_list;

////global rotations
float x_rot_angle = 0.0f;
float y_rot_angle = 0.0f;
float z_rot_angle = 0.0f;

int rot_index = 0;
const float ROT_ANGLE_PER_TIME = 30.0f;

bool do_rotate = false;

//for image saving
GLbyte* pixels = NULL;
vector<Mat> mat_List;

const string SAVED_IMAGE_LIST_ROOT_PATH = "image_output";
const string IMAGE_SUFFIX = "bmp";

////for test
//const string SAVED_IMAGE_LIST_PATH = "image_output\\result.bmp";

int main(int argc, char** argv)
{
	//BVH* bvh = new BVH(PATH.c_str());
	
	//initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(DISPLAYMODE);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
	glutCreateWindow(TITLE);


	//structure testing
	if (bvh)
		bvh->ShowAllStructure();
	
	//callback functions
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

#ifdef NORMAL_ANIMATION_MODE
	////for test
	glutKeyboardFunc(Keyboard);
#endif // NORMAL_ANIMATION_MODE

	glutIdleFunc(Idle);

	//initialize environment
	EnvironmentInit();
	
	//main loop
	glutMainLoop();

	return 0;
}

void EnvironmentInit(void)
{
	//set background color
	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b,
		BACKGROUND_COLOR.alpha);
	
	LoadFilePathList(BVH_ROOT_PATH, bvh_file_name_list);

	if (bvh_file_name_list.size() != 0) {
		//bvh = new BVH(PATH.c_str());
		cout << "list current size = " << bvh_file_name_list.size() << endl;

		bvh = new BVH(bvh_file_name_list[bvh_file_name_list.size() - 1].c_str());
		bvh_file_name_list.pop_back();
	}
	else
		exit(-1);

	if (!bvh->FileLoaded)
		exit(-1);
}

void Display(void)
{
	//clear display window
	glClear(CLEAR_MASK);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//here I use the default setting of glLookAt()

	glPushMatrix(); {

#ifdef TEST_MODE
		glPushMatrix();
		glColor3f(0.0f, 0.0f, 1.0f);
		glRotatef(45.0f, 1.0f, 1.0f, 1.0f);
		glutWireCube(3.0);

		glPopMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS); {
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
		}
		glEnd();

#else
		//if (!bvh) {
		//	if (bvh_file_name_list.size() != 0) {
		//		//bvh = new BVH(PATH.c_str());
		//		cout << "list current size = " << bvh_file_name_list.size() << endl;

		//		bvh = new BVH(bvh_file_name_list[bvh_file_name_list.size() - 1].c_str());
		//		bvh_file_name_list.pop_back();
		//	}
		//	else
		//		exit(-1);
		//	if (!bvh->FileLoaded)
		//		exit(-1);
		//}
			//bvh = new BVH(PATH.c_str());

		if (!bvh && bvh_file_name_list.size() != 0)
			exit(-1);
		else if (!bvh && bvh_file_name_list.size() == 0)
			exit(0);

		glTranslatef(-2.0f, -5.0f, 0.0f); //adjust position of model

		glRotatef(x_rot_angle, -1.0f, 0.0f, 0.0f); //camera_x rotation
		glRotatef(y_rot_angle, 0.0f, -1.0f, 0.0f); //camera_y rotation
		glRotatef(z_rot_angle, 0.0f, 0.0f, -1.0f); //camera_z rotation

		bvh->FigureRender(frame_num, BONE_RADIUS, HEAD_RADIUS, BONE_SCALE);

#endif // TEST_MODE

	}
	glPopMatrix();

	glutSwapBuffers();

}

void Reshape(int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-RANGE, RANGE, -RANGE, RANGE, -Z_NEAR, -Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	win_width = new_width;
	win_height = new_height;

}



void Idle(void)
{
	if (animation_on) {

#ifdef NORMAL_ANIMATION_MODE
		animation_time += ANIMATION_RATE;
#endif // NORMAL_ANIMATION_MODE

		if (bvh) {

#ifdef NORMAL_ANIMATION_MODE
			//frame_num = animation_time / bvh->GetFrameTime();
			//frame_num = frame_num % bvh->GetFrameNum(); //update frame number

			frame_num += 1;

			//without rotation
			if (rot_index == Rotation_Enum::y_rot_0 && do_rotate) {
				do_rotate = false;
			}

			//camera_y rotate 30 degree
			if (rot_index == Rotation_Enum::y_rot_30 && do_rotate) {
				Z_Rotation(ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate 60 degree
			if (rot_index == Rotation_Enum::y_rot_60 && do_rotate) {
				Z_Rotation(ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate -30 degree
			if (rot_index == Rotation_Enum::y_rot_m30 && do_rotate) {
				Rot_Restore();
				Z_Rotation(-ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate -60 degree
			if (rot_index == Rotation_Enum::y_rot_m60 && do_rotate) {
				Z_Rotation(-ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//if (frame_num == 0 && animation_time > ANIMATION_TIME_TOP) {

			if (frame_num >= bvh->GetFrameNum()) {
				frame_num = 0;
				rot_index++;
				do_rotate = true;

				//animation_time = 0.0f;

				//animation_on = false;
				if (rot_index > Rotation_Enum::y_rot_m60) {
					Rot_Restore();
					do_rotate = false;
					rot_index = 0;
					LoadNextBVH();
				}
				/*else
					animation_time = 0.0f;*/
			}
#else
			frame_num += 1; //update frame number

			//without rotation
			if (rot_index == Rotation_Enum::y_rot_0 && do_rotate) {
				do_rotate = false;
			}

			//camera_y rotate 30 degree
			if (rot_index == Rotation_Enum::y_rot_30 && do_rotate) {
				Y_Rotation(ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate 60 degree
			if (rot_index == Rotation_Enum::y_rot_60 && do_rotate) {
				Y_Rotation(ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate -30 degree
			if (rot_index == Rotation_Enum::y_rot_m30 && do_rotate) {
				Rot_Restore();
				Y_Rotation(-ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//camera_y rotate -60 degree
			if (rot_index == Rotation_Enum::y_rot_m60 && do_rotate) {
				Y_Rotation(-ROT_ANGLE_PER_TIME);
				do_rotate = false;
			}

			//frame_num = frame_num % bvh->GetFrameNum();
			int frame_num_per_col = bvh->GetFrameNum() / (LIST_COL_NUM - 1);

			//check whether save a image or not
			if ((frame_num-1 == 1) || (frame_num == (bvh->GetFrameNum()-1)) || 
				((frame_num%frame_num_per_col == 0) && (frame_num < bvh->GetFrameNum()))
				) {
				//cout << "-------------------------------snap here !!!!! " << frame_num << endl;

				Mat imgMat;
				Mat imgMatFlip;

				imgMat = SaveAsImage();
				flip(imgMat, imgMatFlip, 1);

				mat_List.push_back(imgMat);
				mat_List.push_back(imgMatFlip);
					
			}

			if ((frame_num >= bvh->GetFrameNum()) && (rot_index < Rotation_Enum::y_rot_m60)) {
				do_rotate = true;
				frame_num = 0;
				rot_index++;
			}
			else if ((frame_num >= bvh->GetFrameNum()) && (rot_index >= Rotation_Enum::y_rot_m60)) {
				cout << "mat list size = " << mat_List.size() << endl;

				//convert to destination path
				string savedPath = "";
				PathConvert(bvh->GetFileName(), SAVED_IMAGE_LIST_ROOT_PATH, savedPath, IMAGE_SUFFIX);

				cout << bvh->GetFileName() << endl;

				//save image list
				//SaveImageList(savedPath, mat_List);

				frame_num = 0;
				animation_on = false;
				mat_List.clear();

				//restore rotation process
				do_rotate = false;
				Rot_Restore();
				rot_index = 0;

				LoadNextBVH();
			}

#endif // NORMAL_ANIMATION_MODE

		}
		else {
			frame_num = 0;
		}

		glutPostRedisplay();

	}
}

Mat SaveAsImage(void)
{
	int viewPort[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, viewPort);
	if (pixels != NULL)
		delete[] pixels;
	int view_width = viewPort[2] - viewPort[0];
	int view_height = viewPort[3] - viewPort[1];
	pixels = new GLbyte[view_width * view_height * 3];
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);

	Mat img;
	//vector<Mat> imgPlanes; //will meet debug assertion failed
	vector<Mat> imgPlanes(1000);
	img.create(view_height, view_width, CV_8UC3);
	split(img, imgPlanes);
	
	for (int row = 0; row < view_height; ++row) {
		uchar* plane0Ptr = imgPlanes[0].ptr<uchar>(row);
		uchar* plane1Ptr = imgPlanes[1].ptr<uchar>(row);
		uchar* plane2Ptr = imgPlanes[2].ptr<uchar>(row);

		for (int col = 0; col < view_width; ++col) {
			int index = (row*view_width + col) * 3;
			plane2Ptr[col] = pixels[index];
			plane1Ptr[col] = pixels[index + 1];
			plane0Ptr[col] = pixels[index + 2];
		}
	}

	merge(imgPlanes, img);
	flip(img, img, 0); 
	//cv::imshow("show", img);
	//cout << "opencv save opengl img done!" << endl;

	return img;
}

void SaveImageList(string path, const vector<Mat>& matList)
{
	//test here
	/*for (int n = 0; n < matList.size(); ++n) {
		ostringstream oss;
		oss << "show" << n;
		imshow(oss.str(), matList[n]); 
	}*/

	int _row = matList[0].rows * 2;
	int _cols = matList[0].cols * (matList.size() / 2);
	int _type = matList[0].type();

	//cout << "rows = " << _row << endl;
	//cout << "cols = " << _cols << endl;

	Mat imageList;
	imageList.create(_row, _cols, _type);

	Mat tmpMat;
	Mat imgROI;

	/*for (unsigned int i = 0; i < matList.size(); ++i) {
	tmpMat = matList[i];
	tmpMat.colRange(0, matList[i].cols).copyTo(imageList.colRange(i*matList[i].cols,
	(i + 1)*matList[i].cols));
	}*/

	//first row
	for (unsigned int i = 0, j = 0; i < matList.size(); i += 2, j++) {
		tmpMat = matList[i]; //tmpMat is the basic mat from matList
							 //tmpMat.colRange(0, tmpMat.cols).copyTo(imgList.colRange(j*tmpMat.cols, (j + 1)*tmpMat.cols));

		imgROI = imageList(Rect(j*tmpMat.cols, 0, tmpMat.cols, tmpMat.rows));
		tmpMat.colRange(0, tmpMat.cols).copyTo(imgROI);

	}
	//second row
	for (unsigned int i = 1, j = 0; i < matList.size(); i += 2, j++) {
		tmpMat = matList[i]; //tmpMat is the basic mat from matList

		imgROI = imageList(Rect(j*tmpMat.cols, tmpMat.rows, tmpMat.cols, tmpMat.rows));
		tmpMat.colRange(0, tmpMat.cols).copyTo(imgROI);

	}
	
	//imshow(path, imageList);

	//save as image file
	bool result = imwrite(path, imageList);
	if (result)
		cout << "successfully saved at" << path << endl;

}

void LoadNextBVH()
{
	if (bvh_file_name_list.size() == 0) {
		animation_on = false;

		//there have no more bvh resources
		//resource release
		if (bvh) {
			delete bvh;
			bvh = NULL;
		}
	}
	if (bvh_file_name_list.size() != 0) {

		if (!animation_on)
			animation_on = true;

		if (bvh) {
			delete bvh;
			bvh = NULL;
		}
		bvh = new BVH(bvh_file_name_list[bvh_file_name_list.size() - 1].c_str());
		bvh_file_name_list.pop_back();
	}
	if (bvh && !bvh->FileLoaded)
		exit(-1);

	animation_time = 0.0f;



}

////for test on NORMAL_ANIMATION_MODE
void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'k') {
		
		////bvh = new BVH(PATH2.c_str());
		//if (bvh_file_name_list.size() != 0) {

		//	if (bvh) {
		//		delete bvh;
		//		bvh = NULL;
		//	}

		//	//bvh = new BVH(PATH.c_str());
		//	bvh = new BVH(bvh_file_name_list[bvh_file_name_list.size() - 1].c_str());
		//	bvh_file_name_list.pop_back();
		//}

		//if (bvh && !bvh->FileLoaded)
		//	exit(-1);

		//animation_time = 0.0f;

		LoadNextBVH();
	}
	else if (key == 'a') {
		X_Rotation(ROT_ANGLE_PER_TIME);
	}
	else if (key == 's') {
		Y_Rotation(ROT_ANGLE_PER_TIME);
	}
	else if (key == 'd') {
		Z_Rotation(ROT_ANGLE_PER_TIME);
	}
	else if (key == 'c') {
		Rot_Restore();
	}

	//begin re-display
	glutPostRedisplay();
}

void LoadFilePathList(string rootPath, vector<string>& list)
{
	string p;
	struct _finddata_t findData;
	intptr_t hFile;

	if ((hFile = _findfirst(p.assign(rootPath).append("\\*").c_str(), &findData)) != -1) {
		do {

			//when meet a sub directory
			if (findData.attrib & _A_SUBDIR) {
				if ((strcmp(findData.name, ".") != 0) && (strcmp(findData.name, "..") != 0)) {
					LoadFilePathList(p.assign(rootPath).append("\\").append(findData.name), list);
				}
			}
			//when meet a file
			else {
				list.push_back(p.assign(rootPath).append("\\").append(findData.name));
			}

		} while (_findnext(hFile, &findData) == 0);
		_findclose(hFile);
	}
}

void PathConvert(const std::string& srcPath, const std::string& dstRoot, std::string& dstPath,
	const std::string& suffix)
{
	dstPath.assign(srcPath);
	int pos = dstPath.find_first_of('\\');
	dstPath.replace(0, pos, dstRoot);
	pos = dstPath.find('.');
	dstPath.replace(pos + 1, dstPath.length(), suffix);
}

void X_Rotation(float angle)
{
	x_rot_angle += angle;
	if (x_rot_angle >= 360.0f)
		x_rot_angle = 0.0f;
}

void Y_Rotation(float angle)
{
	y_rot_angle += angle;
	if (y_rot_angle >= 360.0f)
		y_rot_angle = 0.0f;
}

void Z_Rotation(float angle)
{
	z_rot_angle += angle;
	if (z_rot_angle >= 360.0f)
		z_rot_angle = 0.0f;
}
void Rot_Restore()
{
	if (x_rot_angle != 0.0f)
		x_rot_angle = 0.0f;
	if (y_rot_angle != 0.0f)
		y_rot_angle = 0.0f;
	if (z_rot_angle != 0.0f)
		z_rot_angle = 0.0f;
}