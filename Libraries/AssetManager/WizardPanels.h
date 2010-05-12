///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  7 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WizardPanels__
#define __WizardPanels__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/bmpbuttn.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

///////////////////////////////////////////////////////////////////////////

namespace AssetManager
{
	#define ID_DEFAULT wxID_ANY // Default
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LocationPanel
	///////////////////////////////////////////////////////////////////////////////
	class LocationPanel : public wxPanel 
	{
		private:
		
		protected:
			
			wxStaticText* m_StaticName;
			
			wxStaticText* m_StaticDirectory;
			wxStaticText* m_StaticNewPath;
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxTextCtrl* m_Name;
			wxCheckBox* m_checkBoxCreateSubfolder;
			wxTextCtrl* m_Directory;
			wxButton* m_ButtonDirectoryPicker;
			wxTextCtrl* m_NewAssetPath;
			wxScrolledWindow* m_ScrollWindow;
			wxPanel* m_DuplicatePathPanel;
			wxStaticText* m_StaticOrigPath;
			wxTextCtrl* m_OriginalAssetPath;
			LocationPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ShaderTypeSelectionPanel
	///////////////////////////////////////////////////////////////////////////////
	class ShaderTypeSelectionPanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxScrolledWindow* m_ScrollWindow;
			ShaderTypeSelectionPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class AttribsPanel
	///////////////////////////////////////////////////////////////////////////////
	class AttribsPanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxScrolledWindow* m_ScrollWindowAttributes;
			wxStaticText* m_StaticAttribDescHeading;
			wxStaticText* m_StaticAttribDescBody;
			AttribsPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class TextureMapPanel
	///////////////////////////////////////////////////////////////////////////////
	class TextureMapPanel : public wxPanel 
	{
		private:
		
		protected:
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnFieldTextEnter( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnFileBrowserButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnExplorerButtonClick( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			wxPanel* m_TextureMapPanel;
			wxStaticText* m_FieldLabel;
			wxTextCtrl* m_FilePathField;
			wxButton* m_ExplorerButton;
			wxBitmapButton* m_FileBrowserButton;
			TextureMapPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 540,-1 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ShaderTexturesPanel
	///////////////////////////////////////////////////////////////////////////////
	class ShaderTexturesPanel : public wxPanel 
	{
		private:
		
		protected:
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnFieldTextEnter( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnFileBrowserButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnExplorerButtonClick( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxScrolledWindow* m_ScrollWindow;
			wxPanel* m_TextureMapPanel;
			wxStaticText* m_FieldLabel;
			wxTextCtrl* m_FilePathField;
			wxButton* m_ExplorerButton;
			wxBitmapButton* m_FileBrowserButton;
			ShaderTexturesPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class FileBackedAttrPanel
	///////////////////////////////////////////////////////////////////////////////
	class FileBackedAttrPanel : public wxPanel 
	{
		private:
		
		protected:
			
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxRadioButton* m_RadioBtnNew;
			wxTextCtrl* m_FilePathNew;
			wxRadioButton* m_RadioBtnExisting;
			wxTextCtrl* m_FilePathExisting;
			wxButton* m_ButtonExisting;
			wxBitmapButton* m_ButtonFindExisting;
			FileBackedAttrPanel( wxWindow* parent, int id = ID_DEFAULT, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class CinematicAttrLevelZonePanel
	///////////////////////////////////////////////////////////////////////////////
	class CinematicAttrLevelZonePanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxTextCtrl* m_FilePathLevel;
			wxButton* m_ButtonBrowse;
			wxBitmapButton* m_ButtonFind;
			wxCheckBox* m_CheckBoxAllZones;
			wxListBox* m_ListBoxZones;
			CinematicAttrLevelZonePanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class GridPanel
	///////////////////////////////////////////////////////////////////////////////
	class GridPanel : public wxPanel 
	{
		private:
		
		protected:
			wxPanel* m_SelectButtonsPanel;
			
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxPropertyGridManager* m_PropertyGrid;
			wxButton* m_SelectAllButton;
			wxButton* m_UnselectAllButton;
			GridPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class SummaryPanel
	///////////////////////////////////////////////////////////////////////////////
	class SummaryPanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxStaticText* m_Title;
			wxStaticText* m_Description;
			wxTextCtrl* m_SummaryTextBox;
			SummaryPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 550,400 ), int style = wxTAB_TRAVERSAL );
		
	};
	
} // namespace AssetManager

#endif //__WizardPanels__
