#pragma once
#include <Windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "glut.h"

#include <cmath>
#include <cstdio>

#include <Windows.h>
#include <Ole2.h>
#include <iostream>
#include <fstream>

#include "Kinect.h"
#include "pac.h"
#include <vector>

const int width = 512;
const int height = 424;
const int colorwidth = 1920;
const int colorheight = 1080;

static bool isUp = false, isDown = false, isLeft = false, isRight = false;


int launchKinect(int argc, char* argv[], Pac* p);