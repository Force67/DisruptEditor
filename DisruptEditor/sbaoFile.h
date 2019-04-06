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

};

template <typename T>
struct CObjectReference {
	uint32_t refAtomicId;
	void read(IBinaryArchive &fp) {
		fp.serialize(refAtomicId);
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
};

struct tdstWaveMarkerElement {
	void read(IBinaryArchive &fp);
};

struct tdstWaveMarkerList {
	uint32_t stringPoolSize;
	Vector<tdstWaveMarkerElement> m_waveMarkers;

	void read(IBinaryArchive &fp);
};

struct DynamicIndexedPropertyContainer {
	int32_t isBlob;

	void read(IBinaryArchive &fp);
};

struct SampleResourceDescriptor {
	bool bLooping;
	bool bTool;
	bool bIsNotifying;
	uint32_t ulLoopByte;
	uint32_t ulLoopSample;
	uint32_t ulBitRate;
	uint32_t ulResNotificationUserData;
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

	void read(IBinaryArchive &fp);
};

struct tdstRandomElement {
	uint32_t resourceId;
	float prob;
	bool bCanBeChosenTwice;
	bool bHasPlayed;

	void read(IBinaryArchive &fp);
};

struct RandomResourceDescriptor {
	bool bUseShuffle;

	Vector<tdstRandomElement> elements;

	void read(IBinaryArchive &fp);
};

struct EmitterSpec {

};

struct BaseResourceDescriptor {
	CDobbsID type;

	//BaseFields
	Vector< CObjectReference<EmitterSpec> > emitterSpecs;

	std::shared_ptr<SampleResourceDescriptor> sampleResourceDescriptor;
	std::shared_ptr<RandomResourceDescriptor> randomResourceDescriptor;

	void read(IBinaryArchive &fp);
};

struct RTPC {
	uint32_t rtpcID;

	void read(IBinaryArchive &fp);
};

struct RTPCVolume {
	RTPC m_rtpc;
	float m_volume_dB;
	void read(IBinaryArchive &fp);
};

struct ResourceVolume {
	RTPCVolume vol;
	float delta_dB;
	uint32_t randomProbDist;
	void read(IBinaryArchive &fp);
};

struct LimiterInfoDescriptor {
	uint32_t m_maxCount;
	int32_t m_limitationRule;

	void read(IBinaryArchive &fp);
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
};

class sbaoFile {
public:
	sbaoFile();
	~sbaoFile();
	void open(IBinaryArchive &fp);

	uint32_t unk1;
	uint32_t unk2;
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5;

	CDobbsID type;

	std::shared_ptr<ResourceDescriptor> resourceDescriptor;
};

