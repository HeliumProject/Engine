#include "Precompile.h"
#include "Editor/Graph/RulerMemoryDC.h"

#include "Editor/Graph/Debug.h"

RulerMemoryDC::RulerMemoryDC()
	: wxMemoryDC()
	, m_Bitmap(1, 1)
{
	// __REMARK__
	// On Linux, wxMemoryDC only computes the
	// bouding box of the drawing if we attach a
	// wxBitmap to it, no matter what size.
	SelectObject(m_Bitmap);
}
