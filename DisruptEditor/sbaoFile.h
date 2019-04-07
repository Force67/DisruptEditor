#pragma once

#include <stdint.h>
#include <SDL_rwops.h>
#include "Vector.h"
#include "CDobbsID.h"
#include <memory>
#include "IBinaryArchive.h"
#include "Serialization.h"

struct sbaoLayer {
	Vector<uint8_t> data;

	//Cached Data
	int channels, sampleRate, samples;
	void fillCache();
	void replace(const char* filename);
	void save(const char* filename);

	enum Type { VORBIS, PCM, ADPCM };
	Type type;
	int play(bool loop);
};

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
	CObjectReference<SndData> dataRef;

	void read(IBinaryArchive &fp);
	void registerMembers(MemberStructure &ms);
};

struct tdstWaveMarkerElement {
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
	int32_t isBlob;

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

struct EmitterSpec {
	void registerMembers(MemberStructure &ms);
};

struct BaseResourceDescriptor {
	CDobbsID type;

	//BaseFields
	Vector< CObjectReference<EmitterSpec> > emitterSpecs;

	std::shared_ptr<SampleResourceDescriptor> sampleResourceDescriptor;
	std::shared_ptr<RandomResourceDescriptor> randomResourceDescriptor;

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
	std::shared_ptr<SndData> sndData;
};

