#pragma once

#include <stdint.h>
#include <SDL_rwops.h>
#include "Vector.h"
#include "CDobbsID.h"
#include <memory>
#include <array>
#include "IBinaryArchive.h"
#include "Serialization.h"

struct ResourceDescriptor;

struct SndData {
	Vector<uint8_t> rawData;
	void registerMembers(MemberStructure &ms) {
	}
};

struct VolumeHelper {
	float volume;
	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, volume);
	}
};

template <typename T>
struct CObjectReference {
	uint32_t refAtomicId;
	void read(IBinaryArchive &fp) {
		fp.serialize(refAtomicId);
	}
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, refAtomicId);
	}
};

struct RTPC {
	uint32_t rtpcID;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct RTPCVolume {
	RTPC m_rtpc;
	float m_volume_dB;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct RTPCVolumeMatrices {
	std::array<RTPCVolume, 6> volume;
	std::array<RTPCVolume, 0xC> volume2;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(volume);
		REGISTER_MEMBER(volume2);
	}
};

struct SND_Cone {
	float m_InnerAngle;
	float m_OuterAngle;
	float m_InnerVolumeDB;
	float m_OuterVolumeDB;
	float m_InnerLPF;
	float m_OuterLPF;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_InnerAngle);
		REGISTER_MEMBER(m_OuterAngle);
		REGISTER_MEMBER(m_InnerVolumeDB);
		REGISTER_MEMBER(m_OuterVolumeDB);
		REGISTER_MEMBER(m_InnerLPF);
		REGISTER_MEMBER(m_OuterLPF);
	}
};

struct RTPCCone {
	SND_Cone cone;
	RTPC m_innerAngleRTPC;
	RTPC m_outerAngleRTPC;
	RTPC m_innerVolumeRTPC;
	RTPC m_outerVolumeRTPC;
	RTPC m_innerLPFRTPC;
	RTPC m_outerLPFRTPC;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(cone);
		REGISTER_MEMBER(m_innerAngleRTPC);
		REGISTER_MEMBER(m_outerAngleRTPC);
		REGISTER_MEMBER(m_innerVolumeRTPC);
		REGISTER_MEMBER(m_outerVolumeRTPC);
		REGISTER_MEMBER(m_innerLPFRTPC);
		REGISTER_MEMBER(m_outerLPFRTPC);
	}
};

struct ResourceVolume {
	RTPCVolume vol;
	float delta_dB;
	uint32_t randomProbDist;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct SND_tdstToolSourceFormat {
	int32_t lTransferRate;
	uint32_t ulNbBytes;
	uint32_t ulNbSamples;
	uint32_t isMTTInterlaced;
	bool bStream;

	//bStream Info
	bool bZeroLatency;
	uint32_t ZLMemPartInBytes;
	uint32_t ulOffsetData;
	uint32_t dataId;
	CObjectReference<SndData> streamRef;
	CObjectReference<SndData> uSndDataZeroLatencyMemPart;

	//!bStream Info
	CObjectReference<SndData> dataRef;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct tdstWaveMarkerElement {
	float fTimePos;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct tdstWaveMarkerList {
	Vector<int8_t> stringPool;
	Vector<tdstWaveMarkerElement> m_waveMarkers;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct DynamicIndexedPropertyContainer {
	int32_t size;
	bool isBlob;

	//Normal Seralize
	uint32_t rawSize;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct SampleResourceDescriptor {
	bool bLooping;
	bool bTool;
	bool bIsNotifying;
	uint32_t ulLoopByte;
	uint32_t ulLoopSample;
	uint32_t ulBitRate;
	uint32_t ulResNotificationUserData;
	//1 - PCM
	//2 - ADPCM
	//4 - OGG
	uint32_t CompressionFormat;
	uint32_t ulNbChannels;
	uint32_t ulFreq;
	SND_tdstToolSourceFormat stToolSourceFormat;
	tdstWaveMarkerList stWaveMarkerList;
	uint32_t autoDuckingSetPresetEventId;
	uint32_t busId;
	DynamicIndexedPropertyContainer platformSpecificProperties;
	uint32_t ulAttackLengthSample;
	uint32_t ulAttackLengthByte;
	uint32_t ulLoopLengthSample;
	uint32_t ulLoopLengthByte;

	std::vector<short> decode();
	void play();
	void saveDecoded(const char* file);
	uint32_t getHelpfulId();

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct tdstRandomElement {
	uint32_t resourceId;
	float prob;
	bool bCanBeChosenTwice;
	bool bHasPlayed;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct RandomResourceDescriptor {
	bool bUseShuffle;

	Vector<tdstRandomElement> elements;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct SilenceResourceDescriptor {
	float fLength;
	uint32_t busId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(fLength);
		REGISTER_MEMBER(busId);
	}
};

struct tdstCoordinate {
	float xFloat;
	float yFloat;
	uint32_t curvType;
	float curveFactor;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(xFloat);
		REGISTER_MEMBER(yFloat);
		REGISTER_MEMBER(curvType);
		REGISTER_MEMBER(curveFactor);
	}
};

struct tdstEffectGraph {
	uint32_t eEffectID;
	uint32_t multiLayerParameterId;
	Vector<tdstCoordinate> m_coordinates;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct tdstMultiLayerElement {
	CObjectReference<ResourceDescriptor> uRes;
	uint32_t activationFlagId;
	int32_t invFlag;
	Vector<tdstEffectGraph> m_effectGraphs;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct MultiLayerResourceDescriptor {
	Vector<tdstMultiLayerElement> m_layers;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct SequenceResourceDescriptor {
	bool bLoop;
	uint32_t ulStartLoop;
	uint32_t ulEndLoop;
	uint32_t ulNbLoops;
	float fLength;
	float fPosMainReLoop;
	Vector< CObjectReference<ResourceDescriptor> > sequences;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(bLoop);
		REGISTER_MEMBER(ulStartLoop);
		REGISTER_MEMBER(ulEndLoop);
		REGISTER_MEMBER(ulNbLoops);
		REGISTER_MEMBER(fLength);
		REGISTER_MEMBER(fPosMainReLoop);
		REGISTER_MEMBER(sequences);
	}
};

struct tdstMultiTrackElement {
	uint32_t ulFreq;
	uint32_t ulNbChannels;
	uint32_t CompressionFormat;
	ResourceVolume trackVolume;
	uint32_t ulNbSamples;
	uint32_t ulNbBytes;
	uint32_t ulBitRate;
	DynamicIndexedPropertyContainer platformSpecificProperties;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(ulFreq);
		REGISTER_MEMBER(ulNbChannels);
		REGISTER_MEMBER(CompressionFormat);
		REGISTER_MEMBER(trackVolume);
		REGISTER_MEMBER(ulNbSamples);
		REGISTER_MEMBER(ulNbBytes);
		REGISTER_MEMBER(ulBitRate);
		REGISTER_MEMBER(platformSpecificProperties);
	}
};

struct DataBlock {
	uint32_t m_allocInfos;
	uint32_t m_allocatorType;
	uint32_t m_memoryType;
	Vector<uint8_t> data;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_allocInfos);
		REGISTER_MEMBER(m_allocatorType);
		REGISTER_MEMBER(m_memoryType);
		REGISTER_MEMBER(data);
	}
};

struct MultiTrackResourceDescriptor {
	bool bLoop;
	bool bIsNotifying;
	uint32_t ulNbTrack;
	uint32_t ulResNotificationUserData;
	uint32_t multiTrackChannelId;
	SND_tdstToolSourceFormat stToolSourceFormat;
	tdstWaveMarkerList stWaveMarkerList;
	uint32_t autoDuckingSetPresetEventId;
	uint32_t busId;
	Vector<tdstMultiTrackElement> m_tracks;
	DataBlock m_tracksRawData;
	DynamicIndexedPropertyContainer platformSpecificProperties;
	uint32_t currentResourceID;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(bLoop);
		REGISTER_MEMBER(bIsNotifying);
		REGISTER_MEMBER(ulNbTrack);
		REGISTER_MEMBER(ulResNotificationUserData);
		REGISTER_MEMBER(multiTrackChannelId);
		REGISTER_MEMBER(stToolSourceFormat);
		REGISTER_MEMBER(stWaveMarkerList);
		REGISTER_MEMBER(autoDuckingSetPresetEventId);
		REGISTER_MEMBER(busId);
		REGISTER_MEMBER(m_tracks);
		REGISTER_MEMBER(m_tracksRawData);
		REGISTER_MEMBER(platformSpecificProperties);
		REGISTER_MEMBER(currentResourceID);
	}
};

struct ThemePartOutroDescriptor {
	CDobbsID type;

	//ThemePartOutroDescriptor
	CObjectReference<ResourceDescriptor> resRef;
	float fPos;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(type);
		REGISTER_MEMBER(resRef);
		REGISTER_MEMBER(fPos);
	}
};

struct ThemePartDescriptor {
	CObjectReference<ResourceDescriptor> resRef;
	bool bLoopStart;
	bool bLoopEnd;
	int32_t lNbLoops;
	float fLength;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(resRef);
		REGISTER_MEMBER(bLoopStart);
		REGISTER_MEMBER(bLoopEnd);
		REGISTER_MEMBER(lNbLoops);
		REGISTER_MEMBER(fLength);
	}
};

struct ThemeResourceDescriptor {
	uint32_t ulStartLoop;
	uint32_t ulNbLoopsulNbLoops;
	bool bStartImmediatly;
	bool bIsNotifying;
	bool bStream;
	uint32_t ulResNotificationUserData;
	uint32_t eTransition;
	uint32_t eIndexFadeInType;
	uint32_t eIndexFadeOutType;
	ThemePartOutroDescriptor pstPartOutro;
	float fThemeLength;
	float fPosMainReLoop;
	uint32_t ulTracksFading;
	float indexFadeInDur;
	float indexFadeOutDur;
	Vector<ThemePartDescriptor> m_themParts;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(ulStartLoop);
		REGISTER_MEMBER(ulNbLoopsulNbLoops);
		REGISTER_MEMBER(bStartImmediatly);
		REGISTER_MEMBER(bIsNotifying);
		REGISTER_MEMBER(bStream);
		REGISTER_MEMBER(ulResNotificationUserData);
		REGISTER_MEMBER(eTransition);
		REGISTER_MEMBER(eIndexFadeInType);
		REGISTER_MEMBER(eIndexFadeOutType);
		REGISTER_MEMBER(pstPartOutro);
		REGISTER_MEMBER(fThemeLength);
		REGISTER_MEMBER(fPosMainReLoop);
		REGISTER_MEMBER(ulTracksFading);
		REGISTER_MEMBER(indexFadeInDur);
		REGISTER_MEMBER(indexFadeOutDur);
		REGISTER_MEMBER(m_themParts);
	}
};

struct EmitterSpec {
	uint32_t m_id;
	RTPCVolume m_volume;
	RTPCVolumeMatrices m_volumesMatrices;
	uint32_t m_positioned;
	uint32_t m_dopplerEffect;
	uint32_t m_speakerPanning;
	uint32_t m_playOnWiimote;
	uint32_t m_useEmitterCone;
	uint32_t m_rolloffId;
	RTPC m_pSpreadFactor;
	RTPC m_pWetVolume;
	RTPC m_pLPFCutoffFrequency;
	RTPCCone m_emitterAudibilityCone;
	Vector<uint32_t> m_effectIds;
	uint16_t m_activeSpeakers;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct GranularPitchInfo {
	float m_freq;
	uint32_t m_samplePos;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_freq);
		REGISTER_MEMBER(m_samplePos);
	}
};

struct GranularResourceDescriptor {
	uint32_t m_idleStop;
	uint32_t m_accStart;
	uint32_t m_accStop;
	uint32_t m_maxStart;
	uint32_t m_maxStop;
	uint32_t m_decStart;
	uint32_t m_numChannels;
	uint32_t m_freq;
	uint32_t m_busId;
	uint32_t m_autoDuckingSetPresetEventId;
	bool m_isDecelerationEnabled;
	uint32_t m_granuleMaxSize;
	uint32_t m_granuleMinSize;
	uint32_t m_constantGranuleMaxSize;
	uint32_t m_constantGranuleMinSize;
	uint32_t m_maxGranuleShuffle;
	float m_smoothfactor;
	float m_granuleOverlap;
	uint32_t m_compression;
	SND_tdstToolSourceFormat m_toolSourceFormat;
	uint32_t m_granuleAligmentRes;
	Vector<GranularPitchInfo> m_pitchInfo;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_idleStop);
		REGISTER_MEMBER(m_accStart);
		REGISTER_MEMBER(m_accStop);
		REGISTER_MEMBER(m_maxStart);
		REGISTER_MEMBER(m_maxStop);
		REGISTER_MEMBER(m_decStart);
		REGISTER_MEMBER(m_numChannels);
		REGISTER_MEMBER(m_freq);
		REGISTER_MEMBER(m_busId);
		REGISTER_MEMBER(m_autoDuckingSetPresetEventId);
		REGISTER_MEMBER(m_isDecelerationEnabled);
		REGISTER_MEMBER(m_granuleMaxSize);
		REGISTER_MEMBER(m_granuleMinSize);
		REGISTER_MEMBER(m_constantGranuleMaxSize);
		REGISTER_MEMBER(m_constantGranuleMinSize);
		REGISTER_MEMBER(m_maxGranuleShuffle);
		REGISTER_MEMBER(m_smoothfactor);
		REGISTER_MEMBER(m_granuleOverlap);
		REGISTER_MEMBER(m_compression);
		REGISTER_MEMBER(m_toolSourceFormat);
		REGISTER_MEMBER(m_granuleAligmentRes);
		REGISTER_MEMBER(m_pitchInfo);
	}
};

struct tdstSwitchElement {
	uint32_t resourceId;
	uint32_t switchValueId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(resourceId);
		REGISTER_MEMBER(switchValueId);
	}
};

struct SwitchResourceDescriptor {
	Vector<tdstSwitchElement> m_elements;
	uint32_t switchTypeId;
	uint32_t defaultSwitchValueId;
	bool bDynamic;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_elements);
		REGISTER_MEMBER(switchTypeId);
		REGISTER_MEMBER(defaultSwitchValueId);
		REGISTER_MEMBER(bDynamic);
	}
};

struct BaseResourceDescriptor {
	CDobbsID type;

	//BaseFields
	Vector< CObjectReference<EmitterSpec> > emitterSpecs;

	std::shared_ptr<SampleResourceDescriptor> sampleResourceDescriptor;
	std::shared_ptr<RandomResourceDescriptor> randomResourceDescriptor;
	std::shared_ptr<SilenceResourceDescriptor> silenceResourceDescriptor;
	std::shared_ptr<MultiLayerResourceDescriptor> multiLayerResourceDescriptor;
	std::shared_ptr<SequenceResourceDescriptor> sequenceResourceDescriptor;
	std::shared_ptr<MultiTrackResourceDescriptor> multiTrackResourceDescriptor;
	std::shared_ptr<ThemeResourceDescriptor> themeResourceDescriptor;
	std::shared_ptr<GranularResourceDescriptor> granularResourceDescriptor;
	std::shared_ptr<SwitchResourceDescriptor> switchResourceDescriptor;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct LimiterInfoDescriptor {
	uint32_t m_maxCount;
	int32_t m_limitationRule;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct ResourceDescriptor {
	uint32_t Id;
	ResourceVolume resVolume;
	bool bLocalised;
	LimiterInfoDescriptor globalLimiterInfo;
	LimiterInfoDescriptor perSoundObjectLimiterInfo;
	uint32_t eType;
	BaseResourceDescriptor pResourceDesc;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct Delay {
	float delayTime;
	float delayTimeDelta;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct EventDescriptor {
	uint32_t Id;
	uint32_t eType;
	LimiterInfoDescriptor globalLimiterInfo;
	LimiterInfoDescriptor perSoundObjectLimiterInfo;
	bool bDynamic;
	bool bLinkable;
	bool bSynthable;
	uint32_t applyVirtBehaviorWhenInaudible;
	uint32_t virtualizationLogic;
	int32_t lPrio;
	Delay delay;
	uint32_t rtpcId;
	int32_t isLoopingChildResource;
	uint32_t tempoBPM;
	uint32_t tempoTimeSignature;
	uint32_t playOnCue;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct PlayEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<ResourceDescriptor> resourceRef;
	float fDeTuneDelta;
	float fValPitchStat;
	float fFadeDuration;
	uint32_t eFadeType;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct MultiEventDescriptor {
	EventDescriptor pBase;
	Vector< CObjectReference<EventDescriptor> > m_events;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct MicSpecDescriptor {
	uint32_t micAtomicId;
	float volumeInDecibels;
	float fadeDuration;
	int32_t fadeType;
	uint32_t rolloffId;
	bool useCone;
	SND_Cone cone;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(micAtomicId);
		REGISTER_MEMBER(volumeInDecibels);
		REGISTER_MEMBER(fadeDuration);
		REGISTER_MEMBER(fadeType);
		REGISTER_MEMBER(rolloffId);
		REGISTER_MEMBER(useCone);
		REGISTER_MEMBER(cone);
	}
};

struct MicPresetDescriptor {
	uint32_t mask;
	MicSpecDescriptor spec;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(mask);
		REGISTER_MEMBER(spec);
	}
};

struct ParameterValue {
	//0 -None? Calls Seralizer::End()
	//1 -S32
	//2 -Float
	//3 - List of S32
	//4 - List of Float
	int32_t parameterType;
	uint32_t parameterIndex;
	uint32_t rtpcId;

	int32_t valueSndS32;
	float valueSndFloat;
	Vector<int32_t> valueListSndS32;
	Vector<float> valueListSndFloat;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(parameterType);
		REGISTER_MEMBER(parameterIndex);
		REGISTER_MEMBER(rtpcId);

		//TODO
		REGISTER_MEMBER(valueSndS32);
		REGISTER_MEMBER(valueSndFloat);
		REGISTER_MEMBER(valueListSndS32);
		REGISTER_MEMBER(valueListSndFloat);
	}
};

struct ParamInfo {
	int32_t fadeInType;
	float fadeInDuration;
	float duration;
	int32_t fadeOutType;
	float fadeOutDuration;
	int32_t absoluteChange;
	ParameterValue paramValue;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(fadeInType);
		REGISTER_MEMBER(fadeInDuration);
		REGISTER_MEMBER(duration);
		REGISTER_MEMBER(fadeOutType);
		REGISTER_MEMBER(fadeOutDuration);
		REGISTER_MEMBER(absoluteChange);
		REGISTER_MEMBER(paramValue);
	}
};

struct EffectPresetInfo {
	uint32_t effectId;
	Vector<ParamInfo> paramsToChange;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(effectId);
		REGISTER_MEMBER(paramsToChange);
	}
};

struct BusPresetInfo {
	uint32_t busId;
	Vector<ParamInfo> paramsToChange;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(busId);
		REGISTER_MEMBER(paramsToChange);
	}
};

struct PresetDescriptor {
	uint32_t id;
	int32_t type;
	Vector<MicPresetDescriptor> micPresetList;
	Vector<EffectPresetInfo> effectPresetInfos;
	Vector<BusPresetInfo> busPresetInfos;
	int32_t priority;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(id);
		REGISTER_MEMBER(type);
		REGISTER_MEMBER(micPresetList);
		REGISTER_MEMBER(effectPresetInfos);
		REGISTER_MEMBER(busPresetInfos);
		REGISTER_MEMBER(priority);
	}
};

struct PresetEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<PresetDescriptor> presetRef;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, pBase);
		REGISTER_MEMBER(presetRef);
	}
};

struct StopEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<EventDescriptor> uEvt;
	float fFadeDuration;
	uint32_t eFadeType;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, pBase);
		REGISTER_MEMBER(uEvt);
		REGISTER_MEMBER(fFadeDuration);
		REGISTER_MEMBER(eFadeType);
	}
};

struct RemovePresetEventDescriptor {
	EventDescriptor pBase;
	uint32_t presetId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, pBase);
		REGISTER_MEMBER(presetId);
	}
};

struct tdstObstructionPreset {
	bool bUseSoftwareFilterObstruction;
	float fLowPassMinFreq;
	float fLowPassMaxFreq;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(bUseSoftwareFilterObstruction);
		REGISTER_MEMBER(fLowPassMinFreq);
		REGISTER_MEMBER(fLowPassMaxFreq);
	}
};

struct tdstOcclusionPortable {
	float fBandPassCenterFrequency;
	float fBandPassBandWidth;
	float fGain;
	uint32_t ulActiveFilters;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(fBandPassCenterFrequency);
		REGISTER_MEMBER(fBandPassBandWidth);
		REGISTER_MEMBER(fGain);
		REGISTER_MEMBER(ulActiveFilters);
	}
};

struct tdstOcclusionPreset {
	uint32_t materialId;
	bool bUseSoftwareFilterOcclusion;
	tdstOcclusionPortable Portable;
	float fSoftwareOcclusion;
	DynamicIndexedPropertyContainer platformSpecificProps;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(materialId);
		REGISTER_MEMBER(bUseSoftwareFilterOcclusion);
		REGISTER_MEMBER(Portable);
		REGISTER_MEMBER(fSoftwareOcclusion);
		REGISTER_MEMBER(platformSpecificProps);
	}
};

struct tdstSoundTexture {
	uint32_t textureId;
	uint32_t resourceId;
	float fReadRate;
	float fContactDuration;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(textureId);
		REGISTER_MEMBER(resourceId);
		REGISTER_MEMBER(fReadRate);
		REGISTER_MEMBER(fContactDuration);
	}
};

struct tdstMultiLayerParameter {
	uint32_t Id;
	float fMin;
	float fMax;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(Id);
		REGISTER_MEMBER(fMin);
		REGISTER_MEMBER(fMax);
	}
};

struct BusVolumeTraits {
	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
	}
};

template <int Size>
struct ChannelVolumes {
	VolumeHelper m_masterVolume;
	std::array<VolumeHelper, Size> m_channelVolumes;

	void read(IBinaryArchive &fp) {
		fp.serialize(m_masterVolume);
		for (int i = 0; i < Size; ++i)
			fp.serialize(m_channelVolumes[i]);
	}
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_masterVolume);
		REGISTER_MEMBER(m_channelVolumes);
	}
};

struct BusChannelVolumes {
	VolumeHelper m_masterVolume;
	ChannelVolumes<6> m_dryChannelsVolumes;
	ChannelVolumes<1> m_wetChannelsVolumes;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_masterVolume);
		REGISTER_MEMBER(m_dryChannelsVolumes);
		REGISTER_MEMBER(m_wetChannelsVolumes);
	}
};

struct BusDescriptor {
	uint32_t id;
	int32_t busType;
	uint32_t parentBusId;
	Vector<uint32_t> effectIds;
	VolumeHelper preEffectVolume;
	VolumeHelper connectedVoicesVolume;
	BusChannelVolumes postEffectChannelVolumes;
	float pitchSemiTone;
	uint32_t processingStage;
	uint32_t childBusesCount;
	int32_t onReverbPath;
	LimiterInfoDescriptor voiceLimiterInfo;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(id);
		REGISTER_MEMBER(busType);
		REGISTER_MEMBER(parentBusId);
		REGISTER_MEMBER(effectIds);
		REGISTER_MEMBER(preEffectVolume);
		REGISTER_MEMBER(connectedVoicesVolume);
		REGISTER_MEMBER(postEffectChannelVolumes);
		REGISTER_MEMBER(pitchSemiTone);
		REGISTER_MEMBER(processingStage);
		REGISTER_MEMBER(childBusesCount);
		REGISTER_MEMBER(onReverbPath);
		REGISTER_MEMBER(voiceLimiterInfo);
	}
};

struct BusTreeDescriptor {
	uint32_t id;
	Vector<uint32_t> busIdList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(id);
		REGISTER_MEMBER(busIdList);
	}
};

struct ProjectBusDataDescriptor {
	Vector<BusDescriptor> busDescList;
	Vector<BusTreeDescriptor> busTreeDescList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(busDescList);
		REGISTER_MEMBER(busTreeDescList);
	}
};

struct EffectParameters {
	Vector<ParameterValue> parameterValueList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, parameterValueList);
	}
};

struct EffectDescriptor {
	uint32_t id;
	int32_t effectType;
	EffectParameters effectParameters;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(id);
		REGISTER_MEMBER(effectType);
		REGISTER_MEMBER(effectParameters);
	}
};

struct ProjectEffectDataDescriptor {
	Vector<EffectDescriptor> effectDescList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		ms.registerMember(NULL, effectDescList);
	}
};

struct RTVariableDescriptor {
	uint32_t m_varId;
	int32_t m_defaultValue;
	bool m_isDistanceBased;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_varId);
		REGISTER_MEMBER(m_defaultValue);
		REGISTER_MEMBER(m_isDistanceBased);
	}
};

struct GraphCoordinate {
	int32_t x;
	int32_t y;
	int32_t interpolationCurveType;
	float interpolationCurveFactor;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(x);
		REGISTER_MEMBER(y);
		REGISTER_MEMBER(interpolationCurveType);
		REGISTER_MEMBER(interpolationCurveFactor);
	}
};

struct RTGraphDescriptor {
	int32_t m_variableType;
	int32_t m_parameterType;
	Vector<GraphCoordinate> m_pointList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_variableType);
		REGISTER_MEMBER(m_parameterType);
		REGISTER_MEMBER(m_pointList);
	}
};

struct RTParameterDescriptor {
	int32_t m_type;
	uint32_t m_targetId;
	uint32_t m_fieldIndex;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_type);
		REGISTER_MEMBER(m_targetId);
		REGISTER_MEMBER(m_fieldIndex);
	}
};

struct RTPCDescriptor {
	Vector<uint32_t> m_varIdList;
	Vector<RTGraphDescriptor> m_graphList;
	Vector<RTParameterDescriptor> m_parameterList;
	uint32_t m_rtpcId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_varIdList);
		REGISTER_MEMBER(m_graphList);
		REGISTER_MEMBER(m_parameterList);
		REGISTER_MEMBER(m_rtpcId);
	}
};

struct RTPCsAndVariablesDescriptor {
	Vector<RTVariableDescriptor> m_variableList;
	Vector<RTPCDescriptor> m_rtpcList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_variableList);
		REGISTER_MEMBER(m_rtpcList);
	}
};

struct ProjectMicDataDescriptor {
	Vector<MicSpecDescriptor> micSpecList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(micSpecList);
	}
};

struct ProjectDesc {
	uint32_t serializerVersion;
	int32_t lProjectVersion;
	bool bLocalised;
	float minStreamingPrefetchBufferLength;
	tdstObstructionPreset stObstructionPreset;
	Vector<tdstOcclusionPreset> stOcclusionPresetList;
	Vector<uint32_t> stMTTChannelList;
	Vector<tdstSoundTexture> stSoundTextureList;
	Vector<tdstMultiLayerParameter> MultiLayerParameters;
	std::array<uint8_t, 16> cTitleGuid;
	std::array<uint8_t, 16> cProjectDataVersion;
	ProjectBusDataDescriptor projectBusDataDescBin;
	ProjectEffectDataDescriptor projectEffectDataDescBin;
	RTPCsAndVariablesDescriptor projectRTPCDataDescBin;
	ProjectMicDataDescriptor projectMicDataDescBin;
	uint32_t dopplerMicAtomicId;
	LimiterInfoDescriptor streamLimiterInfo;
	LimiterInfoDescriptor polyphonyLimiterInfo;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(serializerVersion);
		REGISTER_MEMBER(lProjectVersion);
		REGISTER_MEMBER(bLocalised);
		REGISTER_MEMBER(minStreamingPrefetchBufferLength);
		REGISTER_MEMBER(stObstructionPreset);
		REGISTER_MEMBER(stOcclusionPresetList);
		REGISTER_MEMBER(stMTTChannelList);
		REGISTER_MEMBER(stSoundTextureList);
		REGISTER_MEMBER(MultiLayerParameters);
		REGISTER_MEMBER(cTitleGuid);
		REGISTER_MEMBER(cProjectDataVersion);
		REGISTER_MEMBER(projectBusDataDescBin);
		REGISTER_MEMBER(projectEffectDataDescBin);
		REGISTER_MEMBER(projectRTPCDataDescBin);
		REGISTER_MEMBER(projectMicDataDescBin);
		REGISTER_MEMBER(dopplerMicAtomicId);
		REGISTER_MEMBER(streamLimiterInfo);
		REGISTER_MEMBER(polyphonyLimiterInfo);
	}
};

struct tdstRollOffPoint {
	float m_distance;
	float m_decibel;
	int32_t m_interpolationCurveType;
	float m_interpolationCurveFactor;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_distance);
		REGISTER_MEMBER(m_decibel);
		REGISTER_MEMBER(m_interpolationCurveType);
		REGISTER_MEMBER(m_interpolationCurveFactor);
	}
};

struct RolloffResourceDescriptor {
	Vector<tdstRollOffPoint> m_pointList;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_pointList);
	}
};

struct ChangeVolumeEventDescriptor {
	EventDescriptor pBase;
	bool bApplyOnObjectType;
	float newVolume_dB;
	float fFadeDuration;
	uint32_t eFadeType;
	int32_t lTrackID;
	uint32_t multiTrackChannelId;
	uint32_t emitterSpecId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(pBase);
		REGISTER_MEMBER(bApplyOnObjectType);
		REGISTER_MEMBER(newVolume_dB);
		REGISTER_MEMBER(fFadeDuration);
		REGISTER_MEMBER(eFadeType);
		REGISTER_MEMBER(lTrackID);
		REGISTER_MEMBER(multiTrackChannelId);
		REGISTER_MEMBER(emitterSpecId);
	}
};

struct StopNGoEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<EventDescriptor> uEvtStop;
	CObjectReference<EventDescriptor> uEvtGo;
	float fFadeDuration;
	uint32_t eFadeType;
	bool bCrossFade;
	bool bStopAllEvents;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(pBase);
		REGISTER_MEMBER(uEvtStop);
		REGISTER_MEMBER(uEvtGo);
		REGISTER_MEMBER(fFadeDuration);
		REGISTER_MEMBER(eFadeType);
		REGISTER_MEMBER(bCrossFade);
		REGISTER_MEMBER(bStopAllEvents);
	}
};

struct SwitchEventDescriptorElement {
	CObjectReference<EventDescriptor> eventRef;
	uint32_t switchValueId;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(eventRef);
		REGISTER_MEMBER(switchValueId);
	}
};

struct SwitchEventDescriptor {
	EventDescriptor pBase;
	uint32_t switchTypeId;
	CObjectReference<EventDescriptor> defaultEvent;
	Vector<SwitchEventDescriptorElement> m_elements;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(pBase);
		REGISTER_MEMBER(switchTypeId);
		REGISTER_MEMBER(defaultEvent);
		REGISTER_MEMBER(m_elements);
	}
};

class sbaoFile {
public:
	sbaoFile();
	~sbaoFile();
	void open(IBinaryArchive &fp, size_t size);
	void registerMembers(MemberStructure &ms);

	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;

	CDobbsID type;

	std::shared_ptr<ResourceDescriptor> resourceDescriptor;
	std::shared_ptr<PlayEventDescriptor> playEventDescriptor;
	std::shared_ptr<MultiEventDescriptor> multiEventDescriptor;
	std::shared_ptr<PresetDescriptor> presetDescriptor;
	std::shared_ptr<PresetEventDescriptor> presetEventDescriptor;
	std::shared_ptr<StopEventDescriptor> stopEventDescriptor;
	std::shared_ptr<RemovePresetEventDescriptor> removePresetEventDescriptor;
	std::shared_ptr<ProjectDesc> projectDesc;
	std::shared_ptr<RolloffResourceDescriptor> rolloffResourceDescriptor;
	std::shared_ptr<EmitterSpec> emitterSpec;
	std::shared_ptr<ChangeVolumeEventDescriptor> changeVolumeEventDescriptor;
	std::shared_ptr<StopNGoEventDescriptor> stopNGoEventDescriptor;
	std::shared_ptr<SwitchEventDescriptor> switchEventDescriptor;
	std::shared_ptr<SndData> sndData;
};

