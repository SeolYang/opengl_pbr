#include "Shader.h"
#include "Rendering.h"

#include <fstream>
#include <sstream>
#include <iostream>

constexpr unsigned int INVALID_LOC = 0xFFFFFFFF;

Shader::Shader(const std::string& csPath)
{
	std::string csRaw;
	std::ifstream csFile;

	csFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try
	{
		csFile.open(csPath);
		std::stringstream vsStream;
		vsStream << csFile.rdbuf();
		csFile.close();

		csRaw = vsStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to open shader files " << e.what() << std::endl;
	}

	const char* vsCode = csRaw.c_str();
	unsigned int cs;
	int success = 0;
	char compileLog[512];

	cs = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cs, 1, &vsCode, nullptr);
	glCompileShader(cs);
	glGetShaderiv(cs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(cs, 512, nullptr, compileLog);
		std::cout << "Failed to compile compute shader: " << compileLog << std::endl;
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, cs);
	glLinkProgram(m_id);
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(m_id, 512, nullptr, compileLog);
		std::cout << "Failed to link shader program: " << compileLog << std::endl;
	}

	glDeleteShader(cs);
}

/* @TODO	셰이더 로드시 중복되는 코드 함수로 빼내기 */
Shader::Shader(
	const std::string& vsPath,
	const std::string& fsPath)
{
	std::string vsRaw;
	std::string fsRaw;

	std::ifstream vsFile;
	std::ifstream fsFile;

	vsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try
	{
		vsFile.open(vsPath);
		fsFile.open(fsPath);

		std::stringstream vsStream;
		std::stringstream fsStream;

		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();

		vsFile.close();
		fsFile.close();

		vsRaw = vsStream.str();
		fsRaw = fsStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to open shader files " << e.what() << std::endl;
	}

	const char* vsCode = vsRaw.c_str();
	const char* fsCode = fsRaw.c_str();

	unsigned int vs;
	unsigned int fs;
	int success = 0;
	char compileLog[512];

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsCode, nullptr);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vs, 512, nullptr, compileLog);
		std::cout << "Failed to compile vertex shader: " << compileLog << std::endl;
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsCode, nullptr);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(fs, 512, nullptr, compileLog);
		std::cout << "Failed to compile fragment shader: " << compileLog << std::endl;
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, vs);
	glAttachShader(m_id, fs);
	glLinkProgram(m_id);

	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(m_id, 512, nullptr, compileLog);
		std::cout << "Failed to link shader program: " << compileLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::Shader(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath)
{
	std::string vsRaw;
	std::string gsRaw;
	std::string fsRaw;

	std::ifstream vsFile;
	std::ifstream gsFile;
	std::ifstream fsFile;

	vsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	gsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fsFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try
	{
		vsFile.open(vsPath);
		gsFile.open(gsPath);
		fsFile.open(fsPath);

		std::stringstream vsStream;
		std::stringstream gsStream;
		std::stringstream fsStream;

		vsStream << vsFile.rdbuf();
		gsStream << gsFile.rdbuf();
		fsStream << fsFile.rdbuf();

		vsFile.close();
		gsFile.close();
		fsFile.close();

		vsRaw = vsStream.str();
		gsRaw = gsStream.str();
		fsRaw = fsStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to open shader files " << e.what() << std::endl;
	}

	const char* vsCode = vsRaw.c_str();
	const char* gsCode = gsRaw.c_str();
	const char* fsCode = fsRaw.c_str();

	unsigned int vs = 0;
	unsigned int gs = 0;
	unsigned int fs = 0;
	int success = 0;
	char compileLog[512];

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsCode, nullptr);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vs, 512, nullptr, compileLog);
		std::cout << "Failed to compile vertex shader: " << compileLog << std::endl;
	}

	gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &gsCode, nullptr);
	glCompileShader(gs);
	glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vs, 512, nullptr, compileLog);
		std::cout << "Failed to compile geometry shader: " << compileLog << std::endl;
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsCode, nullptr);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(fs, 512, nullptr, compileLog);
		std::cout << "Failed to compile fragment shader: " << compileLog << std::endl;
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, vs);
	glAttachShader(m_id, gs);
	glAttachShader(m_id, fs);
	glLinkProgram(m_id);

	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(m_id, 512, nullptr, compileLog);
		std::cout << "Failed to link shader program: " << compileLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::SetInt(const std::string& name, int value)
{
	unsigned int loc = FindLoc(name);
	if (loc != INVALID_LOC)
	{
		glUniform1i(loc, value);
	}
}

void Shader::SetFloat(const std::string& name, float value)
{
	unsigned int loc = FindLoc(name);
	if (loc != INVALID_LOC)
	{
		glUniform1f(loc, value);
	}
}

void Shader::SetVec3f(const std::string& name, glm::vec3 value)
{
	unsigned int loc = FindLoc(name);
	if (loc != INVALID_LOC)
	{
		glUniform3fv(loc, 1, &value[0]);
	}
}

void Shader::SetVec4f(const std::string& name, glm::vec4 value)
{
	unsigned int loc = FindLoc(name);
	if (loc != INVALID_LOC)
	{
		glUniform4fv(loc, 1, &value[0]);
	}
}

void Shader::SetMat4f(const std::string& name, glm::mat4 value)
{
	unsigned int loc = FindLoc(name);
	if (loc != INVALID_LOC)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
	}
}

unsigned int Shader::FindLoc(const std::string& name)
{
	unsigned int loc = INVALID_LOC;
	auto found = m_locs.find(name);
	if (found != m_locs.end())
	{
		loc = found->second;
	}
	else
	{
		loc = glGetUniformLocation(m_id, name.c_str());
		m_locs[name] = loc;
	}

	return loc;
}

void Shader::Dispatch(unsigned int numGroupX, unsigned int numGroupY, unsigned int numGroupZ)
{
	glDispatchCompute(numGroupX, numGroupY, numGroupZ);
}
