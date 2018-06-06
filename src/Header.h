//
// Created by KingSun on 2018/05/14
//

#ifndef HEADER_H
#define HEADER_H

//A definition header for project.

//Standard header
#include <iostream>
#include <string>
#include <cmath>

//OpenGL header
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define IMGUI_ENABLE
#ifdef IMGUI_ENABLE
//IMGUI header you should put all imgui file into project for some files need to compile
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#endif

//some basic type
using Kint = int;
using Kuint = unsigned int;
using Kfloat = float;
using Kdouble = double;
using Klong = long;
using Kulong = unsigned long;
using Kshort = short;
using Kushort = unsigned short;
using Kboolean = bool;
using Ksize = unsigned int;
using Kchar = char;
using Kuchar = unsigned char;
using Kbyte = char;
using Kubyte = unsigned char;

//Some definition
#define EPSILON_E6 1E-6
#define PI 3.1415926535
#ifdef KNAN
#undef KNAN
#endif
#define KNAN nan("Nan")
std::string RES_PATH = "./res/";

//debug
#include <iomanip>
#define glCall(x)	x; \
					glCheckError(#x, __FILE__, __LINE__);
void glClearError() { while (glGetError() != GL_NO_ERROR); } //抛弃当前所有错误;
bool glCheckError(const char* fun, const char* file, int line) {
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error at: " << fun << " in file: " << file << " at line: " << line
			<< " with error code: 0x0" << std::setbase(16) << error << std::endl;
		return false;
	}
	return true;
}

#endif //HEADER_H
