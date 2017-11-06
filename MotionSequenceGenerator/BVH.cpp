#include "BVH.h"

BVH::BVH()
{}

BVH::BVH(const char* fileName)
{
	this->LoadFromFile(fileName);
	this->file_name.assign(fileName);
}

void BVH::LoadFromFile(const char* fileName)
{
	ifstream file;
	const int LINEBUFFER = 1024 * 32;
	char line[LINEBUFFER];
	const char* DELIMITER = " ,:\t\n";
	char* token;
	char* buf;
	int jointIndex = 0;

	vector<BVH_Joint*> joint_stack;
	BVH_Joint* lastJoint = NULL;
	BVH_Joint* newJoint = NULL;

	double x, y, z;
	bool is_site = false;

	file.open(fileName, ios_base::in);
	if (!file.is_open())
		return;

	this->FileLoaded = true;

	while (!file.eof()) {
		if (file.eof()) {
			file.close();
			return;
		}

		file.getline(line, LINEBUFFER);
		token = strtok_s(line, DELIMITER, &buf);

		//meet a blank line
		if (token == NULL)
			continue;

		//meet "ROOT" or "JOINT"
		if ((strcmp(token, "ROOT") == 0) || (strcmp(token, "JOINT") == 0)) {

			newJoint = new BVH_Joint();
			newJoint->joint_index = jointIndex++;
			newJoint->parent = lastJoint;

			//when root
			if (strcmp(token, "ROOT") == 0) {
				this->root = newJoint;
			}

			//get name
			token = strtok_s(NULL, "", &buf);
			while ((*token) == ' ')
				token++;
			newJoint->joint_name = token;

			if (lastJoint) {
				lastJoint->children.push_back(newJoint);
			}

			continue;
		}

		//meet '{'
		if (strcmp(token, "{") == 0) {

			//push to temporary stack
			joint_stack.push_back(lastJoint);
			lastJoint = newJoint;

			continue;
		}

		//meet '}'
		if (strcmp(token, "}") == 0) {

			//pop from temporary stack to get last saved joint
			lastJoint = joint_stack.back();
			joint_stack.pop_back();

			if (is_site)
				is_site = false;

			continue;
		}

		//meet "OFFSET"
		if (strcmp(token, "OFFSET") == 0) {

			token = strtok_s(NULL, DELIMITER, &buf);
			x = (token != NULL) ? atof(token) : 0.0;
			token = strtok_s(NULL, DELIMITER, &buf);
			y = (token != NULL) ? atof(token) : 0.0;
			token = strtok_s(NULL, DELIMITER, &buf);
			z = (token != NULL) ? atof(token) : 0.0;

			if (is_site) {
				lastJoint->has_site_info = true;
				lastJoint->site[0] = x;
				lastJoint->site[1] = y;
				lastJoint->site[2] = z;
			}
			else {
				lastJoint->offset[0] = x;
				lastJoint->offset[1] = y;
				lastJoint->offset[2] = z;
			}


			continue;
		}

		//meet "CHANNELS"
		if (strcmp(token, "CHANNELS") == 0) {

			token = strtok_s(NULL, DELIMITER, &buf);
			lastJoint->channels.resize((token) ? atoi(token) : 0);

			for (unsigned int i = 0; i < lastJoint->channels.size(); ++i) {
				Channel* newChannel = new Channel();
				newChannel->joint = lastJoint;

				//to check the type of the new channel (Xposition Yposition Zposition Zrotation Yrotation Xrotation)
				token = strtok_s(NULL, DELIMITER, &buf);
				if (strcmp(token, "Xposition") == 0) {
					newChannel->type = Channel_Enum::Xposition;
				}
				else if (strcmp(token, "Yposition") == 0) {
					newChannel->type = Channel_Enum::Yposition;
				}
				else if (strcmp(token, "Zposition") == 0) {
					newChannel->type = Channel_Enum::Zposition;
				}
				else if (strcmp(token, "Xrotation") == 0) {
					newChannel->type = Channel_Enum::Xrotation;
				}
				else if (strcmp(token, "Yrotation") == 0) {
					newChannel->type = Channel_Enum::Yrotation;
				}
				else if (strcmp(token, "Zrotation") == 0) {
					newChannel->type = Channel_Enum::Zrotation;
				}

				//save new channel into bvh structure
				newChannel->channel_index = this->channels.size(); //begin from 0
				this->channels.push_back(newChannel);
				

				lastJoint->channels[i] = newChannel;

			}

			continue;
		}

		//meet "End"
		if (strcmp(token, "End") == 0) {

			is_site = true;
			newJoint = lastJoint;

			continue;
		}

		//meet "MOTION", just break out
		if (strcmp(token, "MOTION") == 0)
			break;

	}

	//begin reading motion information
	//meet "Frames"
	file.getline(line, LINEBUFFER);
	token = strtok_s(line, DELIMITER, &buf);
	if (strcmp(token, "Frames") != 0) {
		file.close();
		if (FileLoaded)
			FileLoaded = false;
		return;
	}
	token = strtok_s(NULL, DELIMITER, &buf);
	if (!token) {
		file.close();
		if (FileLoaded)
			FileLoaded = false;
		return;
	}
	this->frame_number = atoi(token);

	//meet "Frame Time"
	file.getline(line, LINEBUFFER);
	token = strtok_s(line, ":", &buf);
	if (strcmp(token, "Frame Time") != 0) {
		file.close();
		if (FileLoaded)
			FileLoaded = false;
		return;
	}
	token = strtok_s(NULL, DELIMITER, &buf);
	if (!token) {
		file.close();
		if (FileLoaded)
			FileLoaded = false;
		return;
	}
	this->frame_time = atof(token);

	//meet motions
	this->motion.resize(channels.size() * frame_number);

	for (int f_n = 0; f_n < frame_number; ++f_n) {
		file.getline(line, LINEBUFFER);
		token = strtok_s(line, DELIMITER, &buf);
		if (!token) {
			file.close();
			return;
		}
		for (unsigned int c_n = 0; c_n < channels.size(); ++c_n) {
			motion[f_n * channels.size() + c_n] = atof(token);
			token = strtok_s(NULL, DELIMITER, &buf);
		}
	}

	//file close
	file.close();

}

void BVH::DeleteJoint(BVH_Joint*& joint)
{
	if (joint) {
		//check for site
		if (joint->has_site_info) {
			delete joint;
			joint = NULL;
		}
		else {
			for (unsigned int i = 0; i < joint->children.size(); ++i) {
				DeleteJoint(joint->children[i]);
			}

			if (joint) {
				delete joint;
				joint = NULL;
			}
		}
	}
}

void BVH::ShowJoints(BVH_Joint* joint, int depth)
{
	if (joint->has_site_info) {
		for (int i = 0; i < depth; i++)
			cout << " ";
		cout << "depth=" << depth << ":";
		cout << joint->joint_index << ".";
		//cout << joint->joint_name << "|";
		cout << joint->offset[0] << ";";
		cout << joint->offset[1] << ";";
		cout << joint->offset[2] << "|";
		cout << joint->channels.size() << "|";
		cout << joint->site[0] << ";";
		cout << joint->site[1] << ";";
		cout << joint->site[2] << ";" << endl;
	}
	else {
		for (vector<BVH_Joint*>::const_iterator iter = joint->children.begin();
			iter != joint->children.end(); ++iter) {
			ShowJoints(*iter, depth+1);
		}
		for (int i = 0; i < depth; i++)
			cout << " ";
		cout << "depth=" << depth << ":";
		cout << joint->joint_index << ".";
		cout << joint->joint_name << "|";
		cout << joint->offset[0] << ";";
		cout << joint->offset[1] << ";";
		cout << joint->offset[2] << "|";
		cout << joint->channels.size() << endl;
	}

}

void BVH::ShowAllStructure()
{
	this->ShowJoints(this->root, 0);
	cout << "----------------------------------------------" << endl;
	cout << "channel size = " << this->channels.size() << endl;
	cout << "frame_number = " << this->frame_number << endl;
	cout << "frame_time = " << this->frame_time << endl;
	cout << "----------------------------------------------" << endl;

	//for (unsigned int f_n = 0; f_n < frame_number; ++f_n) {
	//	for (unsigned int c_n = 0; c_n < channels.size(); ++c_n) {
	//		cout << motion[f_n * channels.size() + c_n];
	//		cout << ",";
	//	}
	//	cout << endl;
	//}
}

void BVH::FigureRender(int frame_num, double bone_radius, double head_radius, float scale)
{
	FigureRender(root, frame_num*this->channels.size(), scale, bone_radius, head_radius);
}

void BVH::FigureRender(const BVH_Joint* joint, int motion_index0_per_row, float scale, double bone_radius, double head_radius)
{
	glPushMatrix();

	//translation
	//when root joint
	if (!joint->parent) {
		glTranslatef(motion[motion_index0_per_row + 0] * scale,
			motion[motion_index0_per_row + 1] * scale,
			motion[motion_index0_per_row + 2] * scale);
	}
	//when other joints
	else {
		glTranslatef(joint->offset[0] * scale, joint->offset[1] * scale,
			joint->offset[2] * scale);
	}

	//rotation
	for (vector<Channel*>::const_iterator iter = joint->channels.begin();
		iter != joint->channels.end(); ++iter) {
		Channel* cur_channel = *iter;
		if (cur_channel->type == Channel_Enum::Xrotation) {
			glRotatef(motion[motion_index0_per_row + cur_channel->channel_index],
				1.0f, 0.0f, 0.0f);
		}
		else if (cur_channel->type == Channel_Enum::Yrotation) {
			glRotatef(motion[motion_index0_per_row + cur_channel->channel_index],
				0.0f, 1.0f, 0.0f);
		}
		else if (cur_channel->type == Channel_Enum::Zrotation) {
			glRotatef(motion[motion_index0_per_row + cur_channel->channel_index],
				0.0f, 0.0f, 1.0f);
		}
	}
	
	//begin bone rendering
	//RenderBone(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, { 1.0f, 0.0f, 0.0f }); //just for test
	
	//when site node
	Color bone_color = { 0.0f, 0.0f, 1.0f, 0.0f };
	Color joints_color = bone_color;

	if (joint->has_site_info) {
		//when head
		if ((strcmp(joint->joint_name.c_str(), "Head") == 0) ||
			(strcmp(joint->joint_name.c_str(), "head") == 0)) {
			glPushMatrix(); {
				glTranslatef(0.0f, (head_radius*3.0f)/4.0f, 0.0f);
				glScalef(3.0f/4.0f, 1.0f, 3.0f/4.0f);
				GLUquadric* headSphere = gluNewQuadric();
				gluQuadricDrawStyle(headSphere, GLU_FILL);
				gluQuadricNormals(headSphere, GLU_SMOOTH);
				glColor3f(bone_color.r, bone_color.g, bone_color.b);
				gluSphere(headSphere, head_radius, 20, 20);
			}
			glPopMatrix();
		}
		else {
			RenderBone(0.0f, 0.0f, 0.0f, joint->site[0] * scale, joint->site[1] * scale, joint->site[2] * scale,
				bone_radius, bone_color, joints_color);
		}
		//cout << joint->joint_name << " rendered!" << endl;
	}
	//when joint node has one child
	else if (joint->children.size() == 1) {
		BVH_Joint* child = joint->children[0];
		RenderBone(0.0f, 0.0f, 0.0f, child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale,
			bone_radius, bone_color, joints_color);
		//cout << joint->joint_name << " rendered!" << endl;
	}
	//when joint node has at least two children
	else if (joint->children.size() > 1) {
		float  center[3] = { 0.0f, 0.0f, 0.0f };
		for (unsigned int i = 0; i<joint->children.size(); i++)
		{
			BVH_Joint *  child = joint->children[i];
			center[0] += child->offset[0];
			center[1] += child->offset[1];
			center[2] += child->offset[2];
		}
		center[0] /= joint->children.size() + 1;
		center[1] /= joint->children.size() + 1;
		center[2] /= joint->children.size() + 1;

		RenderBone(0.0f, 0.0f, 0.0f, center[0] * scale, center[1] * scale, center[2] * scale,
			bone_radius, bone_color, joints_color);

		for (unsigned int i = 0; i<joint->children.size(); i++)
		{
			BVH_Joint *  child = joint->children[i];
			RenderBone(center[0] * scale, center[1] * scale, center[2] * scale,
				child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale,
				bone_radius, bone_color, joints_color);
		}

		//cout << joint->joint_name << " rendered!" << endl;
	}

	//begin recurrent call to render children joints
	for (vector<BVH_Joint*>::const_iterator c_iter = joint->children.begin();
		c_iter != joint->children.end(); ++c_iter) {
		FigureRender(*c_iter, motion_index0_per_row, scale, bone_radius, head_radius);
	}

	glPopMatrix();
}

void BVH::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, double bone_radius,
	Color bone_color, Color joints_color)
{
	double height = 0.0;
	double tmp_len = 0.0;
	int slices = 10;
	int stacks = 4;
	GLUquadric* qObj = NULL;
	if (qObj == NULL)
		qObj = gluNewQuadric();

	double dir_x = x1 - x0;
	double dir_y = y1 - y0;
	double dir_z = z1 - z0;
	tmp_len = height = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);

	double front_x = 0.0;
	double front_y = 0.0;
	double front_z = 1.0;

	glPushMatrix(); {
		//begin translate
		glTranslatef(x0, y0, z0);

		//direction normalization
		if (tmp_len < 0.0001) {
			dir_x = 0.0;
			dir_y = 0.0;
			dir_z = 1.0;
			tmp_len = 1.0;
		}
		dir_x /= tmp_len;
		dir_y /= tmp_len;
		dir_z /= tmp_len;

		//calculate normal vector and normalization
		double normal_x = front_y*dir_z - front_z*dir_y;
		double normal_y = front_z*dir_x - front_x*dir_z;
		double normal_z = front_x*dir_y - front_y*dir_x;
		tmp_len = sqrt(normal_x*normal_x + normal_y*normal_y + normal_z*normal_z);
		if (tmp_len < 0.0001) {
			normal_x = 1.0;
			normal_y = 0.0;
			normal_z = 0.0;
			tmp_len = 1.0;
		}
		normal_x /= tmp_len;
		normal_y /= tmp_len;
		normal_z /= tmp_len;

		//calculate angle
		float angle = acos(front_x*dir_x + front_y*dir_y + front_z*dir_z) * 180.0f / PI;

		//begin rotate
		glRotatef(angle, normal_x, normal_y, normal_z);

		gluQuadricDrawStyle(qObj, GLU_FILL);
		gluQuadricNormals(qObj, GLU_SMOOTH);

		//begin drawing cylinder
		glColor3f(bone_color.r, bone_color.g, bone_color.b);
		gluCylinder(qObj, bone_radius, bone_radius, height, slices, stacks);
	}
	glPopMatrix();

	//draw joint pair
	double sphere_slices = 20.0f;
	double sphere_stacks = 20.0f;
	GLUquadric* joint = NULL;

	if (joint == NULL)
		joint = gluNewQuadric();
	//first joint
	glPushMatrix(); {
		glTranslatef(x0, y0, z0);
		gluQuadricDrawStyle(joint, GLU_FILL);
		gluQuadricNormals(joint, GLU_SMOOTH);
		glColor3f(joints_color.r, joints_color.g, joints_color.b);
		gluSphere(joint, bone_radius, sphere_slices, sphere_stacks);
	}
	glPopMatrix();
	//second joint
	glPushMatrix(); {
		glTranslatef(x1, y1, z1);
		gluQuadricDrawStyle(joint, GLU_FILL);
		gluQuadricNormals(joint, GLU_SMOOTH);
		glColor3f(joints_color.r, joints_color.g, joints_color.b);
		gluSphere(joint, bone_radius, sphere_slices, sphere_stacks);
	}
	glPopMatrix();

}

BVH::~BVH()
{
	DeleteJoint(this->root);
	this->file_name = "";
	this->motion_name = "";
	if (FileLoaded)
		FileLoaded = false;
	for (vector<Channel*>::const_iterator iter = channels.begin();
		iter != channels.end(); ++iter) {
		delete *iter;
	}

}