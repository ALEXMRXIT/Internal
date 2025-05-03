#pragma once
#include "Engine.h"

struct ViewProjectonData {
	const XMMATRIX& m_view;
	const XMMATRIX& m_projection;

	ViewProjectonData(const XMMATRIX& view, const XMMATRIX& projection)
		: m_view(view), m_projection(projection) {  }
};