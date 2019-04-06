#pragma once

#include <SDL_rwops.h>
#include "glm/glm.hpp"
#include <string>
#include "Vector.h"

class IBinaryArchive {
public:
	IBinaryArchive();

	void serialize(bool& value);
	void serialize(uint8_t& value);
	void serialize(int8_t& value);
	void serialize(uint16_t& value);
	void serialize(int16_t& value);
	void serialize(uint32_t& value);
	void serialize(int32_t& value);
	void serialize(uint64_t& value);
	void serialize(int64_t& value);
	void serialize(float& value);
	void serialize(double& value);
	void serialize(glm::vec2& value);
	void serialize(glm::vec3& value);
	void serialize(glm::vec4& value);
	void serialize(glm::mat4& value);
	void serialize(std::string& value);
	virtual bool isReading() const = 0;
	virtual void pad(size_t padding) = 0;
	size_t size();
	size_t tell();
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount) = 0;

	template<typename T>
	void serializeNdVectorExternal(Vector<T>& vec);

	template<typename T>
	void serializeNdVectorExternal_pod(Vector<T>& vec);

	template<typename T>
	void serializeNdVector(Vector<T>& vec, uint32_t typeId, uint32_t& unk);

	template<typename T>
	void serialize(T &value);

	SDL_RWops* fp;
	bool bigEndian = false;
	enum PaddingType { PADDING_NONE, PADDING_IBINARYARCHIVE, PADDING_GEAR
	};
	PaddingType padding = PADDING_IBINARYARCHIVE;
};


class CBinaryArchiveReader : public IBinaryArchive {
public:
	CBinaryArchiveReader(SDL_RWops* _fp);

	virtual bool isReading() const;
	virtual void pad(size_t padding);
	virtual void memBlock(void* ptr, size_t objSize, size_t objCount);
};

class CBinaryArchiveWriter : public IBinaryArchive {
public:
	CBinaryArchiveWriter(SDL_RWops* _fp);

	virtual bool isReading() const;
	virtual void pad(size_t padding);
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
		uint32_t count = (uint32_t)vec.size();
		serialize(count);
		for (uint32_t i = 0; i < count; ++i)
			vec[i].read(*this);
	}
}

template<typename T>
inline void IBinaryArchive::serializeNdVectorExternal_pod(Vector<T>& vec) {
	if (isReading()) {
		uint32_t count;
		serialize(count);
		vec.resize(count);
		for (uint32_t i = 0; i < count; ++i)
			serialize(vec[i]);
	}
	else {
		uint32_t count = (uint32_t)vec.size();
		serialize(count);
		for (uint32_t i = 0; i < count; ++i)
			serialize(vec[i]);
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

template<typename T>
inline void IBinaryArchive::serialize(T & value) {
	value.read(*this);
}
