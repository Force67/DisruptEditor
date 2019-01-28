#pragma once
#include <string>
#include "Vector.h"
struct SDL_RWops;

void seekpad(SDL_RWops *fp, long pad);

void writepad(SDL_RWops *fp, long pad);

struct FileInfo {
	std::string fullPath;
	std::string name;
	std::string ext;
};

namespace FH {
	void Init();
	Vector<FileInfo> getFileList(const std::string &dir, const std::string &extFilter = std::string());
	Vector<FileInfo> getFileListFromAbsDir(const std::string &dir, const std::string &extFilter = std::string());

	std::string getAbsoluteFilePath(const std::string &path);
	std::string getAbsoluteFilePath(uint32_t path);

	SDL_RWops* openFile(const std::string &path);
	SDL_RWops* openFile(uint32_t path);

	std::string getReverseFilename(uint32_t hash);
}
