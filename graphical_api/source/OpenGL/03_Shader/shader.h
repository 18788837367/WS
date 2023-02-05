#pragma once

#include "glad/glad.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Shader
{
public:
	Shader(const GLchar *vertexPath, const GLchar *fragPath);

	~Shader();

	void use();

	void setBool(const std::string &name, bool value);

	void setInt(const std::string &name, int value);

	void setFloat(const std::string &name, float value);

	unsigned int ID;
};

