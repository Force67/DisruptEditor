#pragma once

#include "glad.h"
#include <map>
#include <string>
#include "Vector.h"
#include <SDL_assert.h>
#include "tinyxml2.h"
#include <memory>

class Shader {
public:
	GLuint oglid;
	inline void use() { glUseProgram(oglid); }
	std::map<std::string, GLint> uniforms;
	enum UniformTypes { INT, FLOAT, MAT4, VEC2, VEC3, VEC4, TEXTURE2D, TEXTURE3D };
	std::map<std::string, std::string> uniformTypes;
	Vector<std::string> samplerAttributes;
};

static inline bool loadShader(Shader *shader, const char * vertex, const char * fragment) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, &vertex, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		Vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		return false;
	}

	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, &fragment, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	if (Result == GL_FALSE) {
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		Vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return false;
	}

	// Link the program
	shader->oglid = glCreateProgram();
	glAttachShader(shader->oglid, VertexShaderID);
	glAttachShader(shader->oglid, FragmentShaderID);

	glLinkProgram(shader->oglid);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	// Check the program
	glGetProgramiv(shader->oglid, GL_LINK_STATUS, &Result);
	if (Result == GL_FALSE) {
		glGetProgramiv(shader->oglid, GL_INFO_LOG_LENGTH, &InfoLogLength);
		Vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shader->oglid, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		glDeleteProgram(shader->oglid);
		return false;
	}

	glUseProgram(shader->oglid);

	for (auto it = shader->uniforms.begin(), end = shader->uniforms.end(); it != end; ++it) {
		it->second = glGetUniformLocation(shader->oglid, it->first.c_str());
	}

	for (int i = 0; i < shader->samplerAttributes.size(); ++i) {
		glUniform1i(shader->uniforms[shader->samplerAttributes[i]], i);
	}

	return true;
}

inline Shader loadShaders(const char *program) {
	Shader shader;

	// Create the shaders
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(program);
	SDL_assert_release(ret == tinyxml2::XMLError::XML_SUCCESS);

	tinyxml2::XMLElement *root = doc.RootElement();

	tinyxml2::XMLElement *uniform = root->FirstChildElement("uniform");
	while (uniform) {
		const char* name = uniform->Attribute("name");
		const char* type = uniform->Attribute("type");

		shader.uniforms[name] = -1;
		shader.uniformTypes[name] = type;
		if (type == std::string("sampler2D") || type == std::string("sampler3D")) {
			shader.samplerAttributes.push_back(name);
		}

		uniform = uniform->NextSiblingElement("uniform");
	}

	loadShader(&shader, root->FirstChildElement("vertex")->GetText(), root->FirstChildElement("fragment")->GetText());

	return shader;
}

enum VertexBufferOptions { BUFFER_STATIC = GL_STATIC_DRAW, BUFFER_DYNAMIC = GL_DYNAMIC_DRAW, BUFFER_STREAM = GL_STREAM_DRAW
};

class VertexBuffer {
public:
	~VertexBuffer() {
		glDeleteBuffers(1, &buffer_id);
	}
	bool loaded() { return buffer_id != -1; }
	void bind() {
		glBindBuffer(target, buffer_id);
	}
	uint32_t buffer_id = -1;
	unsigned long size = 0, maxsize = 0;
	VertexBufferOptions type;
	GLenum target;
};

inline std::shared_ptr<VertexBuffer> createVertexBuffer(const void *data, unsigned long size, VertexBufferOptions type, GLenum target = GL_ARRAY_BUFFER) {
	std::shared_ptr<VertexBuffer> oglvb = std::make_shared<VertexBuffer>();
	glGenBuffers(1, &oglvb->buffer_id);
	oglvb->bind();
	oglvb->type = type;
	oglvb->target = target;
	glBufferData(target, size, data, type);

	oglvb->maxsize = size;
	oglvb->size = size;

	return oglvb;
}

inline void updateVertexBuffer(const void *data, unsigned long size, std::shared_ptr<VertexBuffer> buffer) {
	SDL_assert_release(buffer->loaded());

	buffer->bind();
	if (buffer->maxsize < size) {
		buffer->maxsize = size;
		glBufferData(buffer->target, size, data, buffer->type);
	} else {
		glBufferSubData(buffer->target, 0, size, data);
	}
	buffer->size = size;
}

inline Vector<uint8_t> getVertexBuffer(std::shared_ptr<VertexBuffer> buffer) {
	SDL_assert_release(buffer->loaded());
	buffer->bind();
	Vector<uint8_t> data(buffer->size);
	glGetBufferSubData(buffer->target, 0, buffer->size, data.data());
	return data;
}
