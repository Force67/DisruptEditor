#pragma once

struct SDL_RWops;

class CSectorHighRes {
public:
	bool open(SDL_RWops* fp);
};

