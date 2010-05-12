///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CinematicEventsPanels__
#define __CinematicEventsPanels__

namespace Luna{ class CinematicTimelineSlider; }

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/slider.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CinematicEventEditor
///////////////////////////////////////////////////////////////////////////////
class CinematicEventEditor : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* m_StatusBar;
		wxMenuBar* m_MenuBar;
		wxMenu* m_FileMenu;
		wxToolBar* m_MainToolBar;
	
	public:
		CinematicEventEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 655,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~CinematicEventEditor();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainPanel
///////////////////////////////////////////////////////////////////////////////
class CinematicPanel : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panel3;
		wxPanel* m_panel2;
		wxStaticText* m_ActorLabel;
		wxStaticText* m_JointLabel;
		
	
	public:
		wxChoice* m_ActorChoice;
		wxChoice* m_JointChoice;
		wxCheckBox* m_Untrigger;
		wxCheckBox* m_InfiniteDuration;
		wxButton* m_AddEventButton;
		wxButton* m_RebuildButton;
		wxButton* m_StopAllButton;
		wxButton* m_DeleteSelection;
		CinematicPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 601,372 ), long style = wxTAB_TRAVERSAL );
		~CinematicPanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CinematicActivePanel
///////////////////////////////////////////////////////////////////////////////
class CinematicActivePanel : public wxPanel 
{
	private:
	
	protected:
	
	public:
		wxGrid* m_EventGrid;
		CinematicActivePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 612,426 ), long style = wxTAB_TRAVERSAL );
		~CinematicActivePanel();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CinematicTimelinePanel
///////////////////////////////////////////////////////////////////////////////
class CinematicTimelinePanel : public wxPanel 
{
	private:
	
	protected:
		
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
	
	public:
		wxBitmapButton* m_PauseButton;
		wxBitmapButton* m_PlayButton;
		wxTextCtrl* m_GotoFrameTextBox;
		wxTextCtrl* m_MoveSelectedFrames;
		Luna::CinematicTimelineSlider* m_TimeSlider;
		wxPanel* m_MarkersPanel;
		wxStaticText* m_StartFrame;
		wxTextCtrl* m_ZoomStart;
		wxTextCtrl* m_ZoomEnd;
		wxStaticText* m_EndFrame;
		CinematicTimelinePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 638,281 ), long style = wxTAB_TRAVERSAL );
		~CinematicTimelinePanel();
	
};

#endif //__CinematicEventsPanels__
