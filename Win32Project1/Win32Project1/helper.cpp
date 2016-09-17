#include "helper.h"

typedef int(*sqlite3_callback)(
	void*,    /* Data provided in the 4th argument of sqlite3_exec() */
	int,      /* The number of columns in row */
	char**,   /* An array of strings representing fields in the row */
	char**    /* An array of strings representing column names */
	);


helper::helper()
{
	// memory cache.
	m_path = ":memory:";
	m_dataBase = NULL;
	init();
}

helper::helper(const std::string& path)
{
	m_path = path;
	m_dataBase = NULL;
	init();
}


helper::~helper()
{
	close();
}

bool helper::init()
{
	int rc;
	sqlite3* db;
	rc = sqlite3_open(m_path.c_str(), &db);//:memory://file::memory:?cache=shared

	if (rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return false;
	}
	m_dataBase = db;
	return true;
}

void helper::close()
{
	if (!m_dataBase)
	{
		return;
	}

	sqlite3_close(m_dataBase);
}

bool helper::exec(const std::string& sqlStr)
{
	if (!m_dataBase)
	{
		return false;
	}

	int rc;
	auto sql = sqlStr.c_str();
	char* errMsg = 0;
	sqlite3_callback callback;
	void* data = NULL;
	rc = sqlite3_exec(m_dataBase, sql, callback, data, &errMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
		return false;
	}

}

#include <fstream>
#include <iostream>
int InsertFile(const std::string& db_name)
{
	std::ifstream file("Sql.pdf", std::ios::in | std::ios::binary);
	if (!file) {
		std::cerr << "An error occurred opening the file\n";
		return 12345;
	}
	file.seekg(0, std::ifstream::end);
	std::streampos size = file.tellg();
	file.seekg(0);

	char* buffer = new char[size];
	file.read(buffer, size);

	sqlite3 *db = NULL;
	int rc = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
	if (rc != SQLITE_OK) {
		std::cerr << "db open failed: " << sqlite3_errmsg(db) << std::endl;
	}
	else {
		sqlite3_stmt *stmt = NULL;
		rc = sqlite3_prepare_v2(db,
			"INSERT INTO ONE(ID, NAME, LABEL, GRP, FILE)"
			" VALUES(NULL, 'some string', NULL, NULL, ?)",
			-1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			std::cerr << "prepare failed: " << sqlite3_errmsg(db) << std::endl;
		}
		else {
			// SQLITE_STATIC because the statement is finalized
			// before the buffer is freed:
			rc = sqlite3_bind_blob(stmt, 1, buffer, size, SQLITE_STATIC);
			if (rc != SQLITE_OK) {
				std::cerr << "bind failed: " << sqlite3_errmsg(db) << std::endl;
			}
			else {
				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
					std::cerr << "execution failed: " << sqlite3_errmsg(db) << std::endl;
			}
		}
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);

	delete[] buffer;
}