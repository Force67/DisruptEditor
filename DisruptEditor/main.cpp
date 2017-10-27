#include "glad.h"
#include <SDL.h>
#include "Implementation.h"
#include "Common.h"
#include "DDRenderInterface.h"
#include "spkFile.h"
#include "sbaoFile.h"
#include "cseqFile.h"
#include "materialFile.h"
#include <unordered_map>
#include "Hash.h"
#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

const ddVec3 red = { 1.0f, 0.0f, 0.0f };
const ddVec3 blue = { 0.0f, 0.0f, 1.0f };
const ddVec3 cyan = { 0.0f, 1.0f, 1.0f };
const ddVec3 magenta = { 1.0f, 0.2f, 0.8f };
const ddVec3 yellow = { 1.0f, 1.0f, 0.0f };
const ddVec3 orange = { 1.0f, 0.5f, 0.0f };
const ddVec3 white = { 1.0f, 1.0f, 1.0f };
const ddVec3 black = { 0.f, 0.f, 0.f };
const ddVec3 green = { 0.0f, 0.6f, 0.0f };

struct BuildingEntity {
	std::string wlu;
	std::string CBatchPath;
	vec3 pos;
	vec3 min, max;
};
std::vector<BuildingEntity> buildingEntities;
std::map<std::string, wluFile> wlus;

void reloadBuildingEntities() {
	buildingEntities.clear();
	for (auto it = wlus.begin(); it != wlus.end(); ++it) {
		Node *Entities = it->second.root.findFirstChild("Entities");
		for (Node &Entity : Entities->children) {
			//CBatchMeshEntity
			Attribute *hidEntityClass = Entity.getAttribute("hidEntityClass");
			if (!hidEntityClass) continue;

			if (*(uint32_t*)hidEntityClass->buffer.data() == 138694286) {
				BuildingEntity be;
				be.wlu = it->first;
				be.CBatchPath = (char*)Entity.getAttribute("ExportPath")->buffer.data();

				//Cut off .batch
				be.CBatchPath = be.CBatchPath.substr(0, be.CBatchPath.size() - 6);

				Attribute *hidPos = Entity.getAttribute("hidPos");
				if (!hidPos) continue;
				be.pos = swapYZ(*(vec3*)hidPos->buffer.data());

				Node *hidBBox = Entity.findFirstChild("hidBBox");
				be.min = *(vec3*)hidBBox->getAttribute("vectorBBoxMin")->buffer.data();
				be.min = swapYZ(be.min);
				be.min += be.pos;
				be.max = *(vec3*)hidBBox->getAttribute("vectorBBoxMax")->buffer.data();
				be.max = swapYZ(be.max);
				be.max += be.pos;

				buildingEntities.push_back(be);
			}
		}
	}
}

int main(int argc, char **argv) {
	//freopen("debug.log", "wb", stdout);
	SDL_Init(SDL_INIT_EVERYTHING);

	/*sbaoFile sb;
	sb.open("D:\\Desktop\\bin\\default\\000fac53.sbao");
	return 0;*/

	//Setup INI
	std::string wludir;
	float textDrawDistance = 5.f;
	{
		ini_t* ini = ini_load(loadFile("settings.ini").c_str(), NULL);

		int settings_section = ini_find_section(ini, "settings", 0);
		wludir = ini_property_value(ini, settings_section, ini_find_property(ini, settings_section, "wludir", 0));
		textDrawDistance = atof(ini_property_value(ini, settings_section, ini_find_property(ini, settings_section, "textDrawDistance", 0)));

		//Pack Files
		int pack_section = ini_find_section(ini, "pack", 0);
		for (int i = 0; i < ini->property_capacity; ++i) {
			if (ini->properties[i].section == pack_section) {
				std::string packPath;
				if (ini->properties[i].value_large)
					packPath = ini->properties[i].value_large;
				else
					packPath = ini->properties[i].value;
				
				if (packPath.back() != '/' && packPath.back() != '\\')
					packPath.push_back('/');

				addSearchPath(packPath);
			}
		}

		ini_destroy(ini);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_Window* window = SDL_CreateWindow(
		"Disrupt Editor",                  // window title
		SDL_WINDOWPOS_CENTERED,           // initial x position
		SDL_WINDOWPOS_CENTERED,           // initial y position
		1600,                               // width, in pixels
		900,                               // height, in pixels
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // flags - see below
	);
	if (window == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL) {
		printf("Could not create context: %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_GL_MakeCurrent(window, glcontext) != 0) {
		printf("Could not create context: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_SetSwapInterval(1);
	gladLoadGL();
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	ImGui_ImplSdlGL3_Init(window);

	//Style
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.19f);
	colors[ImGuiCol_ChildWindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ComboBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.99f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_CloseButton] = ImVec4(0.41f, 0.41f, 0.41f, 0.50f);
	colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
#ifdef IMGUI_HAS_NAV
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.12f);
#endif

	Camera camera;
	camera.type = Camera::FLYCAM;
	camera.far_plane = 4500.f;

	RenderInterface renderInterface;
	dd::initialize(&renderInterface);

	//const std::string wd = "C:/Program Files/Ubisoft/WATCH_DOGS/data_win64/";
	//DatFat df;
	//df.addFat(wd + "common.fat");
	//return 0;
	//df.addFat(wd + "worlds/windy_city/windy_city.fat");

	//df.openRead("domino/user/windycity/main_missions/act_02/mission_09c/a02_m09c.a02_m09c_activation.debug.lua");

	//DominoBox db("D:\\Desktop\\bin\\dlc_solo\\domino\\user\\windycity\\tests\\ai_carfleeing_patterns\\ai_carfleeing_patterns.main.lua");

	tfDIR dir;
	tfDirOpen(&dir, (wludir + std::string("/worlds/windy_city/generated/wlu")).c_str());
	while (dir.has_next) {
		tfFILE file;
		tfReadFile(&dir, &file);

		if (!file.is_dir && strcmp(file.ext, "xml.data.fcb") == 0) {
			printf("Loading %s\n", file.name);

			if (!wlus[file.name].open(file.path)) {
				wlus.erase(file.name);
			}
		}

		tfDirNext(&dir);
	}
	tfDirClose(&dir);

	//Scan Materials
	/*tfDirOpen(&dir, "D:/Desktop/bin/windy_city/graphics/_materials");
	while (dir.has_next) {
		tfFILE file;
		tfReadFile(&dir, &file);

		if (!file.is_dir) {
			printf("Loading %s\n", file.name);

			materialFile spk;
			spk.open(file.path);
		}

		tfDirNext(&dir);
	}
	tfDirClose(&dir);*/

	//Scan Audio
	/*tfDirOpen(&dir, "D:/Desktop/bin/sound_unpack/soundbinary/manual");
	while (dir.has_next) {
		tfFILE file;
		tfReadFile(&dir, &file);

		if (!file.is_dir && strcmp(file.ext, "sbao") == 0) {
			printf("Loading %s\n", file.name);

			sbaoFile spk;
			spk.open(file.path);
		}

		tfDirNext(&dir);
	}
	tfDirClose(&dir);

	//Scan CSeq
	/*tfDirOpen(&dir, (wd + std::string("sequences")).c_str());
	while (dir.has_next) {
		tfFILE file;
		tfReadFile(&dir, &file);

		if (!file.is_dir && strcmp(file.ext, "cseq") == 0) {
			printf("Loading %s\n", file.name);

			cseqFile cseq;
			cseq.open(file.path);

			FILE *fp = fopen("cseq.xml", "w");
			tinyxml2::XMLPrinter printer(fp);
			cseq.root.serializeXML(printer);
			fclose(fp);
		}

		tfDirNext(&dir);
	}
	tfDirClose(&dir);*/

	Uint32 ticks = SDL_GetTicks();
	wluFile &wlu = wlus.begin()->second;
	wlu.shortName = wlus.begin()->first;

	bool windowOpen = true;
	while (windowOpen) {
		ImGui_ImplSdlGL3_NewFrame(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindVertexArray(VertexArrayID);
		glEnable(GL_DEPTH_TEST);

		float delta = (SDL_GetTicks() - ticks) / 1000.f;
		ticks = SDL_GetTicks();
		if (delta > 0.5f)
			delta = 0.5f;

		ivec2 windowSize;
		SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
		glViewport(0, 0, windowSize.x, windowSize.y);
 		camera.update(delta);
		mat4 view = MATlookAt(camera.location, camera.lookingAt, camera.up);
		mat4 projection = MATperspective(camera.fov, (float)windowSize.x / windowSize.y, camera.near_plane, camera.far_plane);
		mat4 vp = projection * view;
		renderInterface.VP = vp;
		renderInterface.windowSize = windowSize;

		ImGui::SetNextWindowPos(ImVec2(5.f, 5.f));
		ImGui::Begin("##Top", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::DragFloat3("##Camera", (float*)&camera.location);
		ImGui::End();

		/*if (nk_begin(ctx, "Dare", nk_rect(0, 0, 450, 500), 0)) {
			nk_layout_row_dynamic(ctx, 20, 1);
			if (nk_button_label(ctx, "Convert OGG to SBAO")) {
				const char *src = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "ogg\0*.ogg\0", NULL, NULL);
				const char *dst = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "sbao\0*.sbao\0", NULL, "00000000");
			}
		}
		nk_end(ctx);

		if (nk_begin(ctx, "Scripts", nk_rect(0, 0, 450, 500), 0)) {
			nk_layout_row_dynamic(ctx, 20, 1);
		}
		nk_end(ctx);*/

		//Draw Layer Window
		ImGui::SetNextWindowSize(ImVec2(600, windowSize.y), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Layers")) {

			static char searchWluBuffer[255] = { 0 };
			ImGui::InputText("Search", searchWluBuffer, sizeof(searchWluBuffer));

			ImGui::ListBoxHeader("##WLU List");
			for (auto it = wlus.begin(); it != wlus.end(); ++it) {
				if (it->first.find(searchWluBuffer) != std::string::npos) {
					int selected = wlu.origFilename == it->second.origFilename;
					if (ImGui::Selectable(it->first.c_str())) {
						wlu = it->second;
						wlu.shortName = it->first;
					}
				}
			}
			ImGui::ListBoxFooter();

			if (ImGui::Button("Save")) {
				std::string backup = wlu.origFilename;
				backup += ".bak";
				CopyFileA(wlu.origFilename.c_str(), backup.c_str(), TRUE);

				FILE *fp = fopen(wlu.origFilename.c_str(), "wb");
				wlu.serialize(fp);
				fclose(fp);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reload")) {
				assert(wlu.open(wlu.origFilename.c_str()));
			}
			ImGui::SameLine();
			if (ImGui::Button("Restore")) {
				std::string backup = wlu.origFilename;
				backup += ".bak";
				CopyFileA(backup.c_str(), wlu.origFilename.c_str(), FALSE);
				wlu.open(wlu.origFilename.c_str());
			}
			ImGui::SameLine();
			std::string xmlFileName = wlu.shortName + ".xml";
			if (ImGui::Button("XML")) {
				FILE *fp = fopen(xmlFileName.c_str(), "wb");
				tinyxml2::XMLPrinter printer(fp);
				wlu.root.serializeXML(printer);
				fclose(fp);
			}
			ImGui::SameLine();
			if (ImGui::Button("Import XML")) {
				tinyxml2::XMLDocument doc;
				doc.LoadFile(xmlFileName.c_str());
				wlu.root.deserializeXML(doc.RootElement());
			}
			ImGui::Separator();

			Node *Entities = wlu.root.findFirstChild("Entities");
			if (!Entities) continue;

			for (auto &entity : Entities->children) {
				bool needsCross = true;
				char imguiHash[512];

				Attribute *hidName = entity.getAttribute("hidName");
				Attribute *hidPos = entity.getAttribute("hidPos");
				vec3 pos = swapYZ(*(vec3*)hidPos->buffer.data());

				//
				Node *hidBBox = entity.findFirstChild("hidBBox");

				Node *Components = entity.findFirstChild("Components");
				assert(Components);

				Node* CGraphicComponent = Components->findFirstChild("CGraphicComponent");
				if (CGraphicComponent) {
					Attribute* XBG = CGraphicComponent->getAttribute(0x3182766C);

					if (XBG && XBG->buffer.size() > 5) {
						ImGui::Text("%s", XBG->buffer.data());
						auto &model = loadXBG((char*)XBG->buffer.data());
						renderInterface.model.use();
						mat4 MVP = vp * MATtranslate(mat4(), pos);
						glUniformMatrix4fv(renderInterface.model.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
						model.draw();
					}
				}

				Node *CProximityTriggerComponent = Components->findFirstChild("CProximityTriggerComponent");
				if (CProximityTriggerComponent) {
					needsCross = false;
					vec3 extent = swapYZ(*(vec3*)CProximityTriggerComponent->getAttribute("vectorSize")->buffer.data());
					dd::box(&pos.x, red, extent.x, extent.y, extent.z);
				}

				if (hidBBox && false) {
					vec3 boxMin = swapYZ(*((vec3*)hidBBox->getAttribute("vectorBBoxMin")->buffer.data()));
					vec3 boxMax = swapYZ(*((vec3*)hidBBox->getAttribute("vectorBBoxMax")->buffer.data()));
					vec3 boxExtent = boxMax - boxMin;
					dd::box(&pos.x, blue, boxExtent.x, boxExtent.y, boxExtent.z);
				}

				Node* PatrolDescription = entity.findFirstChild("PatrolDescription");
				if (PatrolDescription) {
					needsCross = false;
					Node* PatrolPointList = PatrolDescription->findFirstChild("PatrolPointList");

					vec3 last;
					for (Node &PatrolPoint : PatrolPointList->children) {
						vec3 pos = swapYZ(*(vec3*)PatrolPoint.getAttribute("vecPos")->buffer.data());

						if (last != vec3())
							dd::line(&last[0], &pos[0], red);
						else
							dd::projectedText((char*)hidName->buffer.data(), &pos.x, red, &vp[0][0], 0, 0, windowSize.x, windowSize.y, 0.5f);
						last = pos;
					}
				}

				//Iterate through Entity Attributes
				if (ImGui::TreeNode((char*)hidName->buffer.data())) {
					snprintf(imguiHash, sizeof(imguiHash), "Goto##%p", &entity);
					if (ImGui::Button(imguiHash)) {
						camera.phi = 2.43159294f;
						camera.theta = 3.36464548f;
						camera.location = pos + vec3(1.f, 1.f, 0.f);
					}

					for (Attribute &attr : entity.attributes) {
						char name[1024];
						snprintf(name, sizeof(name), "%s##%p", Hash::instance().getReverseHash(attr.hash).c_str(), &attr);

						Hash::Types type = Hash::instance().getHashType(attr.hash);

						switch (type) {
							case Hash::STRING:
							{
								char temp[1024] = { '\0' };
								strncpy(temp, (char*)attr.buffer.data(), sizeof(temp));
								if (ImGui::InputText(name, temp, sizeof(temp))) {
									attr.buffer.resize(strlen(temp) + 1);
									strcpy((char*)attr.buffer.data(), temp);
								}
								break;
							}
							case Hash::FLOAT:
								ImGui::InputFloat(name, (float*)attr.buffer.data());
								break;
							case Hash::VEC2:
								ImGui::InputFloat2(name, (float*)attr.buffer.data());
								break;
							case Hash::VEC3:
								ImGui::InputFloat3(name, (float*)attr.buffer.data());
								break;
							case Hash::VEC4:
								ImGui::InputFloat4(name, (float*)attr.buffer.data());
								break;
							default:
								ImGui::LabelText(name, "BinHex %u", attr.buffer.size());
								break;
						}
					}

					//Handle Components
					if (PatrolDescription && ImGui::TreeNode("PatrolDescription")) {
						Node* PatrolPointList = PatrolDescription->findFirstChild("PatrolPointList");
						for (Node &PatrolPoint : PatrolPointList->children) {
							ImGui::InputFloat3("##a", (float*)PatrolPoint.getAttribute("vecPos")->buffer.data());
						}
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				if (pos.distance(camera.location) < textDrawDistance)
					dd::projectedText((char*)hidName->buffer.data(), &pos.x, white, &vp[0][0], 0, 0, windowSize.x, windowSize.y, 0.5f);
				if (needsCross)
					dd::cross(&pos.x, 0.25f);

			}
			ImGui::End();
		}

		//Render Buildings
		if (buildingEntities.empty())
			reloadBuildingEntities();

		renderInterface.model.use();
		for (const BuildingEntity &Entity : buildingEntities) {
			dd::aabb(&Entity.min.x, &Entity.max.x, blue);
			if (Entity.pos.distance(camera.location) < 256)
				dd::projectedText(Entity.wlu.c_str(), &Entity.pos.x, white, &vp[0][0], 0, 0, windowSize.x, windowSize.y, 0.5f);

			mat4 translate = MATtranslate(mat4(), Entity.pos + vec3(0.f, 64.f, 0.f));
			mat4 MVP = vp * MATscale(translate, vec3(128.f));
			glUniformMatrix4fv(renderInterface.model.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);

			{
				std::string CBatchXbgPath = Entity.CBatchPath + "_building_low.xbg";
				auto &model = loadXBG(CBatchXbgPath);
				if (model.meshes.empty()) continue;
				model.draw();
			}

			//Draw roof
			{
				std::string CBatchXbgPath = Entity.CBatchPath + "_building_roofs.xbg";
				auto &model = loadXBG(CBatchXbgPath);
				model.draw();
			}
		}

		glBindVertexArray(VertexArrayID);
		dd::flush(0);
		ImGui::Render();
		SDL_GL_SwapWindow(window);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSdlGL3_ProcessEvent(&event);
			switch (event.type) {
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
						windowOpen = false;
						exit(0);
					}
					break;
			}
		}
	}

	return 0;
}