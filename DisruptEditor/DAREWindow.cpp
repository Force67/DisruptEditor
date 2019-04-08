#include "ImguiWindows.h"

#include "imgui.h"
#include "Common.h"
#include "sbaoFile.h"
#include "spkFile.h"
#include "Audio.h"
#include "noc_file_dialog.h"
#include "dr_wav.h"
#include "DARE.h"

void UI::displayDARE() {
	if (!settings.openWindows["DARE"])
		return;
	if (!ImGui::Begin("DARE Converter", &settings.openWindows["DARE"], 0)) {
		ImGui::End();
		return;
	}

	static uint64_t inputSpk;
	ImGui::InputUInt64("Input", &inputSpk);
	if (ImGui::Button("Add"))
		DARE::instance().addSoundResource(inputSpk);
	if (ImGui::Button("Clear")) {
		DARE::instance().atomicObjects.clear();
	}

	if (ImGui::Button("Save All")) {
		for (auto it : DARE::instance().atomicObjects) {
			sbaoFile &sbao = it.second.ao;

			//displayImGui(sbao);

			if (sbao.resourceDescriptor) {
				if (sbao.resourceDescriptor->pResourceDesc.sampleResourceDescriptor) {
					SampleResourceDescriptor &srd = *sbao.resourceDescriptor->pResourceDesc.sampleResourceDescriptor;

					uint32_t spkID = it.second.spkFile;
					uint32_t sbaoID = srd.stToolSourceFormat.dataRef.refAtomicId;

					char buffer[80];
					snprintf(buffer, sizeof(buffer), "%08x_%08x.wav", spkID, sbaoID);
					srd.saveDecoded(buffer);
				}
			}

		}
	}

	if (ImGui::Button("Save All XML")) {
		for (auto it : DARE::instance().atomicObjects) {
			sbaoFile &sbao = it.second.ao;
			uint32_t spkID = it.second.spkFile;
			uint32_t sbaoID = it.first;

			char buffer[80];
			snprintf(buffer, sizeof(buffer), "%08x_%08x.spk.xml", spkID, sbaoID);

			std::string xml = serializeToXML(sbao);
			FILE *fp = fopen(buffer, "wb");
			fwrite(xml.c_str(), 1, xml.size(), fp);
			fclose(fp);

		}
	}

	for (auto it : DARE::instance().atomicObjects) {
		ImGui::Text("Atomic Object: %08x", it.first);
		sbaoFile &sbao = it.second.ao;
		ImGui::Text("Type: %s", sbao.type.getReverseName().c_str());

		//displayImGui(sbao);
		
		if (sbao.resourceDescriptor) {
			ImGui::Text("resourceDescriptor Type: %s", sbao.resourceDescriptor->pResourceDesc.type.getReverseName().c_str());
			if (sbao.resourceDescriptor->pResourceDesc.sampleResourceDescriptor) {
				SampleResourceDescriptor &srd = *sbao.resourceDescriptor->pResourceDesc.sampleResourceDescriptor;
				
				switch (srd.CompressionFormat) {
				case 2:
					ImGui::Text("ADPCM");
					break;
				case 4:
					ImGui::Text("OGG");
					break;
				default:
					ImGui::Text("Unknown audio format");
				}

				ImGui::PushID(&srd);
				if (ImGui::Button("Save recording.wav"))
					srd.saveDecoded("recording.wav");

				if (ImGui::Button("Replace")) {
					const char *newFile = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "wav\0*.wav\0", NULL, NULL);
					if (newFile) {
						unsigned int channels, sampleRate;
						drwav_uint64 totalSampleCount;
						short* pSampleData = drwav_open_and_read_file_s16(newFile, &channels, &sampleRate, &totalSampleCount);
						if (pSampleData) {
							srd.ulFreq = sampleRate;
							srd.ulNbChannels = channels;
							srd.CompressionFormat = 1;//PCM


							drwav_free(pSampleData);
						}
					}
				}

				ImGui::PopID();
			} else if (sbao.resourceDescriptor->pResourceDesc.multiTrackResourceDescriptor) {
				MultiTrackResourceDescriptor &mtrd = *sbao.resourceDescriptor->pResourceDesc.multiTrackResourceDescriptor;


				/*ImGui::PushID(&mtrd);
				if (ImGui::Button("Save recording.wav"))
					mtrd.saveDecoded("recording.wav", 0);
				ImGui::PopID();*/
			}
		}

		ImGui::Separator();
	}

	ImGui::End();
}
