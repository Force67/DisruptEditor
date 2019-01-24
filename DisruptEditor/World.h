#pragma once

#include <memory>
#include <map>
#include "tinyxml2.h"
#include "wluFile.h"

class World {
public:
	std::unique_ptr<tinyxml2::XMLDocument> particles;
	std::unique_ptr<tinyxml2::XMLDocument> spawnPointList;
	std::map<std::string, wluFile> wlus;
};

extern World world;
