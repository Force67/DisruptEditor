#include "FileHandler.h"
#include "Common.h"
#include "Hash.h"
#include "tinyfiles.h"
#include <set>
#include <Shlwapi.h>

static std::unordered_map<uint32_t, std::string> knownFiles;
static std::set<std::string> allFiles;

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

std::string FH::getAbsoluteFilePath(const std::string &path) {
	for (const std::string &base : settings.searchPaths) {
		std::string fullPath = base + path;
		if (PathFileExistsA(fullPath.c_str()))
			return fullPath;
	}

	uint32_t hash = Hash::instance().getFilenameHash(path);
	//if (unknownFiles.count(hash))
		//return unknownFiles[hash];

	return std::string();
}

std::string FH::getAbsoluteFilePath(uint32_t path) {
	//if (unknownFiles.count(path))
		//return unknownFiles[path];
	if (knownFiles.count(path))
		return getAbsoluteFilePath(knownFiles[path]);
	return "";
}

void FH::Init() {
	knownFiles.clear();
	allFiles.clear();
	FILE* fp = fopen("res/Watch Dogs.filelist", "r");
	char buffer[500];
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		knownFiles[Hash::instance().getFilenameHash(buffer)] = buffer;
	}
	fclose(fp);
}
