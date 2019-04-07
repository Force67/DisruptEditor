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
	}
	else if (typeName[0] != '_') {
		SDL_assert_release(false);
	}
	else {
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
	std::vector<short> decoded;
	switch (CompressionFormat) {
	case 1: {//PCM
		//Find the SourceObject
		sbaoFile source = DARE::instance().loadAtomicObject(stToolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);

		decoded.resize(source.sndData->rawData.size() / sizeof(short));
		memcpy(decoded.data(), source.sndData->rawData.data(), source.sndData->rawData.size());
		break;
	}
	case 2: {//ADPCM
		//Find the SourceObject
		sbaoFile source = DARE::instance().loadAtomicObject(stToolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);
		decoded.resize(source.sndData->rawData.size() * 16);//This should be a good enough buffer for decoding

		if (ulNbChannels == 2) {
			SAdpcmStereoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = source.sndData->rawData.data() + (44 + 20 + 4);//44 + 20 is what i found made the ADPCM play with no glitches
			param.InputLength = source.sndData->rawData.size() - (44 + 20 + 4);
			param.OutputBuffer = decoded.data();
			int written = 0;
			bool ret = DecompressStereoAdpcm(&param, written);
			SDL_assert_release(ret);
			decoded.resize(written);
		}
		else if (ulNbChannels == 1) {
			SAdpcmMonoParam param;
			memset(&param, 0, sizeof(param));
			param.InputBuffer = source.sndData->rawData.data() + (44 + 20 + 4);//44 + 20 is what i found made the ADPCM play with no glitches
			param.InputLength = source.sndData->rawData.size() - (44 + 20 + 4);
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
		//Find the SourceObject
		sbaoFile source = DARE::instance().loadAtomicObject(stToolSourceFormat.dataRef.refAtomicId);
		SDL_assert_release(source.sndData);

		int channels, sampleRate;
		short *output;
		int ret = stb_vorbis_decode_memory(source.sndData->rawData.data(), source.sndData->rawData.size(), &channels, &sampleRate, &output);
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

	//dataRef Path
	fp.serialize(dataRef);
}

void SND_tdstToolSourceFormat::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(lTransferRate);
	REGISTER_MEMBER(ulNbBytes);
	REGISTER_MEMBER(ulNbSamples);
	REGISTER_MEMBER(isMTTInterlaced);
	REGISTER_MEMBER(bStream);
	REGISTER_MEMBER(dataRef);
}

void tdstWaveMarkerList::read(IBinaryArchive & fp) {
	stringPoolSize = 0;
	fp.serialize(stringPoolSize);
	SDL_assert_release(stringPoolSize == 0);//TODO

	fp.serializeNdVectorExternal(m_waveMarkers);
	SDL_assert_release(m_waveMarkers.size() == 0);//TODO
}

void tdstWaveMarkerList::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(stringPoolSize);
	REGISTER_MEMBER(m_waveMarkers);
}

void DynamicIndexedPropertyContainer::read(IBinaryArchive & fp) {
	int32_t size = 0;
	fp.serialize(size);
	SDL_assert_release(size == 0);//TODO

	fp.serialize(isBlob);
	if (isBlob) {
		uint32_t unk1;
		fp.serialize(unk1);
	} else {
		SDL_assert_release(false);//TODO
	}
}

void DynamicIndexedPropertyContainer::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(isBlob);
}

void tdstWaveMarkerElement::read(IBinaryArchive & fp) {
}

void tdstWaveMarkerElement::registerMembers(MemberStructure & ms) {
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

void EmitterSpec::registerMembers(MemberStructure & ms) {
}

void SilenceResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(fLength);
	fp.serialize(busId);
}

void SilenceResourceDescriptor::registerMembers(MemberStructure & ms) {
	REGISTER_MEMBER(fLength);
	REGISTER_MEMBER(busId);
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
}

void tdstEffectGraph::read(IBinaryArchive & fp) {
	fp.serialize(eEffectID);
	fp.serialize(multiLayerParameterId);
	fp.serializeNdVectorExternal(m_coordinates);
}

void tdstEffectGraph::registerMembers(MemberStructure & ms) {
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
	fp.serialize(cone_m_InnerAngle);
	fp.serialize(cone_m_OuterAngle);
	fp.serialize(cone_m_InnerVolumeDB);
	fp.serialize(cone_m_OuterVolumeDB);
	fp.serialize(cone_m_InnerLPF);
	fp.serialize(cone_m_OuterLPF);
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
