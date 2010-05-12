///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LunaGenerated__
#define __LunaGenerated__

namespace UIToolKit{ class Button; }
namespace UIToolKit{ class MenuButton; }

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/scrolwin.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class DrawerFrameBase
	///////////////////////////////////////////////////////////////////////////////
	class DrawerFrameBase : public wxFrame 
	{
		private:
		
		protected:
			wxBoxSizer* m_EmbeddedWindowSizer;
		
		public:
			DrawerFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 325,375 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
			~DrawerFrameBase();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ToolsPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class ToolsPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			UIToolKit::Button* m_ButtonCreateEntity;
			UIToolKit::Button* m_ButtonCreateVolume;
			UIToolKit::Button* m_ButtonCreateClue;
			UIToolKit::MenuButton* m_ButtonCurve;
			wxStaticText* m_staticText1;
			wxStaticText* m_staticText2;
			wxBitmapButton* m_bpButton445;
			wxStaticBitmap* m_CurrentAssetThumbnail;
			wxButton* m_button1;
			wxCheckBox* m_checkBox1;
			wxCheckBox* m_checkBox11;
			wxCheckBox* m_checkBox12;
			wxButton* m_button2;
			wxButton* m_button3;
		
		public:
			ToolsPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 170,325 ), long style = wxTAB_TRAVERSAL );
			~ToolsPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class GameRowGenerated
	///////////////////////////////////////////////////////////////////////////////
	class GameRowGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxBoxSizer* m_InnerSizer;
			wxStaticBitmap* m_Icon1;
			wxStaticBitmap* m_Icon2;
			wxCheckBox* m_CheckBox;
			wxStaticText* m_Label;
		
		public:
			GameRowGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,35 ), long style = wxTAB_TRAVERSAL );
			~GameRowGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class GamePanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class GamePanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxToolBar* m_ToolBar;
			wxScrolledWindow* m_ScrollWindow;
			wxPanel* m_panel2;
			wxStaticBitmap* m_Icon;
			wxCheckBox* m_CheckBox;
			wxStaticText* m_Label;
			
			wxStaticBitmap* m_bitmap10;
			wxPanel* m_panel21;
			wxStaticBitmap* m_Icon1;
			wxCheckBox* m_CheckBox1;
			wxStaticText* m_Label1;
			wxPanel* m_panel22;
			wxStaticBitmap* m_Icon2;
			wxCheckBox* m_CheckBox2;
			wxStaticText* m_Label2;
		
		public:
			GamePanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 345,300 ), long style = wxTAB_TRAVERSAL );
			~GamePanelGenerated();
		
	};
	
} // namespace Luna

#endif //__LunaGenerated__
