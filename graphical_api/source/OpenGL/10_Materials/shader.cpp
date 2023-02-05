#include "shader.h"

Shader::Shader(const char * vertexPath, const char * fragmentPath, const char * geomtryPath)
{
	std::string vertexCode, fragmentCode, geomtryCode;

	std::ifstream vShaderFile,fShaderFile,gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	
	std::stringstream vShaderStream, fShaderStream;
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();

	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();

	vShaderFile.close();
	fShaderFile.close();

	if (geomtryPath != nullptr)
	{
		gShaderFile.open(geomtryPath);
		std::stringstream gShaderStream;
		gShaderStream << gShaderFile.rdbuf();
		geomtryCode = gShaderStream.str();
		gShaderFile.close();
	}
	
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;
	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode,NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	//fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	//geometry
	unsigned int geomtry;
	if (geomtryPath != nullptr)
	{
		const char* gShaderCode = geomtryCode.c_str();
		geomtry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geomtry, 1, &gShaderCode, NULL);
		glCompileShader(geomtry);
		checkCompileErrors(geomtry, "GEOMETRY");
	}
	//program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geomtryPath != nullptr)
		glAttachShader(ID, geomtry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	//delete
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geomtryPath != nullptr)
		glDeleteShader(geomtry);
}

Shader::~Shader()
{

}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value)
{
	glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void Shader::setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, float x, float y)
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()),1,&value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()),1,&value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setVec4(const std::string &name, const glm::vec4& value)
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string &name, const glm::mat2 & value)
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 & value)
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 & value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type == "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR:SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n ---------------------" << std::endl;
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR:SHADER_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n ---------------------" << std::endl;
			}
		}
	}
}