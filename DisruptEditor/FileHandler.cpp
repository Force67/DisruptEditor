#include "FileHandler.h"
#include "Common.h"
#include "Hash.h"
#include "tinyfiles.h"
#include <Shlwapi.h>
#include "DB.h"

Vector<FileInfo> FH::getFileList(const std::string &dir, const std::string &extFilter) {
	std::map<std::string, tfFILE> files;

	for (const std::string &base : settings.searchPaths) {
		std::string fullPath = base + dir;
		if (!PathFileExistsA(fullPath.c_str())) continue;

		tfDIR dir;
		tfDirOpen(&dir, fullPath.c_str());
		while (dir.has_next) {
			tfFILE file;
			tfReadFile(&dir, &file);

			if (!file.is_dir && files.count(file.name) == 0) {
				if (extFilter.empty() || file.ext == extFilter)
					files[file.name] = file;
			}

			tfDirNext(&dir);
		}
		tfDirClose(&dir);
	}

	Vector<FileInfo> outFiles;
	for (auto &file : files) {
		FileInfo fi;
		fi.name = file.second.name;
		fi.fullPath = file.second.path;
		fi.ext = file.second.ext;
		outFiles.push_back(fi);
	}
	return outFiles;
}

Vector<FileInfo> FH::getFileListFromAbsDir(const std::string & fullDir, const std::string & extFilter) {
	Vector<FileInfo> outFiles;
	if (!PathFileExistsA(fullDir.c_str())) return outFiles;

	tfDIR dir;
	tfDirOpen(&dir, fullDir.c_str());
	while (dir.has_next) {
		tfFILE file;
		tfReadFile(&dir, &file);

		if (!file.is_dir) {
			if (extFilter.empty() || file.ext == extFilter) {
				FileInfo fi;
				fi.name = file.name;
				fi.fullPath = file.path;
				fi.ext = file.ext;
				outFiles.push_back(fi);
			}
		}

		tfDirNext(&dir);
	}
	tfDirClose(&dir);

	return outFiles;
}

SDL_RWops * FH::openFile(const std::string & path) {
	SDL_RWops *fp = SDL_RWFromFile(getAbsoluteFilePath(path).c_str(), "rb");
	return fp;
}

SDL_RWops * FH::openFile(uint32_t path) {
	SDL_RWops *fp = SDL_RWFromFile(getAbsoluteFilePath(path).c_str(), "rb");
	return fp;
}

std::string FH::getReverseFilename(uint32_t hash) {
	auto it = DB::instance().getFileByHash(hash);
	if (!it) {
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "_%08x", hash);
		return std::string(buffer);
	}

	return it->path;
}

std::string FH::getAbsoluteFilePath(const std::string &path) {
	std::string fullPath = settings.patchDir + path;
	if (PathFileExistsA(fullPath.c_str()))
		return fullPath;
	for (const std::string &base : settings.searchPaths) {
		fullPath = base + path;
		if (PathFileExistsA(fullPath.c_str()))
			return fullPath;
	}

	//Search Unknown Files
	uint32_t hash = Hash::getFilenameHash(path);

	return std::string();
}

std::string FH::getAbsoluteFilePath(uint32_t path) {
	auto it = DB::instance().getFileByHash(path);
	if (it)
		return getAbsoluteFilePath(it->path);

	return "";
}

void FH::Init() {
}

void seekpad(SDL_RWops *fp, long pad) {
	//16-byte chunk alignment
	long size = SDL_RWtell(fp);
	long seek = (pad - (size % pad)) % pad;
	SDL_RWseek(fp, seek, RW_SEEK_CUR);
}

void writepad(SDL_RWops *fp, long pad) {
	char padding[32] = { 0 };
	//16-byte chunk alignment
	long size = SDL_RWtell(fp);
	long seek = (pad - (size % pad)) % pad;
	SDL_RWwrite(fp, padding, 1, seek);
}
