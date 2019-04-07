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

static void StreamValidationPoint(IBinaryArchive & fp) {
	//Dare::StreamValidationPoint((SndGear::Serializer &))
	//TODO: CRC Hash?
	if (fp.isReading()) {
		uint32_t crc;
		fp.serialize(crc);
	} else {
		SDL_assert_release(false);
	}
}

sbaoFile::sbaoFile() {
}

sbaoFile::~sbaoFile() {
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
	fp.serialize(type.id);
	std::string typeName = type.getReverseName();

	if (typeName == "ResourceDescriptor") {
		resourceDescriptor = std::make_shared<ResourceDescriptor>();
		resourceDescriptor->read(fp);
	} else if (typeName == "PlayEventDescriptor") {
		playEventDescriptor = std::make_shared<PlayEventDescriptor>();
		playEventDescriptor->read(fp);
	} else if (typeName == "MultiEventDescriptor") {
		multiEventDescriptor = std::make_shared<MultiEventDescriptor>();
		multiEventDescriptor->read(fp);
	} else if (typeName == "PresetDescriptor") {
		presetDescriptor = std::make_shared<PresetDescriptor>();
		presetDescriptor->read(fp);
	} else if (typeName == "PresetEventDescriptor") {
		presetEventDescriptor = std::make_shared<PresetEventDescriptor>();
		presetEventDescriptor->read(fp);
	} else if (typeName == "StopEventDescriptor") {
		stopEventDescriptor = std::make_shared<StopEventDescriptor>();
		stopEventDescriptor->read(fp);
	} else if (typeName == "RemovePresetEventDescriptor") {
		removePresetEventDescriptor = std::make_shared<RemovePresetEventDescriptor>();
		removePresetEventDescriptor->read(fp);
	} else if (typeName == "ProjectDesc") {
		projectDesc = std::make_shared<ProjectDesc>();
		projectDesc->read(fp);
	} else if (typeName == "RolloffResourceDescriptor") {
		rolloffResourceDescriptor = std::make_shared<RolloffResourceDescriptor>();
		rolloffResourceDescriptor->read(fp);
	} else if (typeName == "EmitterSpec") {
		emitterSpec = std::make_shared<EmitterSpec>();
		emitterSpec->read(fp);
	}
	else if (typeName[0] != '_') {
		SDL_assert_release(false);
	}
	else {
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
		REGISTER_MEMBER(*resourceDescriptor);
	else if (typeName == "PlayEventDescriptor")
		REGISTER_MEMBER(*playEventDescriptor);
	else if (typeName == "MultiEventDescriptor")
		REGISTER_MEMBER(*multiEventDescriptor);
	else if (typeName == "PresetDescriptor")
		REGISTER_MEMBER(*presetDescriptor);
	else if (typeName == "PresetEventDescriptor")
		REGISTER_MEMBER(*presetEventDescriptor);
	else if (typeName == "StopEventDescriptor")
		REGISTER_MEMBER(*stopEventDescriptor);
	else if (typeName == "RemovePresetEventDescriptor")
		REGISTER_MEMBER(*removePresetEventDescriptor);
	else if (typeName == "ProjectDesc")
		REGISTER_MEMBER(*projectDesc);
	else if (typeName == "RolloffResourceDescriptor")
		REGISTER_MEMBER(*rolloffResourceDescriptor);
	else if (typeName == "EmitterSpec")
		REGISTER_MEMBER(*emitterSpec);
	else if (typeName == "SndData")
		REGISTER_MEMBER(*sndData);
}

void ResourceDescriptor::read(IBinaryArchive & fp) {
	SDL_Log("%u", fp.tell());

	fp.serialize(Id);
	resVolume.read(fp);
	SDL_Log("%u", fp.tell());
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
		sampleResourceDescriptor = std::make_shared<SampleResourceDescriptor>();
		sampleResourceDescriptor->read(fp);
	} else if (typeName == "RandomResourceDescriptor") {
		randomResourceDescriptor = std::make_shared<RandomResourceDescriptor>();
		randomResourceDescriptor->read(fp);
	} else if (typeName == "SilenceResourceDescriptor") {
		silenceResourceDescriptor = std::make_shared<SilenceResourceDescriptor>();
		silenceResourceDescriptor->read(fp);
	} else if (typeName == "MultiLayerResourceDescriptor") {
		multiLayerResourceDescriptor = std::make_shared<MultiLayerResourceDescriptor>();
		multiLayerResourceDescriptor->read(fp);
	} else if (typeName == "SequenceResourceDescriptor") {
		sequenceResourceDescriptor = std::make_shared<SequenceResourceDescriptor>();
		sequenceResourceDescriptor->read(fp);
	} else if (typeName == "MultiTrackResourceDescriptor") {
		multiTrackResourceDescriptor = std::make_shared<MultiTrackResourceDescriptor>();
		multiTrackResourceDescriptor->read(fp);
	} else if (typeName == "ThemeResourceDescriptor") {
		themeResourceDescriptor = std::make_shared<ThemeResourceDescriptor>();
		themeResourceDescriptor->read(fp);
	}
	else {
		SDL_assert_release(false);
	}
}

void BaseResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(type);
	REGISTER_MEMBER(emitterSpecs);

	std::string typeName = type.getReverseName();
	if (typeName == "SampleResourceDescriptor")
		REGISTER_MEMBER(*sampleResourceDescriptor);
	if (typeName == "RandomResourceDescriptor")
		REGISTER_MEMBER(*randomResourceDescriptor);
	if (typeName == "SilenceResourceDescriptor")
		REGISTER_MEMBER(*silenceResourceDescriptor);
	if (typeName == "MultiLayerResourceDescriptor")
		REGISTER_MEMBER(*multiLayerResourceDescriptor);
	if (typeName == "SequenceResourceDescriptor")
		REGISTER_MEMBER(*sequenceResourceDescriptor);
	if (typeName == "MultiTrackResourceDescriptor")
		REGISTER_MEMBER(*multiTrackResourceDescriptor);
	if (typeName == "ThemeResourceDescriptor")
		REGISTER_MEMBER(*themeResourceDescriptor);
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
		SDL_assert_release(stToolSourceFormat.uSndDataZeroLatencyMemPart.refAtomicId == 0xFFFFFFFF);
		sndData = *streamRef.sndData;
	} else {
		sbaoFile &source = DARE::instance().loadAtomicObject(stToolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);
		sndData = *source.sndData;
	}

	std::vector<short> decoded;
	switch (CompressionFormat) {
	case 1: {//PCM
		decoded.resize(sndData.rawData.size() / sizeof(short));
		memcpy(decoded.data(), sndData.rawData.data(), sndData.rawData.size());
		break;
	}
	case 2: {//ADPCM
		decoded.resize(sndData.rawData.size() * 16);//This should be a good enough buffer for decoding

		if (ulNbChannels == 2) {
			SAdpcmStereoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = sndData.rawData.data() + (44 + 20 + 4);//44 + 20 is what i found made the ADPCM play with minimal glitches
			param.InputLength = sndData.rawData.size() - (44 + 20 + 4);
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressStereoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		}
		else if (ulNbChannels == 1) {
			SAdpcmMonoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = sndData.rawData.data() + (44 + 20 + 4);//44 + 20 is what i found made the ADPCM play with minimal glitches
			param.InputLength = sndData.rawData.size() - (44 + 20 + 4);
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressMonoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		} 
		else {
			SDL_assert_release(false);
		}

		break;
	}
	case 4: {//OGG
		int channels, sampleRate;
		short *output;
		int ret = stb_vorbis_decode_memory(sndData.rawData.data(), sndData.rawData.size(), &channels, &sampleRate, &output);
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

void SampleResourceDescriptor::play() {
	std::vector<short> decoded = decode();

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
	fp.serializeNdVectorExternal_pod(stringPool);
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
	fp.serialize(m_size);
	fp.serialize(m_allocInfos);
	fp.serialize(m_allocatorType);
	fp.serialize(m_memoryType);

	SDL_assert_release(m_size == 0);//TODO
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
	SDL_assert_release(type.id == 0);
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
