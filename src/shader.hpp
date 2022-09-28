#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>

using std::string, std::stringstream, std::ifstream;
using std::cout, std::endl;

unsigned int compileShader(string vertfile, string fragfile);

