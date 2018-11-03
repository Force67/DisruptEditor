#pragma once

#include "glad.h"
struct SDL_RWops;

class xbtFile {
public:
	bool open(SDL_RWops* fp);
	GLuint id;
};

