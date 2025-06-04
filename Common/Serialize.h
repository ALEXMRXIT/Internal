#pragma once
#include "framework.h"

class Serialize {
private:
	FILE* m_file;

private:
	Serialize(const Serialize&) = delete;
	Serialize& operator=(const Serialize&) = delete;

public:
	Serialize(const char* file_name);
	~Serialize();
};