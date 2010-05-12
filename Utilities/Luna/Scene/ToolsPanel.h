#pragma once

namespace Luna
{
  class SceneEditor;

  class ToolsPanel : public wxPanel 
  {
	  public:
		  ToolsPanel( SceneEditor* sceneEditor, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 140, 300 ), long style = wxTAB_TRAVERSAL );
		  virtual ~ToolsPanel();
  	
	  private:
      SceneEditor* m_SceneEditor;
		  wxButton* m_button10;
		  wxButton* m_button11;
		  wxButton* m_button12;
		  wxButton* m_button13;
		  wxButton* m_button14;
		  wxButton* m_button15;
		  wxButton* m_button16;
		  wxStaticLine* m_Divider;
		  wxScrolledWindow* m_ScrollWindow;
		  wxStaticText* m_StaticText;  	
  };
}
