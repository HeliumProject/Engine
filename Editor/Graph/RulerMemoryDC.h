#pragma once

class RulerMemoryDC: public wxMemoryDC
{
	public:
		RulerMemoryDC();

	private:
		wxBitmap m_Bitmap;
};

