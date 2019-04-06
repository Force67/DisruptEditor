#include "sbaoFile.h"

#include <algorithm>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <SDL.h>
#include "Vector.h"
#include "IBinaryArchive.h"

#include "Hash.h"
#include "Audio.h"
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

const uint32_t sbaoMagic = 207362;

#pragma pack(push, 1)
struct sbaoHeader {
	uint32_t magic = sbaoMagic;
	uint32_t unk1 = 0;
	uint32_t unk2 = 0;//first arg
	uint32_t unk3;
	uint32_t unk4;
	uint32_t unk5 = 1342177280;
	uint32_t unk6 = 2;
};
struct oggPageHeader {
	uint32_t magic = 0;
	uint8_t version = 0;
	uint8_t headerType = 0;
	uint64_t granulePos = 0;
	uint32_t serialNo = 0;
	uint32_t pageSeqNum = 0;
	uint32_t checksum = 0;
	uint8_t numSegments = 0;
};
#pragma pack(pop)

size_t getOggPageSize(uint8_t *ptr) {
	oggPageHeader *header = (oggPageHeader*)ptr;
	ptr += sizeof(*header);

	SDL_assert_release(header->magic == 1399285583);

	size_t packetSize = 0;
	for (uint8_t i = 0; i < header->numSegments; ++i) {
		packetSize += *ptr;
		++ptr;
	}

	return sizeof(*header) + header->numSegments + packetSize;
}

void writeZero(SDL_RWops *fp, size_t num) {
	for (size_t i = 0; i < num; ++i)
		SDL_WriteU8(fp, 0);
}

sbaoFile::sbaoFile() {
}

sbaoFile::~sbaoFile() {
}

void sbaoFile::open(IBinaryArchive & fp) {
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
	}

	fp.padding = IBinaryArchive::PADDING_IBINARYARCHIVE;
}

void sbaoLayer::fillCache() {
	if (type != VORBIS) return;
	int error;
	stb_vorbis *v = stb_vorbis_open_memory(data.data(), data.size(), &error, NULL);
	SDL_assert_release(v);

	stb_vorbis_info info = stb_vorbis_get_info(v);
	channels = info.channels;
	sampleRate = info.sample_rate;
	samples = stb_vorbis_stream_length_in_samples(v);

	stb_vorbis_close(v);
}

void sbaoLayer::replace(const char * filename) {
	if (!filename) return;

	SDL_RWops *fp = SDL_RWFromFile(filename, "rb");
	if (!fp) return;

	data.resize(SDL_RWsize(fp));
	SDL_RWread(fp, data.data(), 1, data.size());
	SDL_RWclose(fp);

	fillCache();
}

void sbaoLayer::save(const char * filename) {
	if (!filename) return;

	SDL_RWops *fp = SDL_RWFromFile(filename, "wb");
	if (!fp) return;

	SDL_RWwrite(fp, data.data(), 1, data.size());
	SDL_RWclose(fp);
}

int sbaoLayer::play(bool loop) {
	int channels, sampleRate;
	short *output;
	int ret = stb_vorbis_decode_memory(data.data(), data.size(), &channels, &sampleRate, &output);
	SDL_assert_release(ret > 0);
	int ref = Audio::instance().addSound(sampleRate, channels, output, ret * channels * sizeof(short), loop);
	free(output);
	return ref;
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

void ResourceVolume::read(IBinaryArchive & fp) {
	vol.read(fp);
	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 &) [with T1=Dare::TVolume<(long)0, Dare::NonPositiveVolumeTraits>]
	fp.serialize(delta_dB);
	fp.serialize(randomProbDist);
}

void RTPCVolume::read(IBinaryArchive & fp) {
	m_rtpc.read(fp);
	//SndGear::Serializer &SndGear::operator <<<T1>(SndGear::Serializer &, T1 &) [with T1=Dare::TVolume<(long)1, Dare::NonPositiveVolumeTraits>]
	fp.serialize(m_volume_dB);
}

void BaseResourceDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(type.id);
	std::string typeName = type.getReverseName();

	//Base Attributes
	fp.serializeNdVectorExternal(emitterSpecs);

	if (typeName == "SampleResourceDescriptor") {
		sampleResourceDescriptor = std::make_shared<SampleResourceDescriptor>();
		sampleResourceDescriptor->read(fp);
	}
	else {
		SDL_assert_release(false);
	}
}

void RTPC::read(IBinaryArchive & fp) {
	fp.serialize(rtpcID);
}

void LimiterInfoDescriptor::read(IBinaryArchive & fp) {
	fp.serialize(m_maxCount);
	fp.serialize(m_limitationRule);
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

void SND_tdstToolSourceFormat::read(IBinaryArchive & fp) {
	fp.serialize(lTransferRate);
	fp.serialize(ulNbBytes);
	fp.serialize(ulNbSamples);
	fp.serialize(isMTTInterlaced);
	fp.serialize(bStream);

	//dataRef Path
	fp.serialize(dataRef);
}

void tdstWaveMarkerList::read(IBinaryArchive & fp) {
	stringPoolSize = 0;
	fp.serialize(stringPoolSize);
	SDL_assert_release(stringPoolSize == 0);//TODO

	fp.serializeNdVectorExternal(m_waveMarkers);
	SDL_assert_release(m_waveMarkers.size() == 0);//TODO
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

void tdstWaveMarkerElement::read(IBinaryArchive & fp) {
}
