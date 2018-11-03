#include "Common.h"

#include "Vector.h"
#include "Hash.h"
#include <unordered_map>
#include <SDL_assert.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <SDL_log.h>
#include "NBCF.h"
#include "xbgFile.h"
#include "materialFile.h"
#include "xbtFile.h"
#include "tinyfiles.h"
#include "stb_image.h"
#include "FileHandler.h"

Settings settings;
std::unordered_map<std::string, materialFile> materials;
std::unordered_map<std::string, xbtFile> textures;
std::unordered_map<uint32_t, std::string> knownFiles;

void reloadSettings() {
	tinyxml2::XMLDocument doc;
	doc.LoadFile("settings.xml");

	//Search Path
	settings.searchPaths.clear();

	if (doc.RootElement()->FirstChildElement("patchDir")) {
		settings.patchDir = doc.RootElement()->FirstChildElement("patchDir")->Attribute("src");
		if (settings.patchDir.back() != '/' && settings.patchDir.back() != '\\')
			settings.patchDir.push_back('/');
		settings.searchPaths.push_back(settings.patchDir);
	}

	for (auto it = doc.RootElement()->FirstChildElement("PackFolder"); it; it = it->NextSiblingElement("PackFolder")) {
		std::string packPath = it->Attribute("src");
		if (packPath.back() != '/' && packPath.back() != '\\')
			packPath.push_back('/');
		settings.searchPaths.push_back(packPath);
	}

	if (doc.RootElement()->FirstChildElement("textDrawDistance"))
		settings.textDrawDistance = doc.RootElement()->FirstChildElement("textDrawDistance")->FloatAttribute("src");

	if (doc.RootElement()->FirstChildElement("drawBuildings"))
		settings.drawBuildings = doc.RootElement()->FirstChildElement("drawBuildings")->BoolAttribute("src");

	//Reload Filelist
	knownFiles.clear();
	FILE* fp = fopen("res/Watch Dogs.filelist", "r");
	char buffer[500];
	while(fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer)-1] = '\0';
		knownFiles[Hash::instance().getFilenameHash(buffer)] = buffer;
	}
	fclose(fp);
}

template <typename T>
void pushXMLSetting(tinyxml2::XMLPrinter &printer, const char *name, const T &value) {
	printer.OpenElement(name);
	printer.PushAttribute("src", value);
	printer.CloseElement();
}

template <>
void pushXMLSetting<std::string>(tinyxml2::XMLPrinter &printer, const char *name, const std::string &value) {
	printer.OpenElement(name);
	printer.PushAttribute("src", value.c_str());
	printer.CloseElement();
}

#define pushXMLSetting(p, x) pushXMLSetting(p, #x, settings.x)

void saveSettings() {
	FILE *fp = fopen("settings.xml", "w");
	tinyxml2::XMLPrinter printer(fp);
	printer.OpenElement("Settings");

	for (const std::string &base : settings.searchPaths) {
		if (base == settings.patchDir) continue;

		printer.OpenElement("PackFolder");
		printer.PushAttribute("src", base.c_str());
		printer.CloseElement();
	}

	pushXMLSetting(printer, patchDir);
	pushXMLSetting(printer, textDrawDistance);
	pushXMLSetting(printer, drawBuildings);

	printer.CloseElement();
	fclose(fp);
}

std::string loadFile(const std::string & file) {
	FILE* fp = fopen(file.c_str(), "r");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char data[1024 * 5];
		SDL_assert_release(size < sizeof(data) - 1);
		fread(data, 1, size, fp);
		data[size] = '\0';
		fclose(fp);
		return data;
	}
	return std::string();
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
		model.open(fp);
		SDL_RWclose(fp);
	}
	return xbgs[path];
}

materialFile &loadMaterial(const std::string & path) {
	if (materials.count(path) == 0) {
		auto &model = materials[path];
		SDL_Log("Loading %s...\n", path.c_str());
		SDL_RWops *fp = FH::openFile(path);
		model.open(fp);
		SDL_RWclose(fp);
	}
	return materials[path];
}

xbtFile & loadTexture(const std::string & path) {
	if (textures.count(path) == 0) {
		auto &model = textures[path];
		SDL_Log("Loading %s...\n", path.c_str());
		SDL_RWops *fp = FH::openFile(path);
		model.open(fp);
		SDL_RWclose(fp);
	}
	return textures[path];
}

std::unordered_map<std::string, GLuint> texturesRes;
GLuint loadResTexture(const std::string & path) {
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

