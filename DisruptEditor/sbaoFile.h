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
	uint32_t stringPoolSize;
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
	uint32_t m_size;
	uint32_t m_allocInfos;
	uint32_t m_allocatorType;
	uint32_t m_memoryType;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(m_size);
		REGISTER_MEMBER(m_allocInfos);
		REGISTER_MEMBER(m_allocatorType);
		REGISTER_MEMBER(m_memoryType);
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

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
		REGISTER_MEMBER(type);
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
	int32_t useCone;
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

struct EffectPresetInfo {
	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
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

struct ProjectBusDataDescriptor {
	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms) {
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
	std::shared_ptr<SndData> sndData;
};

