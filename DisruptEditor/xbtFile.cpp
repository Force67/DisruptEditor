#include "xbtFile.h"

#include "Vector.h"
#include "FileHandler.h"
#include <SDL_rwops.h>
#include "glad.h"

//DDS Loader from: https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

bool xbtFile::open(SDL_RWops *fp) {
	//Seek past xbt header
	SDL_RWseek(fp, 8, RW_SEEK_CUR);
	int32_t ddsOffset = SDL_ReadLE32(fp);
	SDL_RWseek(fp, ddsOffset, RW_SEEK_SET);

	// lay out variables to be used
	unsigned int blockSize;
	unsigned int format;

	// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
	// read in 128 bytes from the file
	unsigned char header[128];
	SDL_RWread(fp, header, 1, sizeof(header));

	// compare the `DDS ` signature
	SDL_assert_release(memcmp(header, "DDS ", 4) == 0);

	// extract height, width, and amount of mipmaps - yes it is stored height then width
	unsigned int height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	unsigned int width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	unsigned int mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

	// figure out what format to use for what fourCC file type it is
	// block size is about physical chunk storage of compressed data in file (important)
	if (header[84] == 'D') {
		switch (header[87]) {
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		case '0': // DX10
			// unsupported, else will error
			// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
			// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
		default:
			SDL_assert_release(false);
		}
	}
	else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
		SDL_assert_release(false);

	// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
	// read rest of file
	Vector<uint8_t> buffer(SDL_RWsize(fp) - SDL_RWtell(fp));
	SDL_RWread(fp, buffer.data(), buffer.size(), 1);

	// prepare new incomplete texture
	glGenTextures(1, &id);

	// bind the texture
	// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// prepare some variables
	unsigned int offset = 0;
	unsigned int size = 0;
	unsigned int w = width;
	unsigned int h = height;

	// loop through sending block at a time with the magic formula
	// upload to opengl properly, note the offset transverses the pointer
	// assumes each mipmap is 1/2 the size of the previous mipmap
	for (unsigned int i = 0; i < mipMapCount; i++) {
		if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
			mipMapCount--;
			continue;
		}
		size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer.data() + offset);
		offset += size;
		w /= 2;
		h /= 2;
	}
	// discard any odd mipmaps, ensure a complete texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_RWclose(fp);
	return true;
}
