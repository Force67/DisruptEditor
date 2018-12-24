#pragma once

struct SDL_RWops;

class batchFile {
public:
	bool open(SDL_RWops *fp);
};

