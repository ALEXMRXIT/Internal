#include "Serialize.h"
#include "debug.h"

Serialize::Serialize(const char* file_name) {
	m_file = fopen(file_name, "rb+");
	if (!m_file)
		DXUT_ERR_MSGBOX("Error open file!", (HRESULT)-1);
}

Serialize::~Serialize() {
	if (m_file) fclose(m_file);
}