#include "Serialization.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

template <typename T>
void displayImGui(const char* name, T &value) {
	ImGui::PushID(&value);
	ImGui::AlignTextToFramePadding();
	ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "%s", name);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	char buffer[60];
	/*std::to_string(buffer, sizeof(buffer), value);
	if (ImGui::InputText(name, buffer, sizeof(buffer)))
		value = std::string<T>(buffer);*/
	ImGui::NextColumn();
	ImGui::PopID();
}

void displayImGui(const char* name, bool &value) {
	ImGui::PushID(&value);
	ImGui::Checkbox(name, &value);
	ImGui::PopID();
}

void displayImGui(const char* name, int &value) {
	ImGui::PushID(&value);
	ImGui::InputInt(name, &value, 1);
	ImGui::PopID();
}

void displayImGui(const char* name, float &value) {
	ImGui::PushID(&value);
	ImGui::InputFloat(name, &value, 1.f);
	ImGui::PopID();
}

void displayImGui(const char* name, double &value) {
	ImGui::PushID(&value);
	ImGui::InputDouble(name, &value, 1.f);
	ImGui::PopID();
}

//////////////String Serialization

template <>
void toString<bool>(char *buf, size_t bufSize, bool value) {
	assert(bufSize > 4);
	strncpy(buf, value ? "true" : "false", bufSize);
}

template <>
void toString<uint8_t>(char *buf, size_t bufSize, uint8_t value) {
	snprintf(buf, (int)bufSize, "%i", value);
}

template <>
void toString<int8_t>(char *buf, size_t bufSize, int8_t value) {
	snprintf(buf, (int)bufSize, "%i", value);
}

template <>
void toString<uint16_t>(char *buf, size_t bufSize, uint16_t value) {
	snprintf(buf, (int)bufSize, "%i", value);
}

template <>
void toString<int16_t>(char *buf, size_t bufSize, int16_t value) {
	snprintf(buf, (int)bufSize, "%i", value);
}

template <>
void toString<uint32_t>(char *buf, size_t bufSize, uint32_t value) {
	snprintf(buf, (int)bufSize, "%u", value);
}

template <>
void toString<int32_t>(char *buf, size_t bufSize, int32_t value) {
	snprintf(buf, (int)bufSize, "%i", value);
}

template <>
void toString<uint64_t>(char *buf, size_t bufSize, uint64_t value) {
	snprintf(buf, (int)bufSize, "%lld", value);
}

template <>
void toString<int64_t>(char *buf, size_t bufSize, int64_t value) {
	snprintf(buf, (int)bufSize, "%lld", value);
}

template <>
void toString<float>(char *buf, size_t bufSize, float value) {
	snprintf(buf, (int)bufSize, "%f", value);
}

template <>
void toString<double>(char *buf, size_t bufSize, double value) {
	snprintf(buf, (int)bufSize, "%f", value);
}

template <>
void toString<glm::vec2>(char *buf, size_t bufSize, glm::vec2 value) {
	snprintf(buf, (int)bufSize, "%f,%f", value.x, value.y);
}

template <>
void toString<glm::ivec2>(char *buf, size_t bufSize, glm::ivec2 value) {
	snprintf(buf, (int)bufSize, "%i,%i", value.x, value.y);
}

template <>
void toString<glm::vec3>(char *buf, size_t bufSize, glm::vec3 value) {
	snprintf(buf, (int)bufSize, "%f,%f,%f", value.x, value.y, value.z);
}

template <>
void toString<glm::vec4>(char *buf, size_t bufSize, glm::vec4 value) {
	snprintf(buf, (int)bufSize, "%f,%f,%f,%f", value.x, value.y, value.z, value.w);
}

template <>
void toString<glm::quat>(char* buf, size_t bufSize, glm::quat value) {
	snprintf(buf, (int)bufSize, "%f,%f,%f,%f", value.x, value.y, value.z, value.w);
}

template <>
void toString<glm::mat4>(char *buf, size_t bufSize, glm::mat4 value) {
	snprintf(buf, (int)bufSize, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", value[0][0], value[0][1], value[0][2], value[0][3], value[1][0], value[1][1], value[1][2], value[1][3], value[2][0], value[2][1], value[2][2], value[2][3], value[3][0], value[3][1], value[3][2], value[3][3]);
}

template <>
void toString<std::string>(char *buf, size_t bufSize, std::string value) {
	strncpy(buf, value.c_str(), bufSize);
}

template <>
void toString<const char*>(char *buf, size_t bufSize, const char* value) {
	strncpy(buf, value, bufSize);
}

template <>
void fromString<const char*>(const char *buf, const char* &value) {
	value = buf;
}

template <>
void fromString<bool>(const char *buf, bool& value) {
	value = buf[0] == 't' || buf[0] == '1';
}

template <>
void fromString<uint8_t>(const char *buf, uint8_t &value) {
	value = atoi(buf);
}

template <>
void fromString<int8_t>(const char *buf, int8_t &value) {
	value = atoi(buf);
}

template <>
void fromString<uint16_t>(const char *buf, uint16_t &value) {
	value = atoi(buf);
}

template <>
void fromString<int16_t>(const char *buf, int16_t &value) {
	value = atoi(buf);
}

template <>
void fromString<uint32_t>(const char *buf, uint32_t &value) {
	value = atoll(buf);
}

template <>
void fromString<int32_t>(const char *buf, int32_t &value) {
	value = atoi(buf);
}

template <>
void fromString<uint64_t>(const char *buf, uint64_t &value) {
	value = atoll(buf);
}

template <>
void fromString<int64_t>(const char *buf, int64_t &value) {
	value = atoll(buf);
}

template <>
void fromString<float>(const char *buf, float &value) {
	value = atof(buf);
}

template <>
void fromString<double>(const char *buf, double &value) {
	value = atof(buf);
}

template <>
void fromString<glm::vec2>(const char *buf, glm::vec2 &value) {
	sscanf(buf, "%f,%f", &value.x, &value.y);
}

template <>
void fromString<glm::ivec2>(const char *buf, glm::ivec2 &value) {
	sscanf(buf, "%i,%i", &value.x, &value.y);
}

template <>
void fromString<glm::vec3>(const char *buf, glm::vec3 &value) {
	sscanf(buf, "%f,%f,%f", &value.x, &value.y, &value.z);
}

template <>
void fromString<glm::vec4>(const char *buf, glm::vec4 &value) {
	sscanf(buf, "%f,%f,%f,%f", &value.x, &value.y, &value.z, &value.w);
}

template <>
void fromString<glm::quat>(const char* buf, glm::quat &value) {
	sscanf(buf, "%f,%f,%f,%f", &value.x, &value.y, &value.z, &value.w);
}

template <>
void fromString<glm::mat4>(const char *buf, glm::mat4 &value) {
	sscanf(buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &value[0][0], &value[0][1], &value[0][2], &value[0][3], &value[1][0], &value[1][1], &value[1][2], &value[1][3], &value[2][0], &value[2][1], &value[2][2], &value[2][3], &value[3][0], &value[3][1], &value[3][2], &value[3][3]);
}

template <>
void fromString<std::string>(const char *buf, std::string &value) {
	if(buf)
		value = buf;
}



#undef REG_MEMBER
#define REG_MEMBER(x) \
void MemberStructure::registerMember(const char *name, x &value) {      \
	switch (type) {                                                     \
		case TOXML:                                                     \
		{                                                               \
			if(name)                                                    \
				printer->OpenElement(name);                             \
			printer->PushText(toString(value).c_str());                 \
			if(name)                                                    \
				printer->CloseElement();                                \
			return;                                                     \
		}                                                               \
		case FROMXML:                                                   \
		{                                                               \
			const tinyxml2::XMLElement *e;                              \
			if(name)                                                    \
				e = it->FirstChildElement(name);                        \
			else                                                        \
				e = it;                                                 \
			if(e)                                                       \
				fromString< x >(e->GetText(), value);                   \
			return;                                                     \
		}                                                               \
		case IMGUI:                                                     \
		{                                                               \
			displayImGui(name, value);                                  \
			return;                                                     \
		}                                                               \
	}                                                                   \
}
REG_MEMBER(bool)
REG_MEMBER(uint8_t)
REG_MEMBER(int8_t)
REG_MEMBER(uint16_t)
REG_MEMBER(int16_t)
REG_MEMBER(uint32_t)
REG_MEMBER(int32_t)
REG_MEMBER(uint64_t)
REG_MEMBER(int64_t)
REG_MEMBER(float)
REG_MEMBER(double)
REG_MEMBER(glm::vec2)
REG_MEMBER(glm::ivec2)
REG_MEMBER(glm::vec3)
REG_MEMBER(glm::vec4)
REG_MEMBER(glm::quat)
REG_MEMBER(glm::mat4)
REG_MEMBER(std::string)

size_t XMLNumChildren(tinyxml2::XMLElement* it) {
	size_t count = 0;
	it = it->FirstChildElement();
	while (it) {
		++count;
		it = it->NextSiblingElement();
	}
	return count;
}
