///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AnimationEventsPanels__
#define __AnimationEventsPanels__

namespace Luna{ class AnimationTimelineSlider; }

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
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/slider.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AnimationEventEditor
///////////////////////////////////////////////////////////////////////////////
class AnimationEventEditor : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* m_StatusBar;
		wxMenuBar* m_MenuBar;
		wxMenu* m_FileMenu;
		wxToolBar* m_MainToolBar;
	
	public:
		AnimationEventEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 655,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainPanel
///////////////////////////////////////////////////////////////////////////////
class AnimationPanel : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_UpdateClassLabel;
		wxStaticText* m_ClipLabel;
		
		wxPanel* m_panel3;
		wxPanel* m_panel2;
		wxStaticText* m_EventTypeLabel;
		wxStaticText* m_ValueLabel;
		
	
	public:
		wxChoice* m_UpdateClassChoice;
		wxChoice* m_ClipChoice;
		wxCheckBox* m_OverrideEventsCheckbox;
		wxChoice* m_EventTypeChoice;
		wxButton* m_RefreshEvents;
		wxCheckBox* m_ShowOnlyMatchingEvents;
		wxComboBox* m_ValueCombo;
		wxCheckBox* m_Untrigger;
		wxCheckBox* m_InfiniteDuration;
		wxCheckBox* m_DominantOnly;
		wxCheckBox* m_GameplayEvent;
		wxButton* m_AddEventButton;
		wxButton* m_AuditionButton;
		wxButton* m_StopAllButton;
		wxButton* m_DeleteSelection;
                wxButton* m_RebuildMoby;
		AnimationPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 642,355 ), long style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AnimationActivePanel
///////////////////////////////////////////////////////////////////////////////
class AnimationActivePanel : public wxPanel 
{
	private:
	
	protected:
	
	public:
		wxGrid* m_EventGrid;
		AnimationActivePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 612,426 ), long style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AnimationTimelinePanel
///////////////////////////////////////////////////////////////////////////////
class AnimationTimelinePanel : public wxPanel 
{
	private:
	
	protected:
		
		wxStaticText* m_staticText5;
	
	public:
		wxBitmapButton* m_PauseButton;
		wxBitmapButton* m_PlayButton;
		wxTextCtrl* m_GotoFrameTextBox;
		Luna::AnimationTimelineSlider* m_TimeSlider;
		wxPanel* m_MarkersPanel;
		AnimationTimelinePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 646,281 ), long style = wxTAB_TRAVERSAL );
	
};

#endif //__AnimationEventsPanels__
