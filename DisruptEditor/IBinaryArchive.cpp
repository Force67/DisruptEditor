#include "IBinaryArchive.h"
#include "FileHandler.h"

template <typename T>
static void serializePOD(IBinaryArchive &fp, T &value) {
	if (fp.bigEndian) {
		uint8_t *it = (uint8_t*)(&value + 1) - 1;
		while (it >= (uint8_t*)&value) {
			fp.memBlock(it, 1, 1);
			--it;
		}
	} else {
		fp.memBlock(&value, sizeof(value), 1);
	}
}

IBinaryArchive::IBinaryArchive() {
	fp = NULL;
}

void IBinaryArchive::serialize(bool& value) {
	if (padding == PADDING_GEAR)
		pad(4);
	serializePOD(*this, value);
	SDL_assert_release(value == 0 || value == 1);
	if (padding == PADDING_GEAR)
		pad(4);
}

void IBinaryArchive::serialize(uint8_t& value) {
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(int8_t& value) {
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(uint16_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(2);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(int16_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(2);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(uint32_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(4);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(int32_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(4);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(uint64_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(8);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(int64_t& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(8);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(float& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(4);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(double& value) {
	if (padding == PADDING_IBINARYARCHIVE || padding == PADDING_GEAR)
		pad(8);
	serializePOD(*this, value);
}

void IBinaryArchive::serialize(glm::vec2& value) {
	serialize(value.x);
	serialize(value.y);
}

void IBinaryArchive::serialize(glm::vec3& value) {
	serialize(value.x);
	serialize(value.y);
	serialize(value.z);
}

void IBinaryArchive::serialize(glm::vec4& value) {
	serialize(value.x);
	serialize(value.y);
	serialize(value.z);
	serialize(value.w);
}

void IBinaryArchive::serialize(glm::mat4& value) {
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			serialize(value[x][y]);
}

void IBinaryArchive::serialize(std::string& value) {
	uint32_t len = (uint32_t)value.size();
	serialize(len);
	value.resize(len + 1, '\0');
	memBlock(value.data(), 1, len);
}

size_t IBinaryArchive::size() {
	return SDL_RWsize(fp);
}

size_t IBinaryArchive::tell() {
	return SDL_RWtell(fp);
}

CBinaryArchiveReader::CBinaryArchiveReader(SDL_RWops* _fp) {
	fp = _fp;
}

bool CBinaryArchiveReader::isReading() const {
	return true;
}

void CBinaryArchiveReader::pad(size_t padding) {
	Sint64 size = SDL_RWtell(fp);
	Sint64 seek = (padding - (size % padding)) % padding;

	Vector<uint8_t> data(seek);
	memBlock(data.data(), 1, seek);
	for (Sint64 i = 0; i < seek; ++i)
		SDL_assert(data[i] == 0);
	offset = SDL_RWtell(fp);
}

void CBinaryArchiveReader::memBlock(void* ptr, size_t objSize, size_t objCount) {
	size_t ret = SDL_RWread(fp, ptr, objSize, objCount);
	SDL_assert_release(ret == objCount);
	offset = SDL_RWtell(fp);
}

CBinaryArchiveWriter::CBinaryArchiveWriter(SDL_RWops* _fp) {
	fp = _fp;
}

bool CBinaryArchiveWriter::isReading() const {
	return false;
}

void CBinaryArchiveWriter::pad(size_t padding) {
	char temp[64] = { 0 };
	Sint64 size = SDL_RWtell(fp);
	Sint64 seek = (padding - (size % padding)) % padding;
	SDL_RWwrite(fp, temp, 1, seek);
	offset = SDL_RWtell(fp);
}

void CBinaryArchiveWriter::memBlock(void* ptr, size_t objSize, size_t objCount) {
	size_t ret = SDL_RWwrite(fp, ptr, objSize, objCount);
	SDL_assert_release(ret == objCount);
	offset = SDL_RWtell(fp);
}
