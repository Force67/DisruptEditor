#pragma once

#include <SDL_rwops.h>
#include "glm/glm.hpp"
#include <string>

class IBinaryArchive {
public:
	IBinaryArchive();

	virtual void serialize(bool& value) = 0;
	virtual void serialize(uint8_t& value) = 0;
	virtual void serialize(int8_t& value) = 0;
	virtual void serialize(uint16_t& value) = 0;
	virtual void serialize(int16_t& value) = 0;
	virtual void serialize(uint32_t& value) = 0;
	virtual void serialize(int32_t& value) = 0;
	virtual void serialize(uint64_t& value) = 0;
	virtual void serialize(int64_t& value) = 0;
	virtual void serialize(float& value) = 0;
	virtual void serialize(double& value) = 0;
	virtual void serialize(glm::vec2& value) = 0;
	virtual void serialize(glm::vec3& value) = 0;
	virtual void serialize(glm::vec4& value) = 0;
	virtual void serialize(std::string& value) = 0;
	virtual bool isReading() const = 0;
	virtual void pad(size_t padding) = 0;
	virtual size_t size() = 0;
	virtual size_t tell() = 0;
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount) = 0;

	SDL_RWops* fp;
	bool bigEndian = false;
};


class CBinaryArchiveReader : public IBinaryArchive {
public:
	virtual void serialize(bool& value);
	virtual void serialize(uint8_t& value);
	virtual void serialize(int8_t& value);
	virtual void serialize(uint16_t& value);
	virtual void serialize(int16_t& value);
	virtual void serialize(uint32_t& value);
	virtual void serialize(int32_t& value);
	virtual void serialize(uint64_t& value);
	virtual void serialize(int64_t& value);
	virtual void serialize(float& value);
	virtual void serialize(double& value);
	virtual void serialize(glm::vec2& value);
	virtual void serialize(glm::vec3& value);
	virtual void serialize(glm::vec4& value);
	virtual void serialize(std::string& value);
	virtual bool isReading() const;
	virtual void pad(size_t padding);
	virtual size_t size();
	virtual size_t tell();
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount);
};
