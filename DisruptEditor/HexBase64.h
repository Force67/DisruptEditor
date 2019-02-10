#pragma once

#include <string>
#include <vector>

std::string toHexString(const void *ptr, size_t size);
std::vector<uint8_t> fromHexString(const std::string &str);

std::string toBase64String(const void *ptr, size_t size);
std::vector<uint8_t> fromBase64String(const std::string &str);
