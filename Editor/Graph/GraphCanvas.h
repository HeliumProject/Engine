#pragma once

class GraphCanvas: public wxScrolledWindow
{
	public:
		GraphCanvas(wxWindow *parent, int border = 0);

		virtual void Paint(wxDC& dc) = 0;

		void OnResize(wxSizeEvent& evt);
		void OnLeftDown(wxMouseEvent& evt);
		void OnLeftUp(wxMouseEvent& evt);
		void OnMotion(wxMouseEvent& evt);
		void OnMouseWheel(wxMouseEvent& evt);
		void OnMouseLost(wxMouseCaptureLostEvent& evt);
		void OnKeyDown(wxKeyEvent& evt);
		void OnEraseBackground(wxEraseEvent& evt);
		void OnPaint(wxPaintEvent& evt);
		//virtual void OnDraw(wxDC& dc);

		bool HasDragged();
		void Redraw(bool force);
		double SetZoom(double factor);
		double GetZoom();
		wxPoint GetCenter();
		void Center(int x, int y);
		bool CopyAsMetafile();

		virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);

	protected:
		bool SetCursor(const wxCursor& cursor);
		void MouseToCanvas(int mousex, int mousey, int *bmx, int *bmy);
		void CanvasToMouse(int bmx, int bmy, int *mousex, int *mousey);

	private:
		int			m_BorderSize;
		wxCursor	m_HandCursor;
		double		m_Zoom, m_OldZoom;
		bool		m_HasDragged;
		int			m_MouseX, m_MouseY, m_CanvasX, m_CanvasY, m_LeftX, m_LeftY;
		int			m_Width, m_Height;
		bool		m_Zooming;
		//std::vector<wxRect> m_Clickable;
	
	DECLARE_NO_COPY_CLASS(GraphCanvas)
	DECLARE_EVENT_TABLE()
};

