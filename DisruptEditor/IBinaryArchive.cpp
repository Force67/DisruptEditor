#include "IBinaryArchive.h"
#include "FileHandler.h"

IBinaryArchive::IBinaryArchive() {
	fp = NULL;
}

void CBinaryArchiveReader::serialize(bool& value) {
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(uint8_t& value) {
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(int8_t& value) {
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(uint16_t& value) {
	pad(2);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(int16_t& value) {
	pad(2);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(uint32_t& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(int32_t& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(uint64_t& value) {
	pad(8);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(int64_t& value) {
	pad(8);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(float& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(double& value) {
	pad(8);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(glm::vec2& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(glm::vec3& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(glm::vec4& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(glm::mat4& value) {
	pad(4);
	SDL_RWread(fp, &value, sizeof(value), 1);
}

void CBinaryArchiveReader::serialize(std::string& value) {
	uint32_t len;
	serialize(len);
	value.resize(len + 1, '\0');
	memBlock(&value[0], 1, len);
}

bool CBinaryArchiveReader::isReading() const {
	return true;
}

void CBinaryArchiveReader::pad(size_t padding) {
	seekpad(fp, padding);
}

size_t CBinaryArchiveReader::size() {
	return SDL_RWsize(fp);
}

size_t CBinaryArchiveReader::tell() {
	return SDL_RWtell(fp);
}

void CBinaryArchiveReader::memBlock(void* ptr, size_t objSize, size_t objCount) {
	SDL_RWread(fp, ptr, objSize, objCount);
}
