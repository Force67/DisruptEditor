#pragma once

#include "tinyxml2.h"
#include <memory>

struct SDL_RWops;

std::unique_ptr<tinyxml2::XMLDocument> loadRml(SDL_RWops *fp);

std::unique_ptr<tinyxml2::XMLDocument> loadXml(SDL_RWops *fp);


