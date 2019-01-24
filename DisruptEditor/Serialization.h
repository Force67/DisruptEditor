#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <string.h>
#include "imgui.h"

#define REGISTER_MEMBER(x) ms.registerMember(#x, x)

rapidjson::Value* getRef(const char* name, rapidjson::Value *it, rapidjson::Document *doc, bool create);

template <typename T>
void toString(char *buf, size_t bufSize, T value);

template<typename T>
inline std::string toString(T value) {
	std::string str;
	str.reserve(128);
	toString(str.data(), 128, value);
	return str;
}

template <typename T>
T fromString(const char *buf);

class MemberStructure {
public:
	template <typename T>
	void registerMember(const char* name, std::vector<T> &value);

	template <typename K, typename V>
	void registerMember(const char* name, std::map<K, V> &value);

	template <typename T>
	void registerMember(const char* name, T &value);

#define REG_MEMBER(x) void registerMember(const char *name, x &value);
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
#undef REG_MEMBER

	enum Type { TOJSON, FROMJSON, IMGUI };
	Type type;
	rapidjson::Document *doc;
	rapidjson::Value *it;

	//In your class you need
	//void registerMembers(MemberStructure &ms);
};

template<typename T>
inline void MemberStructure::registerMember(const char * name, std::vector<T>& value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *itBack = it;
			itBack->AddMember(rapidjson::Value(name, doc->GetAllocator()).Move(), rapidjson::Value(rapidjson::kArrayType).Move(), doc->GetAllocator());
			rapidjson::Value &arr = (*itBack)[name];
			for (int i = 0; i < value.size(); ++i) {
				arr.PushBack(rapidjson::Value(rapidjson::kObjectType).Move(), doc->GetAllocator());
				it = &arr[i];
				registerMember(NULL, value[i]);
			}
			it = itBack;
			return;
		}
		case FROMJSON:
		{
			if (!it->HasMember(name))
				return;

			rapidjson::Value *itBack = it;

			rapidjson::Value &arr = (*it)[name];
			value.resize(arr.Size());
			for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
				it = &arr[i];
				registerMember(NULL, value[i]);
			}
			it = itBack;
			return;
		}
		case IMGUI:
		{
			ImGui::PushID(&value);
			ImGui::AlignTextToFramePadding();
			bool node_open = ImGui::TreeNode("Object", "%s", name);
			ImGui::NextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Count: %zu", value.size());
			ImGui::SameLine();
			if (ImGui::Button("+"))
				value.emplace_back();
			ImGui::NextColumn();
			if (node_open) {
				for (int i = 0; i < value.size(); ++i) {
					char buffer[25];
					snprintf(buffer, sizeof(buffer), "%i", i);
					registerMember(buffer, value[i]);
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
			return;
		}
	}
}

template<typename T>
inline void MemberStructure::registerMember(const char *name, T &value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *ref = it;
			if (name) {
				it->AddMember(rapidjson::Value(name, doc->GetAllocator()).Move(), rapidjson::Value().Move(), doc->GetAllocator());
				ref = &(*it)[name];
			}
			ref->SetObject();
			serializeToJSON(value, ref, doc);
			return;
		}
		case FROMJSON:
		{
			rapidjson::Value *ref = it;
			if (name) {
				if (!it->HasMember(name))
					return;
				ref = &(*it)[name];
			}
			unserializeFromJSON(value, ref);
			return;
		}
		case IMGUI:
		{
			displayImGui(value);
			return;
		}
	}
}

template<typename K, typename V>
inline void MemberStructure::registerMember(const char * name, std::map<K, V>& value) {
	switch (type) {
		case TOJSON:
		{
			rapidjson::Value *itBack = it;
			itBack->AddMember(rapidjson::Value(name, doc->GetAllocator()).Move(), rapidjson::Value(rapidjson::kObjectType).Move(), doc->GetAllocator());
			rapidjson::Value &arr = (*itBack)[name];
			it = &arr;
			for (typename std::map<K, V>::iterator v = value.begin(); v != value.end(); ++v) {
				registerMember(toString(v->first).c_str(), v->second);
			}
			it = itBack;
			return;
		}
		case FROMJSON:
		{
			if (!it->HasMember(name))
				return;

			rapidjson::Value *itBack = it;

			rapidjson::Value &arr = (*it)[name];
			it = &arr;
			value.clear();
			for (rapidjson::Value::ConstMemberIterator itr = arr.MemberBegin(); itr != arr.MemberEnd(); ++itr) {
				registerMember(itr->name.GetString(), value[fromString<K>(itr->name.GetString())]);
			}
			it = itBack;
			return;
		}
		case IMGUI:
		{
			if (ImGui::TreeNode(name)) {
				//displayImGui(value);
				ImGui::TreePop();
			}
			return;
		}
	}
}

template<typename T>
inline void serializeToJSON(T & value, rapidjson::Value *it, rapidjson::Document *doc) {
	MemberStructure ms;
	ms.type = MemberStructure::TOJSON;
	ms.it = it;
	ms.doc = doc;
	value.registerMembers(ms);
}

template<typename T>
inline std::string serializeToJSON(T & value) {
	rapidjson::Document doc;
	doc.SetObject();
	serializeToJSON(value, &doc, &doc);
	
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}

template <typename T>
inline void unserializeFromJSON(T& value, rapidjson::Value *it) {
	MemberStructure ms;
	ms.type = MemberStructure::FROMJSON;
	ms.it = it;
	value.registerMembers(ms);
}

template <typename T>
inline void unserializeFromJSON(T& value, const char* str) {
	if (!str || *str == '\0') return;
	rapidjson::Document doc;
	doc.Parse(str);
	unserializeFromJSON(value, &doc);
}

template <typename T>
inline void displayImGui(T& value) {
	MemberStructure ms;
	ms.type = MemberStructure::IMGUI;
	value.registerMembers(ms);
}
