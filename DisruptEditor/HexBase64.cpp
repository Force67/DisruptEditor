#include "HexBase64.h"

std::string toHexString(const void * ptr, size_t size) {
	std::string ret;
	const uint8_t *p = (const uint8_t*)ptr;

	for (size_t i = 0; i < size; ++i) {
		char strbuffer[4];
		snprintf(strbuffer, sizeof(strbuffer), "%02x ", p[i]);
		ret.append(strbuffer);
	}

	if (!ret.empty())
		ret.pop_back();

	return ret;
}

static int char2int(char input) {
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}

std::vector<uint8_t> fromHexString(const std::string &str) {
	std::vector<uint8_t> buffer;
	buffer.reserve((str.size() / 2) + 1);

	std::string accum;
	for (auto it = str.begin(); it != str.end(); ++it) {
		if (*it == ' ') continue;

		accum += *it;

		if (accum.size() == 2) {
			buffer.push_back((char2int(accum[0]) << 4) + char2int(accum[1]));
			accum.clear();
		}
	}
	return buffer;
}

const static char lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static char padchar = '=';
std::string toBase64String(const void * ptr, size_t size) {
	std::string str;
	const uint8_t *p = (const uint8_t*)ptr;

	int outputSize = 4 * ((size / 3) + (size % 3 > 0 ? 1 : 0));
	str.reserve(outputSize);

	for (size_t i = 0; i < size; i += 3) {
		int padding = (i + 3) - size;
		uint32_t tmp = 0;

		switch (padding) {
		case 2:
			tmp |= (p[i] << 16);
			break;
		case 1:
			tmp |= (p[i] << 16) | (p[i + 1] << 8);
			break;
		default:
			tmp |= (p[i] << 16) | (p[i + 1] << 8) | (p[i + 2]);
			break;
		}

		str += lookup[(tmp & 0xfc0000) >> 18];
		str += lookup[(tmp & 0x03f000) >> 12];
		str += padding > 1 ? padchar : lookup[(tmp & 0x000fc0) >> 6];
		str += padding > 0 ? padchar : lookup[(tmp & 0x00003f)];
	}

	return str;
}

static char dlookup[256];
static bool dlookupok = false;
const char *getdlookup() {
	if (!dlookupok) {
		memset(dlookup, 0, sizeof(dlookup));
		for (int i = 0; i < strlen(lookup); i++)
			dlookup[(int)lookup[i]] = i;
		dlookupok = true;
	}
	return dlookup;
}

std::vector<uint8_t> fromBase64String(const std::string &str) {
	const char *dlookup = getdlookup();
	std::vector<uint8_t> decoded;

	int outputSize = (str.size() / 4) * 3;
	decoded.reserve(outputSize);

	char ch[4];
	int padding = 0;

	for (int i = 0; i < str.size(); i += 4) {
		for (int j = 0; j < 4; j++) {
			if (str[i + j] == padchar) {
				ch[j] = '\0';
				padding++;
			}
			else {
				ch[j] = dlookup[str[i + j]];
			}
		}

		uint32_t tmp = 0;
		tmp |= (ch[0] << 18) | (ch[1] << 12) | (ch[2] << 6) | ch[3];

		decoded.push_back((tmp & 0xff0000) >> 16);
		decoded.push_back((tmp & 0xff00) >> 8);
		decoded.push_back((tmp & 0xff));
	}

	return decoded;
}
