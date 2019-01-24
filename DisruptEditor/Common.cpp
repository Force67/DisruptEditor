#include "Common.h"

#include "Vector.h"
#include "Hash.h"
#include <unordered_map>
#include <SDL_assert.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <SDL_log.h>
#include <fstream>
#include "NBCF.h"
#include "xbgFile.h"
#include "materialFile.h"
#include "xbtFile.h"
#include "tinyfiles.h"
#include "stb_image.h"
#include "FileHandler.h"
#include "Serialization.h"
#include <SDL_messagebox.h>

Settings settings;
std::unordered_map<std::string, materialFile> materials;
std::unordered_map<std::string, xbtFile> textures;

void reloadSettings() {
	std::string contents = readFile("settings.json");
	unserializeFromJSON(settings, contents.c_str());
	if (settings.searchPaths.empty() || settings.patchDir.empty()) {
		//Fill with sample files
		saveSettings();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Disrupt Editor is not configured", "You must first setup Disrupt Editor by editing settings.json\nPlease see the readme for more details.", NULL);
		exit(0);
	}

	//Check to make sure searchPaths have trailing slash
	for (std::string &path : settings.searchPaths) {
		if (path.back() != '/' && path.back() != '\\')
			path.push_back('/');
	}

	if (settings.patchDir.back() != '/' && settings.patchDir.back() != '\\')
		settings.patchDir.push_back('/');
}

void saveSettings() {
	bool ret = writeFile("settings.json", serializeToJSON(settings));
	SDL_assert_release(ret);
}

std::string readFile(const std::string & file) {
	std::ifstream t(file);
	if (!t.is_open())
		return std::string();
	return std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
}

bool writeFile(const std::string & file, const std::string &contents) {
	SDL_RWops *fp = SDL_RWFromFile(file.c_str(), "w");
	if (fp) {
		SDL_RWwrite(fp, contents.c_str(), 1, contents.size());
		SDL_RWclose(fp);
		return true;
	}
	return false;
}

std::unordered_map<uint32_t, xbgFile> xbgs;

xbgFile& loadXBG(const std::string &path) {
	return loadXBG(Hash::instance().getFilenameHash(path));
}

xbgFile &loadXBG(uint32_t path) {
	if (xbgs.count(path) == 0) {
		auto &model = xbgs[path];
		SDL_Log("Loading %u.xbg\n", path);
		SDL_RWops *fp = FH::openFile(path);
		if (fp)
			model.open(fp);
	}
	return xbgs[path];
}

materialFile &loadMaterial(const std::string & path) {
	if (materials.count(path) == 0) {
		auto &model = materials[path];
		SDL_Log("Loading %s...\n", path.c_str());
		SDL_RWops *fp = FH::openFile(path);
		if (fp)
			model.open(fp);
	}
	return materials[path];
}

xbtFile & loadTexture(const std::string & path) {
	if (textures.count(path) == 0) {
		auto &model = textures[path];
		SDL_Log("Loading %s...\n", path.c_str());
		SDL_RWops *fp = FH::openFile(path);
		if (fp)
			model.open(fp);
	}
	return textures[path];
}

std::unordered_map<std::string, GLuint> texturesRes;
GLuint loadResTexture(const std::string &path) {
	if (texturesRes.count(path) == 0) {
		int width, height, bpc;
		uint8_t *pixels = stbi_load(("res/" + path).c_str(), &width, &height, &bpc, 0);
		if (!pixels) return 0;
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		switch (bpc) {
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
				break;
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
				break;
			case 2:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
				break;
			case 1:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
				break;
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		texturesRes[path] = id;
		free(pixels);
		return id;
	}
	return texturesRes[path];
}

void Settings::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(searchPaths);
	REGISTER_MEMBER(patchDir);

	REGISTER_MEMBER(textDrawDistance);

	REGISTER_MEMBER(windowSize);
	REGISTER_MEMBER(maximized);
	REGISTER_MEMBER(openWindows);

	REGISTER_MEMBER(keyForward);
	REGISTER_MEMBER(keyBackward);
	REGISTER_MEMBER(keyLeft);
	REGISTER_MEMBER(keyRight);
	REGISTER_MEMBER(keyAscend);
	REGISTER_MEMBER(keyDescend);
	REGISTER_MEMBER(keyFast);
	REGISTER_MEMBER(keySlow);
}
