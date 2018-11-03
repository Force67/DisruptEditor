#include "DatFat.h"

#include <SDL.h>

DatFat::DatFat(const std::string &filename) {
	FILE *fat = fopen(filename.c_str(), "rb");
	SDL_assert_release(fat);

	uint32_t magic;
	fread(&magic, sizeof(magic), 1, fat);
	SDL_assert_release(magic == 1178686515);

	int32_t version;
	fread(&version, sizeof(version), 1, fat);
	SDL_assert_release(version == 8);

	uint32_t flags;
	fread(&flags, sizeof(flags), 1, fat);
	SDL_assert_release((flags & ~0xFFFFFF) == 0);

	uint32_t entries;
	fread(&entries, sizeof(entries), 1, fat);

	std::string datFile = filename;
	datFile[datFile.size() - 3] = 'd';
	dat = SDL_RWFromFile(datFile.c_str(), "rb");
	SDL_assert_release(dat);

	for (uint32_t i = 0; i < entries; ++i) {
		uint32_t a, b, c, d;
		fread(&a, sizeof(uint32_t), 1, fat);
		fread(&b, sizeof(uint32_t), 1, fat);
		fread(&c, sizeof(uint32_t), 1, fat);
		fread(&d, sizeof(uint32_t), 1, fat);

		FileEntry fe;
		fe.realSize = (b >> 3) & 0x1FFFFFFFu;
		fe.compression = (FileEntry::Compression) ((b >> 0) & 0x00000007u);
		fe.offset = (long) d << 3;
		fe.offset |= (c >> 29) & 0x00000007u;
		fe.size = (c >> 0) & 0x1FFFFFFFu;
		files[a] = fe;
	}

	fclose(fat);
}

struct CompressionSettings {
	uint32_t Flags;
	uint32_t WindowSize;
	uint32_t ChunkSize;
};

Vector<uint8_t> DatFat::openRead(uint32_t hash) {
	auto it = files.find(hash);
	Vector<uint8_t> data;
	if (it != files.end()) {
		SDL_RWseek(dat, it->second.offset, RW_SEEK_SET);

		if (it->second.compression == FileEntry::Compression::None) {
			data.resize(it->second.size);
			SDL_RWread(dat, data.data(), 1, it->second.size);
			return data;
		} else if (it->second.compression == FileEntry::Compression::Xbox) {
			uint32_t magic = SDL_ReadBE32(dat);
			SDL_assert_release(magic == 0x0FF512EE);

			uint32_t version = SDL_ReadBE32(dat);
			SDL_assert_release(version == 0x01030000);

			uint32_t unknown08 = SDL_ReadBE32(dat);
			SDL_assert_release(unknown08 == 0);

			uint32_t unknown0C = SDL_ReadBE32(dat);
			SDL_assert_release(unknown0C == 0);

			uint32_t windowSize = SDL_ReadBE32(dat);

			uint32_t chunkSize = SDL_ReadBE32(dat);

			int64_t uncompressedSize = SDL_ReadBE64(dat);

			int64_t compressedSize = SDL_ReadBE64(dat);

			int32_t largestUncompressedChunkSize = SDL_ReadBE32(dat);

			int32_t largestCompressedChunkSize = SDL_ReadBE32(dat);

			if (uncompressedSize < 0 ||
				compressedSize < 0 ||
				largestUncompressedChunkSize < 0 ||
				largestCompressedChunkSize < 0) {
				SDL_assert_release(false);
			}

			SDL_assert_release(uncompressedSize == it->second.realSize);

			Vector<uint8_t> uncompressedBytes(largestUncompressedChunkSize);
			Vector<uint8_t> compressedBytes(largestCompressedChunkSize);

			int64_t remaining = uncompressedSize;
			while (remaining > 0) {
				/*CompressionSettings settings;
				settings.Flags = 0;
				settings.WindowSize = windowSize;
				settings.ChunkSize = chunkSize;

				void* handle;

				CDC CreateDecompressionContext = (CDC)(lib + 0x10197A1D);
				int ret = CreateDecompressionContext(1, settings, 1, handle);

				int32_t compressedChunkSize;
				fread(&compressedChunkSize, sizeof(compressedChunkSize), 1, ar);
				compressedChunkSize = SDL_Swap32(compressedChunkSize);
				if (compressedChunkSize < 0 ||
					compressedChunkSize > largestCompressedChunkSize) {
					SDL_assert_release(false);
				}

				fread(compressedBytes.data(), 1, compressedChunkSize, ar);

				int uncompressedChunkSize = (int) min(largestUncompressedChunkSize, remaining);
				int actualUncompressedChunkSize = uncompressedChunkSize;
				int32_t actualCompressedChunkSize = compressedChunkSize;

				typedef int(*XmemD)(void* context, void* output, int &outputSize, void* input, int &inputSize);
				XmemD Decompress = (XmemD)(lib + 0x101979A1);
				ret = Decompress(handle, uncompressedBytes.data(), actualUncompressedChunkSize, compressedBytes.data(), actualCompressedChunkSize);
				if (ret != 0) {
					SDL_assert_release(false);
				}

				if (actualUncompressedChunkSize != uncompressedChunkSize) {
					SDL_assert_release(false);
				}

				//output.Write(uncompressedBytes, 0, actualUncompressedChunkSize);

				remaining -= actualUncompressedChunkSize;*/
			}
		} else {
			SDL_assert_release(false);
		}

	}

	return data;
}
