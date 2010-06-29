#ifndef __RULERDC_H__
#define __RULERDC_H__

#include <wx/wx.h>

class RulerDC: public wxMemoryDC
{
	public:
		RulerDC();

	private:
		wxBitmap m_Bitmap;
};

#endif /* __RULERDC_H__ */
