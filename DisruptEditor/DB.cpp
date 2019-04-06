#include "DB.h"

#include <sqlite_modern_cpp.h>
#include "Hash.h"
#include "Version.h"
#include "SDL_log.h"

DB::DB() {
	db = new sqlite::database("Disrupt1.db");

	int version;
	*db << "PRAGMA user_version;" >> version;
	if (version != DE_VERSION)
		reinit();
}

DB::~DB() {
	delete db;
}

std::unique_ptr<DB::File> DB::getFileByHash(uint32_t hash) {
	try {
		std::unique_ptr<DB::File> ptr = std::make_unique<DB::File>();
		ptr->hash = hash;
		*db << "select path, type from files where hash=?;" << hash >> tie(ptr->path, ptr->type);
		return ptr;
	}
	catch (...) { }
	return NULL;
}

std::unique_ptr<DB::File> DB::getFileByPath(const char* path) {
	try {
		std::unique_ptr<DB::File> ptr = std::make_unique<DB::File>();
		ptr->path = path;
		*db << "select hash, type from files where path=?;" << path >> tie(ptr->hash, ptr->type);
		return ptr;
	}
	catch (...) {}
	return NULL;
}

std::string DB::getStrFromCRC(uint32_t hash) {
	//Yes, I know we have a DB for this, but it's just too slow otherwise
	static std::unordered_map<uint32_t, std::string> cache;
	auto it = cache.find(hash);
	if (it != cache.end())
		return it->second;

	try {
		std::string str;
		*db << "select str from hashes where hash=?;" << hash >> str;
		cache[hash] = str;
		return str;
	}
	catch (...) {
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "_%08x", hash);
		cache[hash] = buffer;
		return std::string(buffer);
	}
}

std::string DB::getStrFromDobbs(uint32_t hash) {
	return std::string();
}

void DB::reinit() {
	if (db)
		delete db;

	//Delete File
	fclose(fopen("Disrupt1.db", "wb"));
	db = new sqlite::database("Disrupt1.db");

	*db << "begin;";
	*db << "PRAGMA user_version = " DE_VERSIONSTR ";";

	*db <<
		"create table if not exists files ("
		"   hash integer unique not null,"
		"   path text primary key not null,"
		"   type text not null"
		");";

	char buffer[500];
	uint32_t hash;

	//Fill with Known files
	FILE* fp = fopen("res/Watch Dogs.filelist", "r");
	auto ps = *db << "insert into files (hash,path,type) values (?,?,?);";
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		hash = Hash::getFilenameHash(buffer);
		try {
			ps << hash << buffer << "";
			ps++;
		}
		catch (...) {
			SDL_Log("Duplicate key %u %s", hash, buffer);
		}
	}
	fclose(fp);

	//Fill with known FNV
	fp = fopen("res/arches.txt", "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		hash = Hash::getFilenameHash(buffer);
		try {
			ps << hash << buffer << "CArchetypeResource";
			ps++;
		}
		catch (...) {
			SDL_Log("Duplicate key %u %s", hash, buffer);
		}
	}
	fclose(fp);

	fp = fopen("res/archeBrute.txt", "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		hash = Hash::getFilenameHash(buffer);
		try {
			ps << hash << buffer << "CArchetypeResource";
			ps++;
		}
		catch (...) {
			SDL_Log("Duplicate key %u %s", hash, buffer);
		}
	}
	fclose(fp);
	ps.used(true);

	//CRC Hash Table
	*db <<
		"create table if not exists hashes ("
		"   hash integer unique not null,"
		"   dobbs integer not null,"
		"   str text primary key not null,"
		"   type text not null"
		");";

	handleCRCFile("res/classNames.txt", "ClassNames");
	handleCRCFile("res/exeStrings.txt", "etc");
	handleCRCFile("res/strings.txt", "etc");
	handleCRCFile("res/materialNames.txt", "Material");

	*db << "commit;";
}

DB & DB::instance() {
	static DB db;
	return db;
}

void DB::handleCRCFile(const char *file, const char* type) {
	FILE *fp = fopen(file, "r");

	char line[512];
	auto ps = *db << "insert into hashes (hash,dobbs,str,type) values (?,?,?,?);";
	while (fgets(line, sizeof(line), fp)) {
		line[strlen(line) - 1] = '\0';

		uint32_t hash = Hash::crc32buf((const char*)line, strlen(line));
		uint32_t dobbs = Hash::gearDobbsHash((const unsigned char*)line, strlen(line));
		try {
			ps << hash << dobbs << line << type;
			ps++;
		}
		catch (...) {
			SDL_Log("Duplicate key %u %s", hash, line);
		}
	}
	ps.used(true);

	fclose(fp);
}
