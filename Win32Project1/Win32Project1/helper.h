#pragma once
#include <mutex>
#include <string>
#include "sqlite3.h"

//static std::mutex global_mutex;

class helper
{
public:
	// memory cache.
	helper();
	helper(const std::string& path);
	~helper();

	bool init();
	void close();
	bool exec(const std::string& sqlStr);

private:
	std::mutex global_mutex;
	std::string m_path;
	sqlite3* m_dataBase;
};

