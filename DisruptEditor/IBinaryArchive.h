#pragma once

#include <SDL_rwops.h>
#include "glm/glm.hpp"
#include <string>
#include "Vector.h"

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
	virtual void serialize(glm::mat4& value) = 0;
	virtual void serialize(std::string& value) = 0;
	virtual bool isReading() const = 0;
	virtual void pad(size_t padding) = 0;
	virtual size_t size() = 0;
	virtual size_t tell() = 0;
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount) = 0;

	template<typename T>
	void serializeNdVectorExternal(Vector<T>& vec);

	template<typename T>
	void serializeNdVector(Vector<T>& vec, uint32_t typeId, uint32_t& unk);

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
	virtual void serialize(glm::mat4& value);
	virtual void serialize(std::string& value);
	virtual bool isReading() const;
	virtual void pad(size_t padding);
	virtual size_t size();
	virtual size_t tell();
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount);
};

class CBinaryArchiveWriter : public IBinaryArchive {
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
	virtual void serialize(glm::mat4& value);
	virtual void serialize(std::string& value);
	virtual bool isReading() const;
	virtual void pad(size_t padding);
	virtual size_t size();
	virtual size_t tell();
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount);
};

template<typename T>
inline void IBinaryArchive::serializeNdVectorExternal(Vector<T>& vec) {
	if (isReading()) {
		uint32_t count;
		serialize(count);
		vec.resize(count);
		for (uint32_t i = 0; i < count; ++i)
			vec[i].read(*this);
	}
	else {
		uint32_t count = vec.size();
		for (uint32_t i = 0; i < count; ++i)
			vec[i].read(*this);
	}
}

template<typename T>
inline void IBinaryArchive::serializeNdVector(Vector<T>& vec, uint32_t typeId, uint32_t &unk) {
	if (isReading()) {
		uint32_t counter, counter2;
		serialize(counter);

		uint32_t unknownTypeID;
		serialize(unknownTypeID);
		SDL_assert_release(unknownTypeID == typeId);

		serialize(unk);
		serialize(counter2);
		SDL_assert_release(counter == counter2);

		vec.resize(counter);
		for (uint32_t i = 0; i < counter; ++i)
			vec[i].read(*this);
	}
	else {
		//TODO
	}
}
