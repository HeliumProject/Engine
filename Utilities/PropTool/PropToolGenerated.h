///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PropToolGenerated__
#define __PropToolGenerated__

class Workspace;

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include "UIToolkit/ImageViewPanel.h"
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/statusbr.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PropToolFrameBase
///////////////////////////////////////////////////////////////////////////////
class PropToolFrameBase : public wxFrame 
{
	private:
	
	protected:
		wxSplitterWindow* m_MainSplitter;
		wxPanel* m_NotebookPanel;
		wxNotebook* m_Notebook;
		wxPanel* m_ShaderPanel;
		wxListBox* m_ShaderListBox;
		wxStaticText* m_staticText7;
		wxChoice* m_AlphaMode;
		wxStaticText* m_ColorMapLabel;
		wxFilePickerCtrl* m_ColorMapFilePicker;
		Nocturnal::ImageViewPanel* m_ColorMapPreviewPanel;
		wxStaticText* m_NormalMapLabel;
		wxFilePickerCtrl* m_NormalMapFilePicker;
		Nocturnal::ImageViewPanel* m_NormalMapPreviewPanel;
		wxStaticText* m_GPIMapLabel;
		wxFilePickerCtrl* m_GPIMapFilePicker;
		Nocturnal::ImageViewPanel* m_GlossMapPreviewPanel;
		wxStaticText* m_GlossMapPreviewLabel;
		Nocturnal::ImageViewPanel* m_ParallaxMapPreviewPanel;
		wxStaticText* m_ParallaxMapPreviewLabel;
		Nocturnal::ImageViewPanel* m_IncanMapPreviewPanel;
		wxStaticText* m_IncanMapPreviewLabel;
		Workspace* m_Workspace;
		wxStatusBar* m_StatusBar;
		wxMenuBar* m_MainMenubar;
		wxMenu* m_FileMenu;
		wxMenu* m_HelpMenu;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnShaderSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAlphaModeChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnColorMapFileChanged( wxFileDirPickerEvent& event ){ event.Skip(); }
		virtual void OnNormalMapFileChanged( wxFileDirPickerEvent& event ){ event.Skip(); }
		virtual void OnGPIMapFileChanged( wxFileDirPickerEvent& event ){ event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		PropToolFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 829,760 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~PropToolFrameBase();
		void m_MainSplitterOnIdle( wxIdleEvent& )
		{
		m_MainSplitter->SetSashPosition( 470 );
		m_MainSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PropToolFrameBase::m_MainSplitterOnIdle ), NULL, this );
		}
		
	
};

#endif //__PropToolGenerated__
