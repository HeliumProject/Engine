///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorChooserInnerPanel__
#define __EditorChooserInnerPanel__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

///////////////////////////////////////////////////////////////////////////////
/// Class EditorChooserInnerPanel
///////////////////////////////////////////////////////////////////////////////
class EditorChooserInnerPanel : public wxPanel 
{
	private:
	
	protected:
		wxScrolledWindow* m_ScrollWindow;
		wxStaticText* m_TextGetAssets;
		wxStaticText* m_TextRunGame;
		wxStaticText* m_TextRunViewer;
		wxStaticText* m_TextAssetEditor;
		wxStaticText* m_TextSceneEditor;
		wxStaticText* m_TextEffectEditor;
		wxStaticText* m_TextCinematicEditor;
		wxStaticText* m_TextCharacterEditor;
	
	public:
		wxBitmapButton* m_ButtonGetAssets;
		wxBitmapButton* m_ButtonRunGame;
		wxBitmapButton* m_ButtonLive;
		wxBitmapButton* m_ButtonAssetEditor;
		wxBitmapButton* m_ButtonSceneEditor;
		wxBitmapButton* m_ButtonAnimationEventsEditor;
		wxBitmapButton* m_ButtonCinematicEventsEditor;
		wxBitmapButton* m_ButtonCharacterEditor;
		EditorChooserInnerPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,440 ), long style = wxTAB_TRAVERSAL );
		~EditorChooserInnerPanel();
	
};

#endif //__EditorChooserInnerPanel__
