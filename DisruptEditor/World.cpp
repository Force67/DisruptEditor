#include "World.h"

#include "FileHandler.h"

World world;

void World::loadWLUAsync() {
	Vector<FileInfo> files = FH::getFileList("worlds/windy_city/generated/wlu", "xml.data.fcb");
	int i = 0;
	for (FileInfo& file : files) {
		world.loadingStatus = "Loading " + file.name;
		std::shared_ptr<wluFile> wlu = std::make_shared<wluFile>();
		wlu->shortName = file.name;
		wlu->open(file.fullPath);
		world.wlus[file.name] = wlu;
		++i;
		world.loadingProgress = (float)i / files.size();
	}
}
