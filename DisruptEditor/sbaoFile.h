#pragma once

#include <stdint.h>
#include <SDL_rwops.h>
#include "Vector.h"
#include "CDobbsID.h"
#include <memory>
#include "IBinaryArchive.h"
#include "Serialization.h"

struct ResourceDescriptor;

struct SndData {
	Vector<uint8_t> rawData;
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
	void registerMembers(MemberStructure &ms);
};

struct tdstCoordinate {
	float xFloat;
	float yFloat;
	uint32_t curvType;
	float curveFactor;

	void read(IBinaryArchive &fp);
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
};

struct EmitterSpec {
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

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
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

struct ResourceVolume {
	RTPCVolume vol;
	float delta_dB;
	uint32_t randomProbDist;

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
	float cone_m_InnerAngle;
	float cone_m_OuterAngle;
	float cone_m_InnerVolumeDB;
	float cone_m_OuterVolumeDB;
	float cone_m_InnerLPF;
	float cone_m_OuterLPF;

	void read(IBinaryArchive &fp);
};

struct MicPresetDescriptor {
	uint32_t mask;
	MicSpecDescriptor spec;

	void read(IBinaryArchive &fp);
};

struct EffectPresetInfo {
	void read(IBinaryArchive &fp);
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
};

struct BusPresetInfo {
	uint32_t busId;
	Vector<ParamInfo> paramsToChange;

	void read(IBinaryArchive &fp);
};

struct PresetDescriptor {
	uint32_t id;
	int32_t type;
	Vector<MicPresetDescriptor> micPresetList;
	Vector<EffectPresetInfo> effectPresetInfos;
	Vector<BusPresetInfo> busPresetInfos;
	int32_t priority;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct PresetEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<PresetDescriptor> presetRef;

	void read(IBinaryArchive &fp);
};

struct StopEventDescriptor {
	EventDescriptor pBase;
	CObjectReference<EventDescriptor> uEvt;
	float fFadeDuration;
	uint32_t eFadeType;

	void read(IBinaryArchive &fp);
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
	std::shared_ptr<SndData> sndData;
};

