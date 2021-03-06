#include "sbaoFile.h"
#include "sbaoFile.h"

#include <algorithm>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <SDL.h>
#include "Vector.h"
#include "IBinaryArchive.h"
#include "Adpcm.h"
#include "dr_wav.h"
#include "DARE.h"
#include "Hash.h"
#include "Audio.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#include "FileHandler.h"

static void StreamValidationPoint(IBinaryArchive & fp) {
	//Dare::StreamValidationPoint((SndGear::Serializer &))
	//TODO: CRC Hash?
	if (fp.isReading()) {
		uint32_t crc;
		fp.serialize(crc);
	} else {
		uint32_t crc = 0;
		fp.serialize(crc);
	}
}

static std::vector<short> decodeSoundData(uint32_t CompressionFormat, Vector<uint8_t> &rawData, uint32_t ulNbChannels, uint32_t ulFreq) {
	std::vector<short> decoded;
	switch (CompressionFormat) {
	case 1: {//PCM
		decoded.resize(rawData.size() / sizeof(short));
		memcpy(decoded.data(), rawData.data(), rawData.size());
		break;
	}
	case 2: {//ADPCM
		decoded.resize(rawData.size() * 16);//This should be a good enough buffer for decoding

		if (ulNbChannels == 2) {
			SAdpcmStereoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = rawData.data() + 32;
			param.InputLength = rawData.size() - 32;
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressStereoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		} else if (ulNbChannels == 1) {
			SAdpcmMonoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = rawData.data() + 32;
			param.InputLength = rawData.size() - 32;
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressMonoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		} else {
			SDL_assert_release(false);
		}

		break;
	}
	case 3: {//SeekableADPCM
		decoded.resize(rawData.size() * 16);//This should be a good enough buffer for decoding
		SDL_assert_release(false);

		//Header is 0x20
		//SubHeader is 0x4

		if (ulNbChannels == 2) {
			SAdpcmStereoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = rawData.data() + 32;
			param.InputLength = rawData.size() - 32;
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressStereoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		} else if (ulNbChannels == 1) {
			SAdpcmMonoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = rawData.data() + 32;
			param.InputLength = rawData.size() - 32;
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressMonoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		} else {
			SDL_assert_release(false);
		}

		break;
	}
	case 4: {//OGG
		int channels, sampleRate;
		short* output;
		int ret = stb_vorbis_decode_memory(rawData.data(), rawData.size(), &channels, &sampleRate, &output);
		SDL_assert_release(channels == ulNbChannels);
		SDL_assert_release(sampleRate == ulFreq);

		decoded.resize(ret * channels);
		memcpy(decoded.data(), output, ret * channels * sizeof(short));
		free(output);

		break;
	}
	default:
		SDL_assert_release(false);
	}
	return decoded;
}

template <typename T>
void serializeSharedPtr(std::shared_ptr<T>& ptr, IBinaryArchive &fp) {
	if (!ptr)
		ptr = std::make_shared<T>();
	ptr->read(fp);
}

void sbaoFile::open(IBinaryArchive & fp, size_t size) {
	fp.padding = IBinaryArchive::PADDING_GEAR;

	//Header Size: 0x1C
	uint32_t magic = 207362;
	fp.serialize(magic);
	SDL_assert_release(magic == 207362);

	fp.serialize(unk1);
	fp.serialize(unk2);
	fp.serialize(unk3);
	fp.serialize(unk4);
	fp.serialize(unk5);

	uint32_t two = 2;
	fp.serialize(two);
	SDL_assert_release(two == 2);

	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 *&) [with T1=Dare::Private::AtomicObject]
	
	//We need to know ahead of time if this is audio data or a object

	//This is for object
	if (!fp.isReading() && sndData) {
		fp.memBlock(sndData->rawData.data(), 1, sndData->rawData.size());
		fp.padding = IBinaryArchive::PADDING_IBINARYARCHIVE;
		return;
	} else {
		fp.serialize(type.id);
	}

	std::string typeName = type.getReverseName();

	if (typeName == "ResourceDescriptor") {
		serializeSharedPtr(resourceDescriptor, fp);
	} else if (typeName == "PlayEventDescriptor") {
		serializeSharedPtr(playEventDescriptor, fp);
	} else if (typeName == "MultiEventDescriptor") {
		serializeSharedPtr(multiEventDescriptor, fp);
	} else if (typeName == "PresetDescriptor") {
		serializeSharedPtr(presetDescriptor, fp);
	} else if (typeName == "PresetEventDescriptor") {
		serializeSharedPtr(presetEventDescriptor, fp);
	} else if (typeName == "StopEventDescriptor") {
		serializeSharedPtr(stopEventDescriptor, fp);
	} else if (typeName == "RemovePresetEventDescriptor") {
		serializeSharedPtr(removePresetEventDescriptor, fp);
	} else if (typeName == "ProjectDesc") {
		serializeSharedPtr(projectDesc, fp);
	} else if (typeName == "RolloffResourceDescriptor") {
		serializeSharedPtr(rolloffResourceDescriptor, fp);
	} else if (typeName == "EmitterSpec") {
		serializeSharedPtr(emitterSpec, fp);
	} else if (typeName == "ChangeVolumeEventDescriptor") {
		serializeSharedPtr(changeVolumeEventDescriptor, fp);
	} else if (typeName == "StopNGoEventDescriptor") {
		serializeSharedPtr(stopNGoEventDescriptor, fp);
	} else if (typeName == "SwitchEventDescriptor") {
		serializeSharedPtr(switchEventDescriptor, fp);
	} else if (typeName[0] != '_') {
		SDL_assert_release(false);
	} else {
		type = CDobbsID("SndData");

		//Assume this is sound data
		SDL_RWseek(fp.fp, -4, RW_SEEK_CUR);
		size_t rawDataSize = size - (7 * 4);
		sndData = std::make_shared<SndData>();
		sndData->rawData.resize(rawDataSize);
		fp.memBlock(sndData->rawData.data(), 1, rawDataSize);
	}

	fp.padding = IBinaryArchive::PADDING_IBINARYARCHIVE;
}

void sbaoFile::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(unk1);
	REGISTER_MEMBER(unk2);
	REGISTER_MEMBER(unk3);
	REGISTER_MEMBER(unk4);
	REGISTER_MEMBER(unk5);

	REGISTER_MEMBER(type);
	std::string typeName = type.getReverseName();
	if (typeName == "ResourceDescriptor")
		ms.registerMember(NULL, *resourceDescriptor);
	else if (typeName == "PlayEventDescriptor")
		ms.registerMember(NULL, *playEventDescriptor);
	else if (typeName == "MultiEventDescriptor")
		ms.registerMember(NULL, *multiEventDescriptor);
	else if (typeName == "PresetDescriptor")
		ms.registerMember(NULL, *presetDescriptor);
	else if (typeName == "PresetEventDescriptor")
		ms.registerMember(NULL, *presetEventDescriptor);
	else if (typeName == "StopEventDescriptor")
		ms.registerMember(NULL, *stopEventDescriptor);
	else if (typeName == "RemovePresetEventDescriptor")
		ms.registerMember(NULL, *removePresetEventDescriptor);
	else if (typeName == "ProjectDesc")
		ms.registerMember(NULL, *projectDesc);
	else if (typeName == "RolloffResourceDescriptor")
		ms.registerMember(NULL, *rolloffResourceDescriptor);
	else if (typeName == "EmitterSpec")
		ms.registerMember(NULL, *emitterSpec);
	else if (typeName == "ChangeVolumeEventDescriptor")
		ms.registerMember(NULL, *changeVolumeEventDescriptor);
	else if (typeName == "StopNGoEventDescriptor")
		ms.registerMember(NULL, *stopNGoEventDescriptor);
	else if (typeName == "SwitchEventDescriptor")
		ms.registerMember(NULL, *switchEventDescriptor);
	else if (typeName == "SndData")
		ms.registerMember(NULL, *sndData);
}

void ResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(Id);
	resVolume.read(fp);
	fp.serialize(bLocalised);
	globalLimiterInfo.read(fp);
	perSoundObjectLimiterInfo.read(fp);
	fp.serialize(eType);
	pResourceDesc.read(fp);
}

void ResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(Id);
	REGISTER_MEMBER(resVolume);
	REGISTER_MEMBER(bLocalised);
	REGISTER_MEMBER(globalLimiterInfo);
	REGISTER_MEMBER(perSoundObjectLimiterInfo);
	REGISTER_MEMBER(eType);
	REGISTER_MEMBER(pResourceDesc);
}

void ResourceVolume::read(IBinaryArchive & fp) {
	vol.read(fp);
	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 &) [with T1=Dare::TVolume<(long)0, Dare::NonPositiveVolumeTraits>]
	fp.serialize(delta_dB);
	fp.serialize(randomProbDist);
}

void ResourceVolume::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(vol);
	REGISTER_MEMBER(delta_dB);
	REGISTER_MEMBER(randomProbDist);
}

void RTPCVolume::read(IBinaryArchive & fp) {
	m_rtpc.read(fp);
	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 &) [with T1=Dare::TVolume<(long)1, Dare::NonPositiveVolumeTraits>]
	fp.serialize(m_volume_dB);
}

void RTPCVolume::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(m_rtpc);
	REGISTER_MEMBER(m_volume_dB);
}

void BaseResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(type.id);
	std::string typeName = type.getReverseName();

	//Base Attributes
	fp.serializeNdVectorExternal(emitterSpecs);

	if (typeName == "SampleResourceDescriptor") {
		serializeSharedPtr(sampleResourceDescriptor, fp);
	} else if (typeName == "RandomResourceDescriptor") {
		serializeSharedPtr(randomResourceDescriptor, fp);
	} else if (typeName == "SilenceResourceDescriptor") {
		serializeSharedPtr(silenceResourceDescriptor, fp);
	} else if (typeName == "MultiLayerResourceDescriptor") {
		serializeSharedPtr(multiLayerResourceDescriptor, fp);
	} else if (typeName == "SequenceResourceDescriptor") {
		serializeSharedPtr(sequenceResourceDescriptor, fp);
	} else if (typeName == "MultiTrackResourceDescriptor") {
		serializeSharedPtr(multiTrackResourceDescriptor, fp);
	} else if (typeName == "ThemeResourceDescriptor") {
		serializeSharedPtr(themeResourceDescriptor, fp);
	} else if (typeName == "GranularResourceDescriptor") {
		serializeSharedPtr(granularResourceDescriptor, fp);
	} else if (typeName == "SwitchResourceDescriptor") {
		serializeSharedPtr(switchResourceDescriptor, fp);
	} else {
		SDL_assert_release(false);
	}
}

void BaseResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(emitterSpecs);

	std::string typeName = type.getReverseName();
	if (typeName == "SampleResourceDescriptor")
		ms.registerMember(NULL, *sampleResourceDescriptor);
	else if (typeName == "RandomResourceDescriptor")
		ms.registerMember(NULL, *randomResourceDescriptor);
	else if (typeName == "SilenceResourceDescriptor")
		ms.registerMember(NULL, *silenceResourceDescriptor);
	else if (typeName == "MultiLayerResourceDescriptor")
		ms.registerMember(NULL, *multiLayerResourceDescriptor);
	else if (typeName == "SequenceResourceDescriptor")
		ms.registerMember(NULL, *sequenceResourceDescriptor);
	else if (typeName == "MultiTrackResourceDescriptor")
		ms.registerMember(NULL, *multiTrackResourceDescriptor);
	else if (typeName == "ThemeResourceDescriptor")
		ms.registerMember(NULL, *themeResourceDescriptor);
	else if (typeName == "GranularResourceDescriptor")
		ms.registerMember(NULL, *granularResourceDescriptor);
	else if (typeName == "SwitchResourceDescriptor")
		ms.registerMember(NULL, *switchResourceDescriptor);
}

void RTPC::read(IBinaryArchive & fp) {
	fp.serialize(rtpcID);
}

void RTPC::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, rtpcID);
}

void LimiterInfoDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_maxCount);
	fp.serialize(m_limitationRule);
}

void LimiterInfoDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(m_maxCount);
	REGISTER_MEMBER(m_limitationRule);
}

std::vector<short> SampleResourceDescriptor::decode() {
	SndData sndData;
	if (stToolSourceFormat.bStream) {
		sbaoFile &streamRef = DARE::instance().loadAtomicObject(stToolSourceFormat.streamRef.refAtomicId);
		//sbaoFile &uSndDataZeroLatencyMemPart = DARE::instance().loadAtomicObject(stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId);
		if (stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId != 0xFFFFFFFF) {
			sbaoFile& streamRefBegin = DARE::instance().loadAtomicObject(stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId);
			sndData.rawData = streamRefBegin.sndData->rawData;
			if (stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId == stToolSourceFormat.streamRef.refAtomicId) {
				char buffer[80];
				snprintf(buffer, sizeof(buffer), "soundbinary/%08x.sbao", stToolSourceFormat.streamRef.refAtomicId);

				SDL_RWops* fp = FH::openFile(buffer);
				if (fp) {
					std::shared_ptr<sbaoFile> sbao = std::make_shared<sbaoFile>();
					sbao->open(CBinaryArchiveReader(fp), SDL_RWsize(fp));
					SDL_RWclose(fp);
					sndData.rawData.insert(sndData.rawData.end(), sbao->sndData->rawData.begin(), sbao->sndData->rawData.end());
				}
			} else {
				sndData.rawData.insert(sndData.rawData.end(), streamRef.sndData->rawData.begin(), streamRef.sndData->rawData.end());
			}
		} else {
			sndData = *streamRef.sndData;
		}
	} else {
		sbaoFile &source = DARE::instance().loadAtomicObject(stToolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);
		sndData = *source.sndData;
	}

	return decodeSoundData(CompressionFormat, sndData.rawData, ulNbChannels, ulFreq);
}

void SampleResourceDescriptor::play() {
	std::vector<short> decoded = decode();
	Audio::instance().addSound(ulFreq, ulNbChannels, decoded.data(), decoded.size() * sizeof(short), false);
}

void SampleResourceDescriptor::saveDecoded(const char * file) {
	std::vector<short> decoded = decode();

	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_PCM;
	format.channels = ulNbChannels;
	format.sampleRate = ulFreq;
	format.bitsPerSample = 16;
	drwav* pWav = drwav_open_file_write(file, &format);
	drwav_uint64 samplesWritten = drwav_write(pWav, decoded.size(), decoded.data());
	drwav_close(pWav);
}

uint32_t SampleResourceDescriptor::getHelpfulId() {
	if (stToolSourceFormat.bStream) {
		return stToolSourceFormat.streamRef.refAtomicId;
	} else {
		return stToolSourceFormat.dataRef.refAtomicId;
	}
}

void SampleResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(bLooping);
	fp.serialize(bTool);
	fp.serialize(bIsNotifying);
	fp.serialize(ulLoopByte);
	fp.serialize(ulLoopSample);
	fp.serialize(ulBitRate);
	fp.serialize(ulResNotificationUserData);
	fp.serialize(CompressionFormat);
	fp.serialize(ulNbChannels);
	fp.serialize(ulFreq);
	fp.serialize(stToolSourceFormat);
	fp.serialize(stWaveMarkerList);
	fp.serialize(autoDuckingSetPresetEventId);
	fp.serialize(busId);
	fp.serialize(platformSpecificProperties);
	fp.serialize(ulAttackLengthSample);
	fp.serialize(ulAttackLengthByte);
	fp.serialize(ulLoopLengthSample);
	fp.serialize(ulLoopLengthByte);
}

void SampleResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(bLooping);
	REGISTER_MEMBER(bTool);
	REGISTER_MEMBER(bIsNotifying);
	REGISTER_MEMBER(ulLoopByte);
	REGISTER_MEMBER(ulLoopSample);
	REGISTER_MEMBER(ulBitRate);
	REGISTER_MEMBER(ulResNotificationUserData);
	REGISTER_MEMBER(CompressionFormat);
	REGISTER_MEMBER(ulNbChannels);
	REGISTER_MEMBER(ulFreq);
	REGISTER_MEMBER(stToolSourceFormat);
	REGISTER_MEMBER(stWaveMarkerList);
	REGISTER_MEMBER(autoDuckingSetPresetEventId);
	REGISTER_MEMBER(busId);
	REGISTER_MEMBER(platformSpecificProperties);
	REGISTER_MEMBER(ulAttackLengthSample);
	REGISTER_MEMBER(ulAttackLengthByte);
	REGISTER_MEMBER(ulLoopLengthSample);
	REGISTER_MEMBER(ulLoopLengthByte);
}

void SND_tdstToolSourceFormat::read(IBinaryArchive & fp) {
	fp.serialize(lTransferRate);
	fp.serialize(ulNbBytes);
	fp.serialize(ulNbSamples);
	fp.serialize(isMTTInterlaced);
	fp.serialize(bStream);

	if (bStream) {
		fp.serialize(bZeroLatency);
		fp.serialize(ZLMemPartInBytes);
		fp.serialize(ulOffsetData);
		fp.serialize(dataId);
		fp.serialize(streamRef);
		fp.serialize(uSndDataZeroLatencyMemPart);
	} else {
		fp.serialize(dataRef);
	}
}

void SND_tdstToolSourceFormat::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(lTransferRate);
	REGISTER_MEMBER(ulNbBytes);
	REGISTER_MEMBER(ulNbSamples);
	REGISTER_MEMBER(isMTTInterlaced);
	REGISTER_MEMBER(bStream);
	if (bStream) {
		REGISTER_MEMBER(bZeroLatency);
		REGISTER_MEMBER(ZLMemPartInBytes);
		REGISTER_MEMBER(ulOffsetData);
		REGISTER_MEMBER(dataId);
		REGISTER_MEMBER(streamRef);
		REGISTER_MEMBER(uSndDataZeroLatencyMemPart);
	}
	else {
		REGISTER_MEMBER(dataRef);
	}
}

void tdstWaveMarkerList::read(IBinaryArchive & fp) {
	fp.serialize(stringPool);
	fp.serializeNdVectorExternal(m_waveMarkers);
}

void tdstWaveMarkerList::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(stringPool);
	REGISTER_MEMBER(m_waveMarkers);
}

void DynamicIndexedPropertyContainer::read(IBinaryArchive & fp) {
	fp.serialize(size);
	SDL_assert_release(size == 0);

	fp.serialize(isBlob);
	if (isBlob) {
		uint32_t unk1;
		fp.serialize(unk1);
	} else {
		//Dare::DynamicIndexedPropertyContainer::NormalSerialize((SndGear::Serializer &, long))
		if (size > 0) {
			fp.serialize(rawSize);
		}
	}
}

void DynamicIndexedPropertyContainer::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(size);
	REGISTER_MEMBER(isBlob);
	REGISTER_MEMBER(rawSize);//TODO
}

void tdstWaveMarkerElement::read(IBinaryArchive & fp) {
	fp.serialize(fTimePos);
}

void tdstWaveMarkerElement::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, fTimePos);
}

void RandomResourceDescriptor::read(IBinaryArchive & fp) {
	uint32_t ulNbElements = elements.size();
	fp.serialize(ulNbElements);
	fp.serialize(bUseShuffle);

	uint32_t ulNbElements2 = ulNbElements;
	fp.serialize(ulNbElements2);
	SDL_assert_release(ulNbElements2 == ulNbElements);

	elements.resize(ulNbElements);
	for (uint32_t i = 0; i < ulNbElements; ++i)
		fp.serialize(elements[i]);
}

void RandomResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(bUseShuffle);
	REGISTER_MEMBER(elements);
}

void tdstRandomElement::read(IBinaryArchive & fp) {
	fp.serialize(resourceId);
	fp.serialize(prob);
	fp.serialize(bCanBeChosenTwice);
	fp.serialize(bHasPlayed);
}

void tdstRandomElement::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(resourceId);
	REGISTER_MEMBER(prob);
	REGISTER_MEMBER(bCanBeChosenTwice);
	REGISTER_MEMBER(bHasPlayed);
}

void PlayEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(resourceRef);
	fp.serialize(fDeTuneDelta);
	fp.serialize(fValPitchStat);
	fp.serialize(fFadeDuration);
	fp.serialize(eFadeType);
	StreamValidationPoint(fp);
}

void PlayEventDescriptor::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, pBase);
	REGISTER_MEMBER(resourceRef);
	REGISTER_MEMBER(fDeTuneDelta);
	REGISTER_MEMBER(fValPitchStat);
	REGISTER_MEMBER(fFadeDuration);
	REGISTER_MEMBER(eFadeType);
}

void EventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(Id);
	fp.serialize(eType);
	fp.serialize(globalLimiterInfo);
	fp.serialize(perSoundObjectLimiterInfo);
	fp.serialize(bDynamic);
	fp.serialize(bLinkable);
	fp.serialize(bSynthable);
	fp.serialize(applyVirtBehaviorWhenInaudible);
	fp.serialize(virtualizationLogic);
	fp.serialize(lPrio);
	fp.serialize(delay);
	fp.serialize(rtpcId);
	fp.serialize(isLoopingChildResource);
	fp.serialize(tempoBPM);
	fp.serialize(tempoTimeSignature);
	fp.serialize(playOnCue);
}

void EventDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(Id);
	REGISTER_MEMBER(eType);
	REGISTER_MEMBER(globalLimiterInfo);
	REGISTER_MEMBER(perSoundObjectLimiterInfo);
	REGISTER_MEMBER(bDynamic);
	REGISTER_MEMBER(bLinkable);
	REGISTER_MEMBER(bSynthable);
	REGISTER_MEMBER(applyVirtBehaviorWhenInaudible);
	REGISTER_MEMBER(virtualizationLogic);
	REGISTER_MEMBER(lPrio);
	REGISTER_MEMBER(delay);
	REGISTER_MEMBER(rtpcId);
	REGISTER_MEMBER(isLoopingChildResource);
	REGISTER_MEMBER(tempoBPM);
	REGISTER_MEMBER(tempoTimeSignature);
	REGISTER_MEMBER(playOnCue);
}

void Delay::read(IBinaryArchive & fp) {
	fp.serialize(delayTime);
	fp.serialize(delayTimeDelta);
}

void Delay::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(delayTime);
	REGISTER_MEMBER(delayTimeDelta);
}

void EmitterSpec::read(IBinaryArchive & fp) {
	fp.serialize(m_id);
	fp.serialize(m_volume);
	fp.serialize(m_volumesMatrices);
	fp.serialize(m_positioned);
	fp.serialize(m_dopplerEffect);
	fp.serialize(m_speakerPanning);
	fp.serialize(m_playOnWiimote);
	fp.serialize(m_useEmitterCone);
	fp.serialize(m_rolloffId);
	fp.serialize(m_pSpreadFactor);
	fp.serialize(m_pWetVolume);
	fp.serialize(m_pLPFCutoffFrequency);
	fp.serialize(m_emitterAudibilityCone);
	fp.serializeNdVectorExternal_pod(m_effectIds);
	fp.serialize(m_activeSpeakers);
	StreamValidationPoint(fp);
}

void EmitterSpec::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(m_id);
	REGISTER_MEMBER(m_volume);
	REGISTER_MEMBER(m_volumesMatrices);
	REGISTER_MEMBER(m_positioned);
	REGISTER_MEMBER(m_dopplerEffect);
	REGISTER_MEMBER(m_speakerPanning);
	REGISTER_MEMBER(m_playOnWiimote);
	REGISTER_MEMBER(m_useEmitterCone);
	REGISTER_MEMBER(m_rolloffId);
	REGISTER_MEMBER(m_pSpreadFactor);
	REGISTER_MEMBER(m_pWetVolume);
	REGISTER_MEMBER(m_pLPFCutoffFrequency);
	REGISTER_MEMBER(m_emitterAudibilityCone);
	REGISTER_MEMBER(m_effectIds);
	REGISTER_MEMBER(m_activeSpeakers);
}

void SilenceResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(fLength);
	fp.serialize(busId);
}

void MultiLayerResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(m_layers);
}

void MultiLayerResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(m_layers);
}

void tdstMultiLayerElement::read(IBinaryArchive & fp) {
	fp.serialize(uRes);
	fp.serialize(activationFlagId);
	fp.serialize(invFlag);
	fp.serializeNdVectorExternal(m_effectGraphs);
}

void tdstMultiLayerElement::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(uRes);
	REGISTER_MEMBER(activationFlagId);
	REGISTER_MEMBER(invFlag);
	REGISTER_MEMBER(m_effectGraphs);
}

void tdstEffectGraph::read(IBinaryArchive & fp) {
	fp.serialize(eEffectID);
	fp.serialize(multiLayerParameterId);
	fp.serializeNdVectorExternal(m_coordinates);
}

void tdstEffectGraph::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(eEffectID);
	REGISTER_MEMBER(multiLayerParameterId);
	REGISTER_MEMBER(m_coordinates);
}

void MultiEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serializeNdVectorExternal(m_events);
}

void MultiEventDescriptor::registerMembers(MemberStructure & ms) {
	ms.registerMember(NULL, pBase);
	REGISTER_MEMBER(m_events);
}

void PresetDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(id);
	fp.serialize(type);
	fp.serializeNdVectorExternal(micPresetList);
	fp.serializeNdVectorExternal(effectPresetInfos);
	fp.serializeNdVectorExternal(busPresetInfos);
	//SndGear::Serializer &DareSerializeMemberFilter<T1>(SndGear::Serializer &, const char *, T1 &) [with T1=long]
	fp.serialize(priority);
}

void MicPresetDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(mask);
	fp.serialize(spec);
}

void MicSpecDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(micAtomicId);
	fp.serialize(volumeInDecibels);
	fp.serialize(fadeDuration);
	fp.serialize(fadeType);
	fp.serialize(rolloffId);
	fp.serialize(useCone);
	fp.serialize(cone);
}

void EffectPresetInfo::read(IBinaryArchive & fp) {
	fp.serialize(effectId);
	fp.serializeNdVectorExternal(paramsToChange);
}

void BusPresetInfo::read(IBinaryArchive & fp) {
	fp.serialize(busId);
	fp.serializeNdVectorExternal(paramsToChange);
}

void ParamInfo::read(IBinaryArchive & fp) {
	fp.serialize(fadeInType);
	fp.serialize(fadeInDuration);
	fp.serialize(duration);
	fp.serialize(fadeOutType);
	fp.serialize(fadeOutDuration);
	fp.serialize(absoluteChange);
	fp.serialize(paramValue);
}

void ParameterValue::read(IBinaryArchive & fp) {
	fp.serialize(parameterType);
	fp.serialize(parameterIndex);
	fp.serialize(rtpcId);
	switch (parameterType) {
	case 1:
		fp.serialize(valueSndS32);
		break;
	case 2:
		fp.serialize(valueSndFloat);
		break;
	case 3:
		fp.serializeNdVectorExternal_pod(valueListSndS32);
		break;
	case 4:
		fp.serializeNdVectorExternal_pod(valueListSndFloat);
		break;
	case 0:
		//None
		break;
	default:
		SDL_assert_release(false);
	}
}

void PresetEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(presetRef);
	StreamValidationPoint(fp);
}

void StopEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(uEvt);
	fp.serialize(fFadeDuration);
	fp.serialize(eFadeType);
	StreamValidationPoint(fp);
}

void tdstCoordinate::read(IBinaryArchive & fp) {
	fp.serialize(xFloat);
	fp.serialize(yFloat);
	fp.serialize(curvType);
	fp.serialize(curveFactor);
}

void SequenceResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(bLoop);
	fp.serialize(ulStartLoop);
	fp.serialize(ulEndLoop);
	fp.serialize(ulNbLoops);
	fp.serialize(fLength);
	fp.serialize(fPosMainReLoop);
	fp.serializeNdVectorExternal(sequences);
}

std::vector<short> MultiTrackResourceDescriptor::decode(int layer) {
	SDL_RWops* mem;
	Vector<uint8_t> tempRawData;
	if (m_tracksRawData.data.empty()) {
		char buffer[80];
		snprintf(buffer, sizeof(buffer), "soundbinary/%08x.sbao", currentResourceID);

		SDL_RWops* fp = FH::openFile(buffer);
		if (fp) {
			std::shared_ptr<sbaoFile> sbao = std::make_shared<sbaoFile>();
			sbao->open(CBinaryArchiveReader(fp), SDL_RWsize(fp));
			SDL_RWclose(fp);
			tempRawData = sbao->sndData->rawData;
			mem = SDL_RWFromConstMem(tempRawData.data(), tempRawData.size());
		}
	} else {
		mem = SDL_RWFromConstMem(m_tracksRawData.data.data(), m_tracksRawData.data.size());
	}

	CBinaryArchiveReader fp(mem);
	fp.padding = fp.PADDING_NONE;

	uint32_t type = 1048585;
	fp.serialize(type);
	SDL_assert_release(type == 1048585);

	uint32_t zero = 0;
	fp.serialize(zero);
	SDL_assert_release(zero == 0);

	uint32_t numLayers = ulNbTrack;
	fp.serialize(numLayers);
	SDL_assert_release(numLayers == ulNbTrack);

	uint32_t totalBlocks;
	fp.serialize(totalBlocks);

	Vector<uint8_t> infoTable;
	fp.serializeNdVectorExternal_pod(infoTable);
	//TODO: Figure this out
	SDL_RWseek(fp.fp, 64 - numLayers * 4, RW_SEEK_CUR);

	Vector<uint32_t> headerSizes(numLayers);
	for (uint32_t i = 0; i < numLayers; ++i)
		fp.serialize(headerSizes[i]);

	Vector< Vector<uint8_t> > data(numLayers);
	//Read Headers
	for (uint32_t i = 0; i < numLayers; ++i) {
		data[i].resize(headerSizes[i]);
		fp.memBlock(data[i].data(), 1, headerSizes[i]);
	}

	//Read Blocks
	for (uint32_t i = 0; i < totalBlocks; ++i) {
		uint32_t blockID;
		fp.serialize(blockID);

		uint32_t unk;
		fp.serialize(unk);

		Vector<uint32_t> blockSizes(numLayers);
		for (uint32_t j = 0; j < numLayers; ++j)
			fp.serialize(blockSizes[j]);

		for (uint32_t j = 0; j < numLayers; ++j) {
			Vector<uint8_t> blockData(blockSizes[j]);
			fp.memBlock(blockData.data(), 1, blockSizes[j]);
			data[j].insert(data[j].end(), blockData.begin(), blockData.end());
		}
	}

	return decodeSoundData(m_tracks[layer].CompressionFormat, data[layer], m_tracks[layer].ulNbChannels, m_tracks[layer].ulFreq);
}

void MultiTrackResourceDescriptor::play(int layer) {
	std::vector<short> decoded = decode(layer);
	Audio::instance().addSound(m_tracks[layer].ulFreq, m_tracks[layer].ulNbChannels, decoded.data(), decoded.size() * sizeof(short), false);
}

void MultiTrackResourceDescriptor::saveDecoded(const char* file, int layer) {
	std::vector<short> decoded = decode(layer);

	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_PCM;
	format.channels = m_tracks[layer].ulNbChannels;
	format.sampleRate = m_tracks[layer].ulFreq;
	format.bitsPerSample = 16;
	drwav* pWav = drwav_open_file_write(file, &format);
	drwav_uint64 samplesWritten = drwav_write(pWav, decoded.size(), decoded.data());
	drwav_close(pWav);
}

void MultiTrackResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(bLoop);
	fp.serialize(bIsNotifying);
	fp.serialize(ulNbTrack);
	fp.serialize(ulResNotificationUserData);
	fp.serialize(multiTrackChannelId);
	fp.serialize(stToolSourceFormat);
	fp.serialize(stWaveMarkerList);
	fp.serialize(autoDuckingSetPresetEventId);
	fp.serialize(busId);
	fp.serializeNdVectorExternal(m_tracks);
	fp.serialize(m_tracksRawData);
	fp.serialize(platformSpecificProperties);
	fp.serialize(currentResourceID);
	StreamValidationPoint(fp);
}

void tdstMultiTrackElement::read(IBinaryArchive & fp) {
	fp.serialize(ulFreq);
	fp.serialize(ulNbChannels);
	fp.serialize(CompressionFormat);
	fp.serialize(trackVolume);
	fp.serialize(ulNbSamples);
	fp.serialize(ulNbBytes);
	fp.serialize(ulBitRate);
	fp.serialize(platformSpecificProperties);
}

void DataBlock::read(IBinaryArchive & fp) {
	uint32_t m_size = data.size();
	fp.serialize(m_size);
	fp.serialize(m_allocInfos);
	fp.serialize(m_allocatorType);
	fp.serialize(m_memoryType);

	data.resize(m_size);
	fp.memBlock(data.data(), 1, m_size);
}

void RemovePresetEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(presetId);
	StreamValidationPoint(fp);
}

void ThemeResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(ulStartLoop);
	fp.serialize(ulNbLoopsulNbLoops);
	fp.serialize(bStartImmediatly);
	fp.serialize(bIsNotifying);
	fp.serialize(bStream);
	fp.serialize(ulResNotificationUserData);
	fp.serialize(eTransition);
	fp.serialize(eIndexFadeInType);
	fp.serialize(eIndexFadeOutType);
	fp.serialize(pstPartOutro);
	fp.serialize(fThemeLength);
	fp.serialize(fPosMainReLoop);
	fp.serialize(ulTracksFading);
	fp.serialize(indexFadeInDur);
	fp.serialize(indexFadeOutDur);
	fp.serializeNdVectorExternal(m_themParts);
}

void ThemePartOutroDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(type.id);
	std::string typeName = type.getReverseName();

	if (typeName == "ThemePartOutroDescriptor") {
		fp.serialize(resRef);
		fp.serialize(fPos);
	}
}

void ThemePartDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(resRef);
	fp.serialize(bLoopStart);
	fp.serialize(bLoopEnd);
	fp.serialize(lNbLoops);
	fp.serialize(fLength);
}

void ProjectDesc::read(IBinaryArchive & fp) {
	fp.serialize(serializerVersion);
	fp.serialize(lProjectVersion);
	fp.serialize(bLocalised);
	fp.serialize(minStreamingPrefetchBufferLength);
	fp.serialize(stObstructionPreset);
	fp.serializeNdVectorExternal(stOcclusionPresetList);
	fp.serializeNdVectorExternal_pod(stMTTChannelList);
	fp.serializeNdVectorExternal(stSoundTextureList);
	fp.serializeNdVectorExternal(MultiLayerParameters);
	fp.memBlock(cTitleGuid.data(), 1, sizeof(cTitleGuid));
	fp.memBlock(cProjectDataVersion.data(), 1, sizeof(cProjectDataVersion));
	fp.serialize(projectBusDataDescBin);
	fp.serialize(projectEffectDataDescBin);
	fp.serialize(projectRTPCDataDescBin);
	fp.serialize(projectMicDataDescBin);
	fp.serialize(dopplerMicAtomicId);
	fp.serialize(streamLimiterInfo);
	fp.serialize(polyphonyLimiterInfo);

	StreamValidationPoint(fp);
}

void tdstObstructionPreset::read(IBinaryArchive & fp) {
	fp.serialize(bUseSoftwareFilterObstruction);
	fp.serialize(fLowPassMinFreq);
	fp.serialize(fLowPassMaxFreq);
}

void tdstOcclusionPreset::read(IBinaryArchive & fp) {
	fp.serialize(materialId);
	fp.serialize(bUseSoftwareFilterOcclusion);
	fp.serialize(Portable);
	fp.serialize(fSoftwareOcclusion);
	fp.serialize(platformSpecificProps);
}

void tdstOcclusionPortable::read(IBinaryArchive & fp) {
	fp.serialize(fBandPassCenterFrequency);
	fp.serialize(fBandPassBandWidth);
	fp.serialize(fGain);
	fp.serialize(ulActiveFilters);
}

void tdstSoundTexture::read(IBinaryArchive & fp) {
	fp.serialize(textureId);
	fp.serialize(resourceId);
	fp.serialize(fReadRate);
	fp.serialize(fContactDuration);
}

void tdstMultiLayerParameter::read(IBinaryArchive & fp) {
	fp.serialize(Id);
	fp.serialize(fMin);
	fp.serialize(fMax);
}

void RolloffResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(m_pointList);
	StreamValidationPoint(fp);
}

void tdstRollOffPoint::read(IBinaryArchive & fp) {
	fp.serialize(m_distance);
	fp.serialize(m_decibel);
	fp.serialize(m_interpolationCurveType);
	fp.serialize(m_interpolationCurveFactor);
}

void RTPCVolumeMatrices::read(IBinaryArchive & fp) {
	for (int i = 0; i < 6; ++i)
		fp.serialize(volume[i]);
	for (int i = 0; i < 0xC; ++i)
		fp.serialize(volume2[i]);
}

void SND_Cone::read(IBinaryArchive & fp) {
	fp.serialize(m_InnerAngle);
	fp.serialize(m_OuterAngle);
	fp.serialize(m_InnerVolumeDB);
	fp.serialize(m_OuterVolumeDB);
	fp.serialize(m_InnerLPF);
	fp.serialize(m_OuterLPF);
}

void RTPCCone::read(IBinaryArchive & fp) {
	fp.serialize(cone);
	fp.serialize(m_innerAngleRTPC);
	fp.serialize(m_outerAngleRTPC);
	fp.serialize(m_innerVolumeRTPC);
	fp.serialize(m_outerVolumeRTPC);
	fp.serialize(m_innerLPFRTPC);
	fp.serialize(m_outerLPFRTPC);
}

void ProjectBusDataDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(busDescList);
	fp.serializeNdVectorExternal(busTreeDescList);
}

void BusDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(id);
	fp.serialize(busType);
	fp.serialize(parentBusId);
	fp.serializeNdVectorExternal_pod(effectIds);
	fp.serialize(preEffectVolume);
	fp.serialize(connectedVoicesVolume);
	fp.serialize(postEffectChannelVolumes);
	fp.serialize(pitchSemiTone);
	fp.serialize(processingStage);
	fp.serialize(childBusesCount);
	fp.serialize(onReverbPath);
	fp.serialize(voiceLimiterInfo);
}

void BusTreeDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(id);
	fp.serializeNdVectorExternal_pod(busIdList);
}

void VolumeHelper::read(IBinaryArchive & fp) {
	fp.serialize(volume);
}

void BusChannelVolumes::read(IBinaryArchive & fp) {
	fp.serialize(m_masterVolume);
	fp.serialize(m_dryChannelsVolumes);
	fp.serialize(m_wetChannelsVolumes);
}

void ProjectEffectDataDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(effectDescList);
}

void EffectDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(id);
	fp.serialize(effectType);
	fp.serialize(effectParameters);
}

void EffectParameters::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(parameterValueList);
}

void RTPCsAndVariablesDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(m_variableList);
	fp.serializeNdVectorExternal(m_rtpcList);
}

void RTVariableDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_varId);
	fp.serialize(m_defaultValue);
	fp.serialize(m_isDistanceBased);
}

void RTPCDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal_pod(m_varIdList);
	fp.serializeNdVectorExternal(m_graphList);
	fp.serializeNdVectorExternal(m_parameterList);
	fp.serialize(m_rtpcId);
}

void GraphCoordinate::read(IBinaryArchive & fp) {
	fp.serialize(x);
	fp.serialize(y);
	fp.serialize(interpolationCurveType);
	fp.serialize(interpolationCurveFactor);
}

void RTGraphDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_variableType);
	fp.serialize(m_parameterType);
	fp.serializeNdVectorExternal(m_pointList);
}

void RTParameterDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_type);
	fp.serialize(m_targetId);
	fp.serialize(m_fieldIndex);
}

void ProjectMicDataDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(micSpecList);
}

std::vector<short> GranularResourceDescriptor::decode() {
	SndData sndData;
	if (m_toolSourceFormat.bStream) {
		sbaoFile& streamRef = DARE::instance().loadAtomicObject(m_toolSourceFormat.streamRef.refAtomicId);
		//sbaoFile &uSndDataZeroLatencyMemPart = DARE::instance().loadAtomicObject(stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId);
		SDL_assert_release(m_toolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId == 0xFFFFFFFF);
		sndData = *streamRef.sndData;
	}
	else {
		sbaoFile& source = DARE::instance().loadAtomicObject(m_toolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);
		sndData = *source.sndData;
	}

	return decodeSoundData(m_compression, sndData.rawData, m_numChannels, m_freq);
}

void GranularResourceDescriptor::play() {
	std::vector<short> decoded = decode();
	Audio::instance().addSound(m_freq, m_numChannels, decoded.data(), decoded.size() * sizeof(short), false);
}

void GranularResourceDescriptor::saveDecoded(const char* file) {
	std::vector<short> decoded = decode();

	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_PCM;
	format.channels = m_numChannels;
	format.sampleRate = m_freq;
	format.bitsPerSample = 16;
	drwav* pWav = drwav_open_file_write(file, &format);
	drwav_uint64 samplesWritten = drwav_write(pWav, decoded.size(), decoded.data());
	drwav_close(pWav);
}

uint32_t GranularResourceDescriptor::getHelpfulId() {
	if (m_toolSourceFormat.bStream) {
		return m_toolSourceFormat.streamRef.refAtomicId;
	}
	else {
		return m_toolSourceFormat.dataRef.refAtomicId;
	}
}

void GranularResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_idleStop);
	fp.serialize(m_accStart);
	fp.serialize(m_accStop);
	fp.serialize(m_maxStart);
	fp.serialize(m_maxStop);
	fp.serialize(m_decStart);
	fp.serialize(m_numChannels);
	fp.serialize(m_freq);
	fp.serialize(m_busId);
	fp.serialize(m_autoDuckingSetPresetEventId);
	fp.serialize(m_isDecelerationEnabled);
	fp.serialize(m_granuleMaxSize);
	fp.serialize(m_granuleMinSize);
	fp.serialize(m_constantGranuleMaxSize);
	fp.serialize(m_constantGranuleMinSize);
	fp.serialize(m_maxGranuleShuffle);
	fp.serialize(m_smoothfactor);
	fp.serialize(m_granuleOverlap);
	fp.serialize(m_compression);
	fp.serialize(m_toolSourceFormat);
	fp.serialize(m_granuleAligmentRes);
	fp.serializeNdVectorExternal(m_pitchInfo);
}

void GranularPitchInfo::read(IBinaryArchive & fp) {
	fp.serialize(m_freq);
	fp.serialize(m_samplePos);
}

void ChangeVolumeEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(bApplyOnObjectType);
	fp.serialize(newVolume_dB);
	fp.serialize(fFadeDuration);
	fp.serialize(eFadeType);
	fp.serialize(lTrackID);
	fp.serialize(multiTrackChannelId);
	fp.serialize(emitterSpecId);
	StreamValidationPoint(fp);
}

void SwitchResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serializeNdVectorExternal(m_elements);
	fp.serialize(switchTypeId);
	fp.serialize(defaultSwitchValueId);
	fp.serialize(bDynamic);
}

void tdstSwitchElement::read(IBinaryArchive & fp) {
	fp.serialize(resourceId);
	fp.serialize(switchValueId);
}

void StopNGoEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(uEvtStop);
	fp.serialize(uEvtGo);
	fp.serialize(fFadeDuration);
	fp.serialize(eFadeType);
	fp.serialize(bCrossFade);
	fp.serialize(bStopAllEvents);
	StreamValidationPoint(fp);
}

void SwitchEventDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(pBase);
	fp.serialize(switchTypeId);
	fp.serialize(defaultEvent);
	fp.serializeNdVectorExternal(m_elements);
	StreamValidationPoint(fp);
}

void SwitchEventDescriptorElement::read(IBinaryArchive & fp) {
	fp.serialize(eventRef);
	fp.serialize(switchValueId);
}

void StringPool::read(IBinaryArchive & fp) {
	if (fp.isReading()) {
		uint32_t size;
		fp.serialize(size);
		Vector<char> data(size);
		fp.memBlock(data.data(), 1, size);

		strings.clear();
		std::string temp;
		auto it = data.begin();
		while (it != data.end()) {
			if (*it == '\0') {
				strings.push_back(temp);
				temp.clear();
			} else {
				temp += *it;
			}

			++it;
		}
		if (!temp.empty())
			strings.push_back(temp);
	}
	else {
		Vector<char> data;
		for (std::string str : strings) {

		}

		uint32_t size = data.size();
		fp.serialize(size);
		fp.memBlock(data.data(), 1, data.size());
	}
}

void addReferenceSpk(uint32_t ID) {
	DARE::instance().addAODependency(ID);
}

void addReferenceSbao(uint32_t ID) {
	DARE::instance().addSndDataDependency(ID);
}
