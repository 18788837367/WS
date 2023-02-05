#pragma once

#include "glad/glad.h"
#include <glm/glm.hpp>
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

	void setVec2(const std::string &name, const glm::vec2& value);
	void setVec2(const std::string &name, float x, float y);

	void setVec3(const std::string &name, const glm::vec3& value);
	void setVec3(const std::string &name, float x, float y, float z);

	void setVec4(const std::string &name, const glm::vec4& value);
	void setVec4(const std::string &name, float x, float y, float z, float w);

	void setMat2(const std::string &name, const glm::mat2 & value);

	void setMat3(const std::string &name, const glm::mat3 & value);

	void setMat4(const std::string &name, const glm::mat4 & value);

	unsigned int ID;
};

