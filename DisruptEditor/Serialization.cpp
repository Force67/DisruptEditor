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

#undef REG_MEMBER
#define REG_MEMBER(x, f) \
void MemberStructure::registerMember(const char *name, x &value) {      \
	switch (type) {                                                     \
		case TOJSON:                                                    \
		{                                                               \
			*getRef(name, it, doc, true) = value;                       \
			return;                                                     \
		}                                                               \
		case FROMJSON:                                                  \
		{                                                               \
			rapidjson::Value *ref = getRef(name, it, doc, false);       \
			if (!ref) return;                                           \
			value = ref->f();                                           \
			return;                                                     \
		}                                                               \
		case IMGUI:                                                     \
		{                                                               \
			displayImGui(name, value);                                  \
			return;                                                     \
		}                                                               \
	}                                                                   \
}
REG_MEMBER(bool, GetBool)
REG_MEMBER(uint8_t, GetInt)
REG_MEMBER(int8_t, GetInt)
REG_MEMBER(uint16_t, GetInt)
REG_MEMBER(int16_t, GetInt)
REG_MEMBER(uint32_t, GetUint)
REG_MEMBER(int32_t, GetInt)
REG_MEMBER(uint64_t, GetUint64)
REG_MEMBER(int64_t, GetInt64)
REG_MEMBER(float, GetFloat)
REG_MEMBER(double, GetDouble)

void MemberStructure::registerMember(const char *name, glm::vec2 &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray().PushBack(rapidjson::Value(value.x).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.y).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			value.x = (*ref)[0].GetFloat();
			value.y = (*ref)[1].GetFloat();
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::InputFloat2(name, &value.x);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, glm::ivec2 &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray().PushBack(rapidjson::Value(value.x).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.y).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			value.x = (*ref)[0].GetInt();
			value.y = (*ref)[1].GetInt();
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::InputInt2(name, &value.x);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, glm::vec3 &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray().PushBack(rapidjson::Value(value.x).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.y).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.z).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			value.x = (*ref)[0].GetFloat();
			value.y = (*ref)[1].GetFloat();
			value.z = (*ref)[2].GetFloat();
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::InputFloat3(name, &value.x);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, glm::vec4 &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray().PushBack(rapidjson::Value(value.x).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.y).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.z).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.w).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			value.x = (*ref)[0].GetFloat();
			value.y = (*ref)[1].GetFloat();
			value.z = (*ref)[2].GetFloat();
			value.w = (*ref)[3].GetFloat();
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::InputFloat4(name, &value.x);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, glm::quat &value) {
	switch (type) {
		case TOJSON: {
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray().PushBack(rapidjson::Value(value.x).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.y).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.z).Move(), doc->GetAllocator()).PushBack(rapidjson::Value(value.w).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON: {
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			if (ref->Size() != 4) return;
			value.x = (*ref)[0].GetFloat();
			value.y = (*ref)[1].GetFloat();
			value.z = (*ref)[2].GetFloat();
			value.w = (*ref)[3].GetFloat();
			return;
		}
		case IMGUI: {
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::InputFloat4(name, &value.x);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, glm::mat4 &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetArray();
			for (int i = 0; i < 16; ++i)
				ref->PushBack(rapidjson::Value(*(&value[0][0] + i)).Move(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			for (int i = 0; i < 16; ++i)
				*(&value[0][0] + i) = (*ref)[i].GetFloat();
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			if (!name)
				name = "";
			ImGui::Text("TODO Mat4: %s", name);
			ImGui::PopID();
			return;
		}
	}
}

void MemberStructure::registerMember(const char *name, std::string &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, true);
			ref->SetString(value.c_str(), doc->GetAllocator());
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = getRef(name, it, doc, false);
			if (!ref) return;
			value = ref->GetString();
			return;
		}
		case IMGUI:
		{
			if (!name)
				name = "";

			ImGui::PushID(&value);
			char buffer[1024 * 16];
			strncpy(buffer, value.c_str(), sizeof(buffer));
			if (ImGui::InputText(name, buffer, sizeof(buffer)))
				value.assign(buffer);
			ImGui::PopID();
			return;
		}
	}
}

rapidjson::Value* getRef(const char * name, rapidjson::Value * it, rapidjson::Document *doc, bool create) {
	if (create) {
		if (name) {
			it->AddMember(rapidjson::Value(name, doc->GetAllocator()).Move(), rapidjson::Value().Move(), doc->GetAllocator());
			return &(*it)[name];
		}
		return it;
	} else {
		if (name) {
			if (!it->HasMember(name))
				return NULL;
			return &(*it)[name];
		}
		return it;
	}
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
	snprintf(buf, (int)bufSize, "%lld", value);
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
const char* fromString<const char*>(const char *buf) {
	return buf;
}

template <>
bool fromString<bool>(const char *buf) {
	return buf[0] == 't' || buf[0] == '1';
}

template <>
uint8_t fromString<uint8_t>(const char *buf) {
	return atoi(buf);
}

template <>
int8_t fromString<int8_t>(const char *buf) {
	return atoi(buf);
}

template <>
uint16_t fromString<uint16_t>(const char *buf) {
	return atoi(buf);
}

template <>
int16_t fromString<int16_t>(const char *buf) {
	return atoi(buf);
}

template <>
uint32_t fromString<uint32_t>(const char *buf) {
	return (uint32_t)atoll(buf);
}

template <>
int32_t fromString<int32_t>(const char *buf) {
	return atoi(buf);
}

template <>
uint64_t fromString<uint64_t>(const char *buf) {
	return atoll(buf);
}

template <>
int64_t fromString<int64_t>(const char *buf) {
	return atoll(buf);
}

template <>
float fromString<float>(const char *buf) {
	return (float)atof(buf);
}

template <>
double fromString<double>(const char *buf) {
	return atof(buf);
}

template <>
glm::vec2 fromString<glm::vec2>(const char *buf) {
	glm::vec2 value;
	sscanf(buf, "%f,%f", &value.x, &value.y);
	return value;
}

template <>
glm::ivec2 fromString<glm::ivec2>(const char *buf) {
	glm::ivec2 value;
	sscanf(buf, "%i,%i", &value.x, &value.y);
	return value;
}

template <>
glm::vec3 fromString<glm::vec3>(const char *buf) {
	glm::vec3 value;
	sscanf(buf, "%f,%f,%f", &value.x, &value.y, &value.z);
	return value;
}

template <>
glm::vec4 fromString<glm::vec4>(const char *buf) {
	glm::vec4 value;
	sscanf(buf, "%f,%f,%f,%f", &value.x, &value.y, &value.z, &value.w);
	return value;
}

template <>
glm::mat4 fromString<glm::mat4>(const char *buf) {
	glm::mat4 value;
	sscanf(buf, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &value[0][0], &value[0][1], &value[0][2], &value[0][3], &value[1][0], &value[1][1], &value[1][2], &value[1][3], &value[2][0], &value[2][1], &value[2][2], &value[2][3], &value[3][0], &value[3][1], &value[3][2], &value[3][3]);
	return value;
}

template <>
std::string fromString<std::string>(const char *buf) {
	return buf;
}


