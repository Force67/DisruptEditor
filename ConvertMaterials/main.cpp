#include <stdio.h>
#include <string>
#include "SDL.h"

#include "materialFile.h"
#include "IBinaryArchive.h"
#include "Serialization.h"

void convertMatToXML(const std::string &filename, const std::string &outFilename) {
	SDL_RWops* fp = SDL_RWFromFile(filename.c_str(), "rb");
	if (!fp) {
		printf("Failed to open file for reading\n");
		return;
	}
	CBinaryArchiveReader reader(fp);
	materialFile mat;
	mat.open(reader);
	SDL_RWclose(fp);

	std::string str = serializeToXML(mat);
	fp = SDL_RWFromFile(outFilename.c_str(), "wb");
	if (!fp) {
		printf("Failed to open file for writing\n");
		return;
	}
	SDL_RWwrite(fp, str.data(), str.size(), 1);
	SDL_RWclose(fp);
}

void convertXMLToMat(const std::string &filename, const std::string &outFilename) {
	SDL_RWops* fp = SDL_RWFromFile(filename.c_str(), "rb");
	if (!fp) {
		printf("Failed to open file for reading\n");
		return;
	}
	Vector<char> str(SDL_RWsize(fp) + 1);
	SDL_RWread(fp, str.data(), 1, str.size());
	SDL_RWclose(fp);

	materialFile mat;
	unserializeFromXML(mat, str.data());

	fp = SDL_RWFromFile(outFilename.c_str(), "wb");
	if (!fp) {
		printf("Failed to open file for writing\n");
		return;
	}
	CBinaryArchiveWriter writer(fp);
	mat.open(writer);
	SDL_RWclose(fp);
}

bool hasEnding(const std::string  &fullString, const std::string &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	return false;
}

int main(int argc, char **argv) {
	printf("Disrupt Editor - Material Converter Standalone\nCreated by Jon : https://github.com/j301scott/DisruptEditor\n");

	if (argc < 2) {
		printf("Usage: ConvertMaterials.exe test.material.bin - Converts to XML\n");
		printf("Usage: ConvertMaterials.exe test.material.bin.xml - Converts to BIN\n");
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		printf("Converting file %s\n", argv[i]);
		std::string filename(argv[i]);

		if (hasEnding(filename, ".material.bin.xml")) {
			std::string outFilename = filename.substr(0, filename.size() - 4);
			printf("Converting to BIN: %s\n", outFilename.c_str());
			convertXMLToMat(filename, outFilename);
		} else if(hasEnding(filename, ".material.bin")) {
			std::string outFilename = filename + ".xml";
			printf("Converting to XML: %s\n", outFilename.c_str());
			convertMatToXML(filename, outFilename);
		} else {
			printf("I'm not sure what to do with this file based on it's file extension ¯\_(ツ)_/¯");
		}

		printf("\n");
	}

	SDL_Delay(3 * 1000);

	return 0;
}