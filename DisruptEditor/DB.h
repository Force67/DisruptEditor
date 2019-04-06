#pragma once

#include <string>

namespace sqlite {
	class database;
}

class DB {
public:
	DB();
	~DB();

	sqlite::database *db;

	struct File {
		uint32_t hash;
		std::string path;
		std::string type;
	};
	std::unique_ptr<File> getFileByHash(uint32_t hash);
	std::unique_ptr<File> getFileByPath(const char *path);

	std::string getStrFromCRC(uint32_t hash);
	std::string getStrFromDobbs(uint32_t hash);

	void reinit();
	static DB& instance();
private:
	void handleCRCFile(const char* file, const char* type);
};

