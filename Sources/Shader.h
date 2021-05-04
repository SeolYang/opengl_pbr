#pragma once
#include <string>
#include <unordered_map>

#include "glm/matrix.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

class Shader
{
public:
	Shader(const std::string& csPath);
	Shader(const std::string& vsPath, const std::string& fsPath);
	Shader(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath);

	unsigned int GetProgramID() const { return m_id; }

	void Bind();

	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetVec3f(const std::string& name, glm::vec3 value);
	void SetVec4f(const std::string& name, glm::vec4 value);
	void SetMat4f(const std::string& name, glm::mat4 value);

	unsigned int FindLoc(const std::string& name);

	void Dispatch(unsigned int numGroupX, unsigned int numGroupY, unsigned int numGroupZ);

private:
	unsigned int m_id;
	std::unordered_map<std::string, int> m_locs;

};