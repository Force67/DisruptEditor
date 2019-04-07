#pragma once

#include "tinyxml2.h"
#include <string>
#include <vector>
#include <array>
#include <map>
#include <stdint.h>
#include <string.h>
#include "imgui.h"

#define REGISTER_MEMBER(x) ms.registerMember(#x, x)

template <typename T>
void toString(char *buf, size_t bufSize, T value);

template<typename T>
inline std::string toString(T value) {
	std::string str;
	str.resize(65535, '\0');
	toString(str.data(), 65535, value);
	return str;
}

template <typename T>
void fromString(const char *buf, T &value);

size_t XMLNumChildren(tinyxml2::XMLElement* it);

class MemberStructure {
public:
	template <typename T, size_t Size>
	void registerMember(const char* name, std::array<T, Size> &value);

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

	enum Type { TOXML, FROMXML, IMGUI };
	Type type;
	tinyxml2::XMLElement *it;
	tinyxml2::XMLPrinter* printer;

	//In your class you need
	//void registerMembers(MemberStructure &ms);
};

template<typename T, size_t Size>
inline void MemberStructure::registerMember(const char * name, std::array<T, Size>& value) {
	switch (type) {
	case TOXML:
	{
		if (name)
			printer->OpenElement(name);
		for (int i = 0; i < Size; ++i) {
			registerMember("Elem", value[i]);
		}
		if (name)
			printer->CloseElement();
		return;
	}
	case FROMXML:
	{
		tinyxml2::XMLElement* itBack = it;
		if (name)
			it = it->FirstChildElement(name);
		if (it) {
			it = it->FirstChildElement();
			size_t i = 0;
			while (it) {
				registerMember(NULL, value[i]);
				it = it->NextSiblingElement();
				++i;
			}
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
		ImGui::NextColumn();
		if (node_open) {
			for (int i = 0; i < Size; ++i) {
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
inline void MemberStructure::registerMember(const char * name, std::vector<T>& value) {
	switch (type) {
		case TOXML:
		{
			if (name)
				printer->OpenElement(name);
			for (int i = 0; i < value.size(); ++i) {
				registerMember("Elem", value[i]);
			}
			if (name)
				printer->CloseElement();
			return;
		}
		case FROMXML:
		{
			tinyxml2::XMLElement* itBack = it;
			if (name)
				it = it->FirstChildElement(name);
			if (it) {
				value.clear();
				it = it->FirstChildElement();
				while (it) {
					registerMember(NULL, value.emplace_back());
					it = it->NextSiblingElement();
				}
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
		case TOXML:
		{
			if(name)
				printer->OpenElement(name);
			value.registerMembers(*this);
			if (name)
				printer->CloseElement();
			return;
		}
		case FROMXML:
		{
			tinyxml2::XMLElement* ref = name ? it->FirstChildElement(name) : it;
			if (ref)
				unserializeFromXML(value, ref);
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
		case TOXML:
		{
			printer->OpenElement(name);
			for (typename std::map<K, V>::iterator v = value.begin(); v != value.end(); ++v) {
				registerMember(toString(v->first).c_str(), v->second);
			}
			printer->CloseElement();
			return;
		}
		case FROMXML:
		{
			tinyxml2::XMLElement* itBack = it;
			it = it->FirstChildElement(name);
			if (it) {
				value.clear();
				it = it->FirstChildElement();
				while (it) {
					K key;
					fromString(it->Name(), key);
					registerMember(it->Name(), value[key]);
					it = it->NextSiblingElement();
				}
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
inline void serializeToXML(T & value, tinyxml2::XMLPrinter *printer) {
	MemberStructure ms;
	ms.type = MemberStructure::TOXML;
	ms.printer = printer;
	value.registerMembers(ms);
}

template<typename T>
inline std::string serializeToXML(T & value) {
	tinyxml2::XMLPrinter printer;
	printer.OpenElement("Root");
	serializeToXML(value, &printer);
	printer.CloseElement();
	return std::string(printer.CStr());
}

template <typename T>
inline void unserializeFromXML(T& value, tinyxml2::XMLElement *it) {
	MemberStructure ms;
	ms.type = MemberStructure::FROMXML;
	ms.it = it;
	value.registerMembers(ms);
}

template <typename T>
inline void unserializeFromXML(T& value, const char* str) {
	if (!str || *str == '\0') return;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.Parse(str);
	if(ret == tinyxml2::XML_SUCCESS)
		unserializeFromXML(value, doc.RootElement());
}

template <typename T>
inline void displayImGui(T& value) {
	MemberStructure ms;
	ms.type = MemberStructure::IMGUI;
	value.registerMembers(ms);
}
