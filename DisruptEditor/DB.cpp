#include "DB.h"

#include <sqlite_modern_cpp.h>
#include "Hash.h"
#include "Version.h"

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
	try {
		std::string str;
		*db << "select str from hashes where str=?;" << hash >> str;
		return str;
	}
	catch (...) {
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "_%08x", hash);
		return std::string(buffer);
	}
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
		"   hash integer not null,"
		"   path text not null,"
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
		ps << hash << buffer << "";
		ps++;
	}
	fclose(fp);

	//Fill with known FNV
	fp = fopen("res/arches.txt", "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		hash = Hash::getFilenameHash(buffer);
		ps << hash << buffer << "CArchetypeResource";
		ps++;
	}
	fclose(fp);

	fp = fopen("res/archeBrute.txt", "r");
	while (fgets(buffer, sizeof(buffer), fp)) {
		buffer[strlen(buffer) - 1] = '\0';
		hash = Hash::getFilenameHash(buffer);
		ps << hash << buffer << "CArchetypeResource";
		ps++;
	}
	fclose(fp);
	ps.used(true);

	//CRC Hash Table
	*db <<
		"create table if not exists hashes ("
		"   hash integer not null,"
		"   str text not null,"
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
	auto ps = *db << "insert into hashes (hash,str,type) values (?,?,?);";
	while (fgets(line, sizeof(line), fp)) {
		line[strlen(line) - 1] = '\0';

		uint32_t hash = Hash::crc32buf((const char*)line, strlen(line));
		ps << hash << line << type;
		ps++;
	}
	ps.used(true);

	fclose(fp);
}
