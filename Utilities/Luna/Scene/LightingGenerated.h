///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LightingGenerated__
#define __LightingGenerated__

namespace Luna{ class LightingJobTree; }
namespace Luna{ class LightingListLight; }
namespace Luna{ class LightingListLightingEnvironment; }
namespace Luna{ class LightingListObject; }
namespace Luna{ class LightingListProbe; }
namespace Luna{ class LightingListVolume; }
namespace Luna{ class LightingListZone; }

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	#define ID_DEFAULT wxID_ANY // Default
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class InnerLightingPanel
	///////////////////////////////////////////////////////////////////////////////
	class InnerLightingPanel : public wxPanel 
	{
		private:
		
		protected:
			
			wxSplitterWindow* m_VerticalSplitter;
			wxPanel* m_PanelLeft;
			wxPanel* m_PanelRight;
			
			
			
			
		
		public:
			wxButton* m_ButtonRender;
			wxChoice* m_ChoiceRenderType;
			wxChoice* m_ChoiceRenderLocation;
			wxButton* m_ButtonView;
			wxButton* m_ButtonUpdateCasters;
			wxButton* m_ButtonUpdateLit;
			wxButton* m_ButtonCheckSetup;
			wxButton* m_ButtonViewUVs;
			wxButton* m_ButtonQuickSetup;
			wxButton* m_ButtonQuickRender;
			wxNotebook* m_Tabs;
			wxPanel* m_TabObjects;
			Luna::LightingListObject* m_LightableInstanceList;
			wxPanel* m_TabLights;
			Luna::LightingListLight* m_LightList;
			wxPanel* m_TabVolumes;
			Luna::LightingListVolume* m_VolumeList;
			wxPanel* m_TabProbes;
			Luna::LightingListProbe* m_ProbeList;
			wxPanel* m_TabLightingEnvironments;
			Luna::LightingListLightingEnvironment* m_EnvironmentList;
			wxPanel* m_TabZones;
			Luna::LightingListZone* m_ZoneList;
			wxButton* m_ButtonNewJob;
			wxButton* m_ButtonDeleteJob;
			wxButton* m_ButtonCopyJob;
			wxButton* m_ButtonAddJobMembers;
			wxButton* m_ButtonRemoveJobMembers;
			wxButton* m_ButtonSort;
			wxButton* m_ButtonSync;
			Luna::LightingJobTree* m_JobTree;
			wxButton* m_ButtonSelect;
			wxChoice* m_ChoiceSelectType;
			wxButton* m_ButtonSetTPM;
			wxTextCtrl* m_ValueTPMMax;
			wxTextCtrl* m_ValueTPMStep;
			InnerLightingPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,629 ), long style = wxTAB_TRAVERSAL );
			~InnerLightingPanel();
			void m_VerticalSplitterOnIdle( wxIdleEvent& )
			{
			m_VerticalSplitter->SetSashPosition( 0 );
			m_VerticalSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( InnerLightingPanel::m_VerticalSplitterOnIdle ), NULL, this );
			}
			
		
	};
	
} // namespace Luna

#endif //__LightingGenerated__
