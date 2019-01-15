#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL_assert.h>
#include <new>
#include <vector>

template<typename T> using Vector = std::vector<T>;

template<typename T>
inline static void appendBinary(Vector<T> &buf, const void * begin, const void * end) {
	size_t datalen = (uint8_t*)end - (uint8_t*)begin;
	buf.resize(buf.size() + datalen);
	memcpy((uint8_t*)buf.end()._Ptr - datalen, begin, datalen);
}
