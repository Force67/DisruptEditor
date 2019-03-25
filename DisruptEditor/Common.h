#pragma once

#include "Vector.h"
#include <string>
#include <map>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glad.h"
#include <SDL_rwops.h>
#include <SDL_keycode.h>

class xbgFile;
class materialFile;
class xbtFile;
class Node;
class MemberStructure;

typedef uint32_t SDL_Scancode_V;
typedef uint32_t SDL_Keymod_V;

struct Settings {
	//Filesystem Settings
	Vector<std::string> searchPaths;
	std::string patchDir;

	//Graphics Settings
	float textDrawDistance = 5.f;
	std::map<std::string, bool> displayComponents;
	bool displayNear = true;
	
	// Window Settings
	glm::ivec2 windowSize = glm::ivec2(1600, 900);
	bool maximized = false;
	std::map<std::string, bool> openWindows;

	//Camera Controls
	SDL_Scancode_V keyForward = SDL_SCANCODE_W;
	SDL_Scancode_V keyBackward = SDL_SCANCODE_S;
	SDL_Scancode_V keyLeft = SDL_SCANCODE_A;
	SDL_Scancode_V keyRight = SDL_SCANCODE_D;
	SDL_Scancode_V keyAscend = SDL_SCANCODE_R;
	SDL_Scancode_V keyDescend = SDL_SCANCODE_F;
	SDL_Keymod_V keyFast = KMOD_LSHIFT;
	SDL_Keymod_V keySlow = KMOD_LCTRL;
	float flyMultiplier = 1.f;

	void registerMembers(MemberStructure &ms);
};

extern Settings settings;

void reloadSettings();
void saveSettings();

std::string readFile(const std::string &file);
bool writeFile(const std::string &file, const std::string &contents);

xbgFile& loadXBG(const std::string &path);
xbgFile& loadXBG(uint32_t path);

materialFile& loadMaterial(const std::string &path);

xbtFile& loadTexture(const std::string &path);

GLuint loadResTexture(const std::string &path);

#include "imgui.h"

namespace ImGui {
	static bool InputUInt64(const char * label, uint64_t * v) {
		std::string a = std::to_string(*v);
		char temp[36];
		strncpy(temp, a.c_str(), sizeof(temp));

		if (ImGui::InputText(label, temp, sizeof(temp), ImGuiInputTextFlags_CharsDecimal)) {
			*v = std::stoull(temp);
			return true;
		}

		return false;
	}
};
