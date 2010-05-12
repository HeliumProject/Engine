///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  7 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CharacterForms__
#define __CharacterForms__

#include <wx/wx.h>

#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////

#define ID_SKELETON_TREE 10000
#define ID_ATTRIBUTES_TREE 10001
#define ID_DYNAMIC_JOINT_LISTER 10002

///////////////////////////////////////////////////////////////////////////////
/// Class MainPanel
///////////////////////////////////////////////////////////////////////////////
class MainPanel : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_SkeletonTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxTreeEvent& event ){ event.Skip(); }
		virtual void OnTreeSelect( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		MainPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 638,539 ), int style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class PropertiesPanel
///////////////////////////////////////////////////////////////////////////////
class PropertiesPanel : public wxPanel 
{
	private:
	
	protected:
	
	public:
		PropertiesPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 305,445 ), int style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AttributesPanel
///////////////////////////////////////////////////////////////////////////////
class AttributesPanel : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_AttributesTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSelect( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		AttributesPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 264,158 ), int style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class JointListPanel
///////////////////////////////////////////////////////////////////////////////
class JointListPanel : public wxPanel 
{
	private:
	
	protected:
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSelect( wxListEvent& event ){ event.Skip(); }
		
	
	public:
		wxListCtrl* m_JointLister;
		JointListPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LooseAttachWizardSingle
///////////////////////////////////////////////////////////////////////////////
class LooseAttachWizardSingle : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_Title;
		wxStaticLine* m_Line;
		wxStaticText* m_NameLabel;
		wxTextCtrl* m_NameText;
		wxTextCtrl* m_StartJointText;
		wxBitmapButton* m_ButtonPickStart;
		
		
		wxButton* m_button5;
		wxButton* m_button6;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnButtonPickStart( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonFinish( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		LooseAttachWizardSingle( wxWindow* parent, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 440,201 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LooseAttachWizardDouble
///////////////////////////////////////////////////////////////////////////////
class LooseAttachWizardDouble : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_Title;
		wxStaticLine* m_Line;
		wxStaticText* m_NameLabel;
		wxTextCtrl* m_NameText;
		wxTextCtrl* m_StartJointText;
		wxBitmapButton* m_ButtonPickStart;
		wxTextCtrl* m_MiddleJointText;
		wxBitmapButton* m_ButtonPickMiddle;
		wxTextCtrl* m_EndJointText;
		wxBitmapButton* m_ButtonPickEnd;
		
		
		wxButton* m_button5;
		wxButton* m_button6;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnButtonPickStart( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonPickMiddle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonPickEnd( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonFinish( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		LooseAttachWizardDouble( wxWindow* parent, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 440,317 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LooseAttachPanel
///////////////////////////////////////////////////////////////////////////////
class LooseAttachPanel : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_LooseAttachTree;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTreeSelect( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		LooseAttachPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 321,357 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__CharacterForms__
