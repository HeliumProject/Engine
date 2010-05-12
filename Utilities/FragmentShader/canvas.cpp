#include "canvas.h"

#include <wx/metafile.h>
#include <wx/dcbuffer.h>
#include <math.h>

#include "rulerdc.h"

#include "debug.h"

BEGIN_EVENT_TABLE(Canvas, wxScrolledWindow)
	EVT_SIZE(Canvas::OnResize)
	EVT_LEFT_DOWN(Canvas::OnLeftDown)
	EVT_LEFT_UP(Canvas::OnLeftUp)
	EVT_MOTION(Canvas::OnMotion)
	EVT_MOUSEWHEEL(Canvas::OnMouseWheel)
	EVT_KEY_DOWN(Canvas::OnKeyDown)
	EVT_ERASE_BACKGROUND(Canvas::OnEraseBackground)
	EVT_PAINT(Canvas::OnPaint)
END_EVENT_TABLE()

Canvas::Canvas(wxWindow* parent, int border) :
	wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxHSCROLL | wxVSCROLL),
	m_BorderSize(border),
	m_HandCursor(wxCURSOR_SIZING),
	m_Zoom(1.0f),
	m_OldZoom(0.0f),
	m_Width(0),
	m_Height(0),
	m_Zooming(false)
{
	SetVirtualSize(0, 0);
	SetScrollRate(1, 1);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void
Canvas::OnResize(wxSizeEvent &evt)
{
	Redraw(false);
	evt.Skip();
}

void
Canvas::OnLeftDown(wxMouseEvent &evt)
{
	m_HasDragged = false;
	m_LeftX = evt.GetX() - m_BorderSize;
	m_LeftY = evt.GetY() - m_BorderSize;
	evt.Skip();
}

void
Canvas::OnLeftUp(wxMouseEvent& evt)
{
	if (m_HasDragged)
	{
		m_HasDragged = false;
		Redraw(false);
		SetCursor(*wxSTANDARD_CURSOR);
	}
	evt.Skip();
}

void
Canvas::OnMotion(wxMouseEvent& evt)
{
	m_MouseX = evt.GetX() - m_BorderSize;
	m_MouseY = evt.GetY() - m_BorderSize;
	MouseToCanvas(m_MouseX, m_MouseY, &m_CanvasX, &m_CanvasY);
	if (evt.Dragging() && evt.LeftIsDown())
	{
		int dx = m_LeftX - m_MouseX;
		int dy = m_LeftY - m_MouseY;
		if (m_HasDragged || abs(dx) > 5 || abs(dy) > 5)
		{
			if (!m_HasDragged)
			{
				m_HasDragged = true;
				SetCursor(m_HandCursor);
			}
			m_LeftX = m_MouseX;
			m_LeftY = m_MouseY;
			int viewx, viewy;
			GetViewStart(&viewx, &viewy);
			Scroll(viewx + dx, viewy + dy);
		}
	}
	else
	{
		//SetCursor(*wxSTANDARD_CURSOR);
	}
	evt.Skip();
}

void
Canvas::OnMouseWheel(wxMouseEvent& evt)
{
	if (evt.GetWheelRotation() > 0)
	{
		SetZoom(m_Zoom * 1.25f);
	}
	else
	{
		SetZoom(m_Zoom / 1.25f);
	}
}

void
Canvas::OnKeyDown(wxKeyEvent& evt)
{
	int x, y;

	int key = evt.GetKeyCode();
	switch (key)
	{
		case WXK_PAGEUP:
			SetZoom(m_Zoom * 1.25f);
			break;
		case WXK_PAGEDOWN:
			SetZoom(m_Zoom / 1.25f);
			break;
		case WXK_HOME:
			SetZoom(1.0);
		case WXK_UP:
			GetViewStart(&x, &y);
			Scroll(x, y - 10);
			break;
		case WXK_DOWN:
			GetViewStart(&x, &y);
			Scroll(x, y + 10);
			break;
		case WXK_LEFT:
			GetViewStart(&x, &y);
			Scroll(x - 10, y);
			break;
		case WXK_RIGHT:
			GetViewStart(&x, &y);
			Scroll(x + 10, y);
			break;
		default:
			evt.Skip();
			break;
	}
}

void
Canvas::OnEraseBackground(wxEraseEvent& evt)
{
	(void)evt;
}

void
Canvas::OnPaint(wxPaintEvent& evt)
{
	// __REMARK__
	// Previously we were using a wxMetafileDC to both evaluate the
	// graph size and serve as a cache to draw it to a wxBitmapDC and
	// avoid having to parse the xdot file again. The wxBitmapDC then
	// was used on all paint operations until the user changed the
	// zoom value.
	wxAutoBufferedPaintDC dc(this);
	DoPrepareDC(dc);
	// Get the size of the client area.
	int clwidth, clheight;
	GetClientSize(&clwidth, &clheight);
	// The deltas start at 0.
	int dx = 0, dy = 0;
	// If the width of the drawing is less than the width
	// of the client area, we compute the delta to
	// centralize horizontally. width and height are
	// computed in another place.
	if (m_Width < clwidth)
	{
		dx = (clwidth - m_Width) / 2;
	}
	// The same for the height.
	if (m_Height < clheight)
	{
		dy = (clheight - m_Height) / 2;
	}
	// Get the pre-computed offset.
	wxCoord x, y;
	dc.GetDeviceOrigin(&x, &y);
	// Set the user scale.
	dc.SetUserScale(m_Zoom, m_Zoom);
	// Clear the client area.
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	// Set the offset to the original one plus the deltas plus the border.
	int zborder = (int)(m_BorderSize * m_Zoom);
	dc.SetDeviceOrigin(x + dx + zborder, y + dy + zborder);
	// Paint will draw whatever it wants to draw.
	Paint(dc);
	// Draw a dot at the bottom-right of the DC to account for the border.
	if (m_BorderSize > 0)
	{
		int x = dc.MaxX() + zborder - 1;
		int y = dc.MaxY() + zborder - 1;
		wxColor color;
		dc.GetPixel(x, y, &color);
		wxPen pen(color);
		dc.SetPen(pen);
		dc.DrawPoint(x, y);
		dc.SetPen(wxNullPen);
	}
	// Set the user scale back to 1.0, 1.0 to avoid
	// pixalation.
	// __REMARK__
	// Windows doesn't need it but on Linux
	// we get a pixelated image if we don't do it.
	dc.SetUserScale(1.0, 1.0);
}

bool
Canvas::HasDragged()
{
	return m_HasDragged;
}

void
Canvas::Redraw(bool force)
{
	if (force)
	{
		// __REMARK__
		// The redrawing is actually just an evaluation of the
		// graph size so that we can set the virtual size of the
		// Canvas accordingly.
		RulerDC ruler;
		Paint(ruler);
		// Oddly enough, if we directly set width and height
		// the drawing offsets are uncorrectly computed...
		int neww = (int)ceil((ruler.MaxX() + 1 + m_BorderSize * 2) * m_Zoom);
		int newh = (int)ceil((ruler.MaxY() + 1 + m_BorderSize * 2) * m_Zoom);
		SetVirtualSize(neww, newh);
		if (m_Zooming)
		{
			// Find what the previous coordinates of the mouse
			// would be with the new zoom.
			int previousx, previousy;
			CanvasToMouse(m_CanvasX, m_CanvasY, &previousx, &previousy);
			// Get the view start...
			int x, y;
			GetViewStart(&x, &y);
			// and move it to centralize the drawing in the mouse.
			Scroll(x + previousx - m_MouseX, y + previousy - m_MouseY);
			// Update the canvas position.
			//int cx = m_CanvasX, cy = m_CanvasY;
			MouseToCanvas(m_MouseX, m_MouseY, &m_CanvasX, &m_CanvasY);
		}
		m_Width = neww;
		m_Height = newh;
	}
	// Now we repaint the client area.
	Refresh(false);
}

double
Canvas::SetZoom(double zoom)
{
	if (zoom > 0.1f && zoom < 4.0f)
	{
		double old = m_Zoom;
		m_Zoom = zoom;
		// Set the zooming flag to disable ScrollWindow.
		m_Zooming = true;
		Redraw(true);
		// Reset it for normal operation.
		m_Zooming = false;
		return old;
	}
	return m_Zoom;
}

double
Canvas::GetZoom()
{
	return m_Zoom;
}

wxPoint
Canvas::GetCenter()
{
	// Top left.
	int left, top;
	MouseToCanvas(0, 0, &left, &top);
	// Bottom right;
	wxSize size = GetClientSize();
	int right, bottom;
	MouseToCanvas(size.GetWidth() - 1, size.GetHeight() - 1, &right, &bottom);
	// Return the center.
	return wxPoint((left + right) / 2, (top + bottom) / 2);
}

void
Canvas::Center(int x, int y)
{
	wxSize size = GetClientSize();
	Scroll(x * m_Zoom - size.GetWidth() / 2, y * m_Zoom - size.GetHeight() / 2);
}

bool
Canvas::CopyAsMetafile()
{
#ifdef _WIN32
	wxMetafileDC mdc;
	Paint(mdc);
	wxMetafile *meta = mdc.Close();
	bool ok = meta->SetClipboard();
	DESTROY(meta);
	return ok;
#else
	return false;
#endif
}

void
Canvas::ScrollWindow(int dx, int dy, const wxRect *rect)
{
	// If we're zooming, we disable the physical copy of the
	// pixels and let OnPaint redraw the client area.
	if (!m_Zooming)
	{
		wxScrolledWindow::ScrollWindow(dx, dy, rect);
	}
}

bool
Canvas::SetCursor(const wxCursor& cursor)
{
	bool ret = wxScrolledWindow::SetCursor(cursor);
	// WarpPointer is necessary to force cursor to change
	wxPoint pos = ::wxGetMousePosition();
	ScreenToClient(&pos.x, &pos.y);
	WarpPointer(pos.x, pos.y);
	return ret;
}

void
Canvas::MouseToCanvas(int mousex, int mousey, int *bmx, int *bmy)
{
	int clwidth, clheight;
	GetClientSize(&clwidth, &clheight);
	int dx = 0, dy = 0;
	if (m_Width < clwidth)
	{
		dx = (clwidth - m_Width) / 2;
	}
	if (m_Height < clheight)
	{
		dy = (clheight - m_Height) / 2;
	}
	wxClientDC dc(this);
	DoPrepareDC(dc);
	dc.SetUserScale(m_Zoom, m_Zoom);
	int zborder = (int)(m_BorderSize * m_Zoom);
	*bmx = dc.DeviceToLogicalX(mousex - dx - zborder);
	*bmy = dc.DeviceToLogicalY(mousey - dy - zborder);
}

void
Canvas::CanvasToMouse(int bmx, int bmy, int *mousex, int *mousey)
{
	int clwidth, clheight;
	GetClientSize(&clwidth, &clheight);
	int dx = 0, dy = 0;
	if (m_Width < clwidth)
	{
		dx = (clwidth - m_Width) / 2;
	}
	if (m_Height < clheight)
	{
		dy = (clheight - m_Height) / 2;
	}
	wxClientDC dc(this);
	DoPrepareDC(dc);
	dc.SetUserScale(m_Zoom, m_Zoom);
	int zborder = (int)(m_BorderSize * m_Zoom);
	*mousex = dc.LogicalToDeviceX(bmx) + dx + zborder;
	*mousey = dc.LogicalToDeviceY(bmy) + dy + zborder;
}
