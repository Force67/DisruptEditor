#include "glad.h"
#include <SDL.h>
#include "Implementation.h"
#include "Common.h"
#include "DDRenderInterface.h"
#include "cseqFile.h"
#include "wluFile.h"
#include "xbgFile.h"
#include "materialFile.h"
#include "Camera.h"
#include <map>
#include <unordered_map>
#include "Hash.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "LoadingScreen.h"
#include "Dialog.h"
#include "Entity.h"
#include "DominoBox.h"
#include <future>
#include <unordered_set>
#include <Ntsecapi.h>
#include "RML.h"
#include "glm/gtc/matrix_transform.hpp"
#include "CSector.h"
#include "batchFile.h"
#include "Audio.h"
#include "ImGuizmo.h"
#include "FileHandler.h"
#include "World.h"
#include "ImguiWindows.h"
#include "Serialization.h"
#include "Version.h"
#include "IBinaryArchive.h"
#include "HexBase64.h"
#include "DB.h"
#include "DARE.h"

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	LoadingScreen *loadingScreen = new LoadingScreen;
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

	reloadSettings();

	SDL_Window* window = SDL_CreateWindow("Disrupt Editor v" DE_VERSIONSTR, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.windowSize.x, settings.windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (window == NULL) {
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL) {
		SDL_Log("Could not create context: %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_GL_MakeCurrent(window, glcontext) != 0) {
		SDL_Log("Could not create context: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_SetSwapInterval(1);
	gladLoadGL();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsDark(NULL);

	Camera camera;
	camera.type = Camera::FLYCAM;
	camera.near_plane = 0.25f;
	camera.far_plane = 6500.f;

	//Debug
#if _DEBUG
	{
		DARE::instance().addSoundResource(0x0012a611);

		//locFile loc;
		//loc.open("Z:\\scratch\\bin\\common\\languages\\main_english.loc");

		/*auto b = fromHexString("10 ae 2346");
		auto c = fromHexString("10AE2346");

		temp2 = std::to_string(Hash::getHash("SBatchedSoundPointBreakable"));*/

		char buffer[255];
		snprintf(buffer, sizeof(buffer), "graphics\\Terrain\\Water\\AnimatedCaustics_%03d.xbt", 0);

		uint32_t d = Hash::getFilenameHash(buffer);

		CStringID temp;
		temp.id = 0x1D072E66;
		std::string temp2 = temp.getReverseName();
		FH::Init();

		/*tfDIR dir;
		tfDirOpen(&dir, "C:\\Users\\Jonathan\\Desktop\\WD_materials_BIN");
		while (dir.has_next) {
			tfFILE file;
			tfReadFile(&dir, &file);

			if (!file.is_dir && strstr(file.name, ".bin") != NULL && strstr(file.name, ".bin.xml") == NULL) {
				SDL_Log("Loading %s", file.name);

				materialFile mat;
				SDL_RWops* fp = SDL_RWFromFile(file.path, "rb");
				CBinaryArchiveReader reader(fp);
				mat.open(reader);
				SDL_RWclose(fp);
				std::string str = serializeToXML(mat);
				std::string outFilename = file.path + std::string(".xml");
				fp = SDL_RWFromFile(outFilename.c_str(), "wb");
				SDL_RWwrite(fp, str.data(), str.size(), 1);
				SDL_RWclose(fp);
			}

			tfDirNext(&dir);
		}
		tfDirClose(&dir);*/

		{
			xbgFile xbg;
			SDL_RWops *fp = SDL_RWFromFile("C:\\Users\\Jonathan\\Desktop\\char01.xbg", "rb");
			CBinaryArchiveReader reader(fp);
			xbg.open(reader);
			SDL_RWclose(fp);
		}

		batchFile bf;
		try {
			SDL_RWops* fp = SDL_RWFromFile("C:\\Program Files\\Ubisoft\\WATCH_DOGS\\bin\\patch\\worlds\\windy_city\\generated\\batchmeshentity\\batchmeshentity_c2_i0_xn0767_yp0513_xn0641_yp0639_compound.cbatch", "rb");
			CBinaryArchiveReader reader(fp);
			//bf.open(reader);
			SDL_RWclose(fp);

			//bf.componentMBP.batchProcessors.clear();
			//bf.physicsFile.id = -1;

			/*fp = SDL_RWFromFile("C:\\Program Files\\Ubisoft\\WATCH_DOGS\\bin\\patch\\worlds\\windy_city\\generated\\batchmeshentity\\batchmeshentity_c2_i0_xn0767_yp0513_xn0641_yp0639_compound.cbatch", "wb");
			bf.open(CBinaryArchiveWriter(fp));
			SDL_RWclose(fp);*/

		}
		catch (...) {}
		std::string str = serializeToXML(bf);
		int a = 1;

		/*tfDIR dir;
		tfDirOpen(&dir, "C:\\Program Files\\Ubisoft\\WATCH_DOGS\\bin\\patch\\worlds\\windy_city\\generated\\batchmeshentity");
		while (dir.has_next) {
			tfFILE file;
			tfReadFile(&dir, &file);

			if (!file.is_dir && strstr(file.name, "_compound.cbatch") != NULL) {
				SDL_Log("Loading %s", file.name);

				batchFile bf;
				bf.open(SDL_RWFromFile(file.path, "rb"));
				std::string str = serializeToJSON(bf);
				std::string outFilename = file.name + std::string(".json");
			}

			tfDirNext(&dir);
		}
		tfDirClose(&dir);*/

		//tfDirOpen(&dir, "D:\\Desktop\\bin\\sound\\soundbinary");
		//tfDirOpen(&dir, "D:\\Desktop\\bin\\sound\\__UNKNOWN/sfx");
		/*tfDirOpen(&dir, "D:\\Desktop\\bin\\windy_city\\soundbinary");
		SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
		while (dir.has_next) {
			tfFILE file;
			tfReadFile(&dir, &file);

			if (!file.is_dir && strstr(file.name, ".spk") != NULL) {
				SDL_Log("Loading %s\n", file.name);

				spkFile bf;
				bf.open(file.path);
			}

			tfDirNext(&dir);
		}
		tfDirClose(&dir);*/
		

		/*tfDirOpen(&dir, "C:\\Program Files\\Ubisoft\\WATCH_DOGS\\bin\\patch\\worlds/windy_city/generated/sdat");
		while (dir.has_next) {
			tfFILE file;
			tfReadFile(&dir, &file);

			if (!file.is_dir && strstr(file.name, ".sdhr") != NULL) {
				SDL_Log("Loading %s\n", file.name);

				CSectorHighRes spk;
				spk.open(file.path);
			}

			tfDirNext(&dir);
		}
		tfDirClose(&dir);*/
	}
#endif

	{
		loadingScreen->setTitle("Setting Up Database");
		std::future<void> f = std::async(FH::Init);
		loadingScreen->waitForFuture(f);

		f = std::async([]() { DB::instance(); });
		loadingScreen->waitForFuture(f);

		loadingScreen->setTitle("Loading Entity Library");
		f = std::async(loadEntityLibrary);
		loadingScreen->waitForFuture(f);

		loadingScreen->setTitle("Loading Language Files");
		//Dialog::instance();

		SDL_PumpEvents();
		loadingScreen->setTitle("Loading Particle Library");
		world.particles = loadRml(FH::openFile("worlds/windy_city/generated/windy_city_deploadnewparticles.rml"));

		world.spawnPointList = loadXml(FH::openFile("worlds/windy_city/generated/spawnpointlist.xml"));

		std::thread wluThread(world.loadWLUAsync);
		wluThread.detach();
	}

	/*{
		FILE *out = fopen("out.txt", "w");
		for (auto it = entityLibraryUID.begin(); it != entityLibraryUID.end(); ++it) {
			std::string arche = Hash::getReverseHashFNV(it->first);
			fprintf(out, "%s = %s\n", it->second.c_str(), arche.c_str());
		}
		fclose(out);
	}*/

	/*{
		loadingScreen->mutex.lock();
		loadingScreen->title = "Brute forcing archetypes...";
		loadingScreen->message.clear();
		loadingScreen->mutex.unlock();
		std::unordered_set<uint32_t> unknown;
		{
			for (auto it = entityLibraryUID.begin(); it != entityLibraryUID.end(); ++it) {
				unknown.emplace(it->first);
			}

			for (auto it = Hash::reverseFNVHash.begin(); it != Hash::reverseFNVHash.end(); ++it) {
				unknown.erase(it->first);
			}
		}
		struct UUID {
			uint32_t Data1;
			uint16_t  Data2;
			uint16_t  Data3;
			uint8_t  Data4[8];
		};
		UUID guid;
		char buffer[250];
		uint32_t hash;
		FILE *fp = fopen("res/archeBrute.txt", "a");
		while (!unknown.empty()) {
			snprintf(buffer, sizeof(buffer), "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

			hash = Hash::getFilenameHash(buffer);
			if (unknown.count(hash) > 0) {
				unknown.erase(hash);
				fprintf(fp, "%s\n", buffer);
				fflush(fp);
			}

			RtlGenRandom(&guid, sizeof(guid));
		}
		fclose(fp);
	}
	return 0;*/

	Uint32 ticks = SDL_GetTicks();
	uint64_t frameCount = 0;
	std::shared_ptr<wluFile> currentWlu;

	dd::initialize(&RenderInterface::instance());
	if (settings.maximized)
		SDL_MaximizeWindow(window);
	SDL_ShowWindow(window);
	delete loadingScreen;
	loadingScreen = NULL;

	bool windowOpen = true;
	while (windowOpen) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindVertexArray(RenderInterface::instance().VertexArrayID);
		glEnable(GL_DEPTH_TEST);

		float delta = (SDL_GetTicks() - ticks) / 1000.f;
		ticks = SDL_GetTicks();
		if (delta > 0.5f)
			delta = 0.5f;

		RenderInterface &renderInterface = RenderInterface::instance();

		SDL_GetWindowSize(window, &renderInterface.windowSize.x, &renderInterface.windowSize.y);
		glViewport(0, 0, renderInterface.windowSize.x, renderInterface.windowSize.y);
		renderInterface.View = glm::lookAtLH(camera.location, camera.lookingAt, camera.up);
		renderInterface.Projection = glm::perspective(camera.fov, (float)renderInterface.windowSize.x / renderInterface.windowSize.y, camera.near_plane, camera.far_plane);
		renderInterface.VP = renderInterface.Projection * renderInterface.View;

		UI::displayTopMenu();
		UI::displayTempWindows();
		UI::displayWindows();

		if (world.loadingProgress != 1.f) {
			ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_Always);
			ImGui::Begin("Loading", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("%i%% %s", (int)(world.loadingProgress * 100), world.loadingStatus.c_str());
			ImGui::End();
		}

		//Draw Layer Window
		ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(1150.f, 5.f), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Layers")) {
			//Wlu List
			ImGui::PushItemWidth(-1.f);
			static char searchWluBuffer[255] = { 0 };
			ImGui::InputText("##Search", searchWluBuffer, sizeof(searchWluBuffer));

			ImVec2 size = ImGui::GetWindowContentRegionMax();
			size.y -= 75;
			size.x -= 5;
			ImGui::ListBoxHeader("##WLU List", size);
			for (auto it = world.wlus.begin(); it != world.wlus.end(); ++it) {
				if (it->first.find(searchWluBuffer) != std::string::npos) {
					bool selected = currentWlu == it->second;
					if (ImGui::Selectable(it->first.c_str(), selected))
						currentWlu = it->second;
				}
			}
			ImGui::ListBoxFooter();
			ImGui::PopItemWidth();

			if (currentWlu) {
				wluFile& wlu = *currentWlu;

				if (ImGui::Button("Save")) {
					std::string backup = wlu.origFilename;
					backup += ".bak";
					CopyFileA(wlu.origFilename.c_str(), backup.c_str(), TRUE);
					wlu.serialize(wlu.origFilename.c_str());
				}
				ImGui::SameLine();
				if (ImGui::Button("Reload")) {
					SDL_assert_release(wlu.open(wlu.origFilename.c_str()));
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
					FILE* fp = fopen(xmlFileName.c_str(), "wb");
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
			}
			
			ImGui::End();
			ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Properties");

			if (currentWlu) {
				wluFile& wlu = *currentWlu;
				wlu.draw();

				Node* Entities = wlu.root.findFirstChild("Entities");
				if (!Entities) continue;

				for (Node& entityRef : Entities->children) {
					bool needsCross = true;

					Node* entityPtr = &entityRef;
					Attribute* ArchetypeGuid = entityRef.getAttribute("ArchetypeGuid");
					if (ArchetypeGuid) {
						uint32_t uid = Hash::getFilenameHash((const char*)ArchetypeGuid->buffer.data());
						entityPtr = findEntityByUID(uid);
						if (!entityPtr) {
							SDL_Log("Could not find %s\n", ArchetypeGuid->buffer.data());
							SDL_assert_release(false && "Could not lookup entity by archtype, check that dlc_solo is loaded first before other packfiles");
							entityPtr = &entityRef;
						}
					}
					Node& entity = *entityPtr;

					Attribute* hidName = entity.getAttribute("hidName");
					glm::vec3& pos = entity.get<glm::vec3>("hidPos");
					glm::vec3& angles = entity.get<glm::vec3>("hidAngles");

					//
					Node* hidBBox = entity.findFirstChild("hidBBox");

					Node* Components = entity.findFirstChild("Components");
					SDL_assert_release(Components);

					Node* CGraphicComponent = Components->findFirstChild("CGraphicComponent");
					if (CGraphicComponent) {
						Attribute* XBG = CGraphicComponent->getAttribute(0x3182766C);

						/*if (XBG && XBG->buffer.size() > 5) {
							auto &model = loadXBG((char*)XBG->buffer.data());
							renderInterface.model.use();

							glm::mat4 modelMatrix = glm::translate(glm::mat4(), pos);
							modelMatrix = glm::rotate(modelMatrix, angles.x, glm::vec3(1, 0, 0));
							modelMatrix = glm::rotate(modelMatrix, angles.y, glm::vec3(0, 1, 0));
							modelMatrix = glm::rotate(modelMatrix, angles.z, glm::vec3(0, 0, 1));

							glm::mat4 MVP = renderInterface.VP * modelMatrix;
							glUniformMatrix4fv(renderInterface.model.uniforms["MVP"], 1, GL_FALSE, &MVP[0][0]);
							model.draw();
						}*/
					}

					Node* CProximityTriggerComponent = Components->findFirstChild("CProximityTriggerComponent");
					if (CProximityTriggerComponent) {
						needsCross = false;
						glm::vec3 extent = *(glm::vec3*)CProximityTriggerComponent->getAttribute("vectorSize")->buffer.data();
						dd::box(&pos.x, red, extent.x, extent.y, extent.z);
					}

					if (hidBBox && false) {
						glm::vec3 boxMin = *((glm::vec3*)hidBBox->getAttribute("vectorBBoxMin")->buffer.data());
						glm::vec3 boxMax = *((glm::vec3*)hidBBox->getAttribute("vectorBBoxMax")->buffer.data());
						glm::vec3 boxExtent = boxMax - boxMin;
						dd::box(&pos.x, blue, boxExtent.x, boxExtent.y, boxExtent.z);
					}

					Node* PatrolDescription = entity.findFirstChild("PatrolDescription");
					if (PatrolDescription) {
						needsCross = false;
						Node* PatrolPointList = PatrolDescription->findFirstChild("PatrolPointList");

						glm::vec3 last;
						for (Node& PatrolPoint : PatrolPointList->children) {
							glm::vec3 pos = *(glm::vec3*)PatrolPoint.getAttribute("vecPos")->buffer.data();

							if (last != glm::vec3())
								dd::line(&last[0], &pos[0], red);
							else
								dd::projectedText((char*)hidName->buffer.data(), &pos.x, red, &renderInterface.VP[0][0], 0, 0, renderInterface.windowSize.x, renderInterface.windowSize.y, 0.5f);
							last = pos;
						}
					}

					Node* RaceDescription = entity.findFirstChild("RaceDescription");
					if (RaceDescription) {
						needsCross = false;
						Node* RacePointList = RaceDescription->findFirstChild("RacePointList");

						glm::vec3 last;
						for (Node& RacePoint : RacePointList->children) {
							glm::vec3 pos = *(glm::vec3*)RacePoint.getAttribute("vecPos")->buffer.data();
							float fShortcutRadius = *(float*)RacePoint.getAttribute("fShortcutRadius")->buffer.data();

							dd::sphere((float*)& pos.x, red, fShortcutRadius);

							if (last != glm::vec3())
								dd::line(&last[0], &pos[0], red);
							else
								dd::projectedText((char*)hidName->buffer.data(), &pos.x, red, &renderInterface.VP[0][0], 0, 0, renderInterface.windowSize.x, renderInterface.windowSize.y, 0.5f);
							last = pos;
						}
					}

					if (glm::distance(pos, camera.location) < settings.textDrawDistance)
						dd::projectedText((char*)hidName->buffer.data(), &pos.x, white, &renderInterface.VP[0][0], 0, 0, renderInterface.windowSize.x, renderInterface.windowSize.y, 0.5f);
					if (needsCross)
						dd::cross(&pos.x, 0.25f);

				}
			}
			ImGui::End();
		}

		if (!ImGui::IsAnyWindowHovered())
			camera.update(delta);

		glBindVertexArray(RenderInterface::instance().VertexArrayID);
		dd::xzSquareGrid(-50.0f, 50.0f, -1.0f, 1.7f, green);
		dd::flush(0);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
		frameCount++;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type) {
			case SDL_WINDOWEVENT: {
				switch(event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					windowOpen = false;
					saveSettings();
					exit(0);
				case SDL_WINDOWEVENT_RESIZED:
					settings.windowSize = glm::ivec2(event.window.data1, event.window.data2);
					saveSettings();
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					settings.maximized = true;
					saveSettings();
					break;
				case SDL_WINDOWEVENT_RESTORED:
					settings.maximized = false;
					saveSettings();
					break;
				}
				break;
			}
			}
		}
	}

	return 0;
}