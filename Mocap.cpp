#include "Mocap.h"

// We'll be using buffer objects to store the kinect point cloud
GLuint vboId;
GLuint cboId;

// Intermediate Buffers
unsigned char rgbimage[colorwidth*colorheight * 4];    // Stores RGB color image
ColorSpacePoint depth2rgb[width*height];             // Maps depth pixels to rgb pixels
CameraSpacePoint depth2xyz[width*height];			 // Maps depth pixels to 3d coordinates

// Body tracking variables
BOOLEAN tracked;							// Whether we see a body
Joint joints[JointType_Count];				// List of joints in the tracked body
JointOrientation jointsOrientation[JointType_Count];

// Kinect Variables
IKinectSensor* sensor;             // Kinect sensor
IMultiSourceFrameReader* reader;   // Kinect data source
ICoordinateMapper* mapper;         // Converts between depth, color, and 3d coordinates

CameraSpacePoint currentLocation;

Pac* moCapPac;

void Output() {
	if (currentLocation.X > 0.3f && !isLeft) {
		isUp = false, isDown = false, isRight = false;
		isLeft = true;
		moCapPac->Turn(0);
	}
	else if (currentLocation.X < -0.3f && !isRight) {
		isUp = false, isDown = false, isLeft = false;
		isRight = true;
		moCapPac->Turn(180);
	}
	else if (currentLocation.Y > 0.3f && !isUp) {
		isDown = false, isLeft = false, isRight = false;
		isUp = true;
		moCapPac->Turn(90);
	}
	else if (currentLocation.Y < -0.3f && !isDown) {
		isUp = false, isLeft = false, isRight = false;
		isDown = true;
		moCapPac->Turn(270);
	}
	else if (currentLocation.X < 0.3f && currentLocation.X > -0.3f && currentLocation.Y < 0.3f && currentLocation.Y > -0.3f && (isUp || isDown || isLeft || isRight)) {
		isUp = false, isDown = false, isLeft = false, isRight = false;
	}
}

bool initKinect() {
	if (FAILED(GetDefaultKinectSensor(&sensor))) {
		return false;
	}
	if (sensor) {
		sensor->get_CoordinateMapper(&mapper);

		sensor->Open();
		sensor->OpenMultiSourceFrameReader(
			FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_Body,
			&reader);
		return reader;
	}
	else {
		return false;
	}
}

void getDepthData(IMultiSourceFrame* frame, GLubyte* dest) {
	IDepthFrame* depthframe;
	IDepthFrameReference* frameref = NULL;
	frame->get_DepthFrameReference(&frameref);
	frameref->AcquireFrame(&depthframe);
	if (frameref) frameref->Release();

	if (!depthframe) return;

	// Get data from frame
	unsigned int sz;
	unsigned short* buf;
	depthframe->AccessUnderlyingBuffer(&sz, &buf);

	// Write vertex coordinates
	mapper->MapDepthFrameToCameraSpace(width*height, buf, width*height, depth2xyz);
	float* fdest = (float*)dest;
	for (int i = 0; i < sz; i++) {
		*fdest++ = depth2xyz[i].X;
		*fdest++ = depth2xyz[i].Y;
		*fdest++ = depth2xyz[i].Z;
	}

	// Fill in depth2rgb map
	mapper->MapDepthFrameToColorSpace(width*height, buf, width*height, depth2rgb);
	if (depthframe) depthframe->Release();
}

void getRgbData(IMultiSourceFrame* frame, GLubyte* dest) {
	IColorFrame* colorframe;
	IColorFrameReference* frameref = NULL;
	frame->get_ColorFrameReference(&frameref);
	frameref->AcquireFrame(&colorframe);
	if (frameref) frameref->Release();

	if (!colorframe) return;

	// Get data from frame
	colorframe->CopyConvertedFrameDataToArray(colorwidth*colorheight * 4, rgbimage, ColorImageFormat_Rgba);

	// Write color array for vertices
	float* fdest = (float*)dest;
	for (int i = 0; i < width*height; i++) {
		ColorSpacePoint p = depth2rgb[i];
		// Check if color pixel coordinates are in bounds
		if (p.X < 0 || p.Y < 0 || p.X > colorwidth || p.Y > colorheight) {
			*fdest++ = 0;
			*fdest++ = 0;
			*fdest++ = 0;
		}
		else {
			int idx = (int)p.X + colorwidth * (int)p.Y;
			*fdest++ = rgbimage[4 * idx + 0] / 255.;
			*fdest++ = rgbimage[4 * idx + 1] / 255.;
			*fdest++ = rgbimage[4 * idx + 2] / 255.;
		}
	}

	if (colorframe) colorframe->Release();
}

void getBodyData(IMultiSourceFrame* frame) {
	IBodyFrame* bodyframe;
	IBodyFrameReference* frameref = NULL;
	frame->get_BodyFrameReference(&frameref);
	frameref->AcquireFrame(&bodyframe);
	if (frameref) frameref->Release();

	if (!bodyframe) return;

	IBody* body[BODY_COUNT] = { 0 };
	bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
	for (int i = 0; i < BODY_COUNT; i++) {
		body[i]->get_IsTracked(&tracked);
		if (tracked) {
			body[i]->GetJoints(JointType_Count, joints);
			body[i]->GetJointOrientations(JointType_Count, jointsOrientation);
			break;
		}
	}

	if (bodyframe) bodyframe->Release();
}

void getKinectData() {
	IMultiSourceFrame* frame = NULL;
	if (SUCCEEDED(reader->AcquireLatestFrame(&frame))) {
		GLubyte* ptr;
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (ptr) {
			getDepthData(frame, ptr);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindBuffer(GL_ARRAY_BUFFER, cboId);
		ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (ptr) {
			getRgbData(frame, ptr);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		getBodyData(frame);
	}
	if (frame) frame->Release();
}

void drawKinectData() {
	getKinectData();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, cboId);
	glColorPointer(3, GL_FLOAT, 0, NULL);

	glPointSize(5.f);
	glDrawArrays(GL_POINTS, 0, width*height);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	if (tracked) {

		const CameraSpacePoint& rht = joints[JointType_HandTipRight].Position;

		currentLocation = rht;

		glPointSize(5.0f);
		glBegin(GL_POINTS);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(rht.X, rht.Y, rht.Z);
		glEnd();


		glBegin(GL_LINES);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(-0.1f, -0.1f, 0.5f);
		glVertex3f(0.1f, -0.1f, 0.5f);

		glVertex3f(0.1f, -0.1f, 0.5f);
		glVertex3f(0.1f, 0.1f, 0.5f);

		glVertex3f(0.1f, 0.1f, 0.5f);
		glVertex3f(-0.1f, 0.1f, 0.5f);

		glVertex3f(-0.1f, 0.1f, 0.5f);
		glVertex3f(-0.1f, -0.1f, 0.5f);
		glEnd();

	}
}

void draw() {
	glutSetWindow(2);
	glutPostRedisplay();
	drawKinectData();
	glutSwapBuffers();
	Output();
}

int launchKinect(int argc, char* argv[], Pac* p) {
	moCapPac = p;
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Kinect Mocap");
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glewInit();
	
	if (!initKinect()) return 1;

	glutHideWindow();

	//OpenGL setup
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);

	// Set up array buffers
	const int dataSize = width * height * 3 * 4;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_DYNAMIC_DRAW);
	glGenBuffers(1, &cboId);
	glBindBuffer(GL_ARRAY_BUFFER, cboId);
	glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_DYNAMIC_DRAW);

	// Camera setup
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLdouble)height, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);

	return 0;
}