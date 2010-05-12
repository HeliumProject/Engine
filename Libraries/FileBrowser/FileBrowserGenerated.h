///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FileBrowserGenerated__
#define __FileBrowserGenerated__

namespace UIToolKit{ class AutoCompleteComboBox; }
namespace UIToolKit{ class SortableListView; }

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/dialog.h>

#include "API.h"

///////////////////////////////////////////////////////////////////////////

namespace File
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class FileBrowserGenerated
	///////////////////////////////////////////////////////////////////////////////
	class FILEBROWSER_API FileBrowserGenerated : public wxDialog 
	{
		private:
		
		protected:
			enum
			{
				ID_DEFAULT = wxID_ANY, // Default
				ID_SearchQuery = 1000,
				ID_FileType,
				ID_LookInPath,
				ID_BrowseButton,
				ID_AssetTUID,
				ID_SearchButton,
				ID_ClearButton,
				ID_ListCtrlResults,
			};
			
			wxStaticText* m_staticTextMessage;
			wxStaticText* m_staticTextSearchBasic;
			UIToolKit::AutoCompleteComboBox* m_comboBoxSearch;
			wxStaticText* m_staticTextFileType;
			wxChoice* m_choiceFiletype;
			wxStaticText* m_staticTextLookIn;
			UIToolKit::AutoCompleteComboBox* m_comboBoxLookIn;
			wxButton* m_buttonBrowse;
			wxStaticLine* m_staticline8;
			wxStaticText* m_staticTextTUID;
			UIToolKit::AutoCompleteComboBox* m_comboBoxTUID;
			wxCheckBox* m_checkBoxSeachHistoryData;
			wxStaticLine* m_staticline3;
			wxStaticText* m_staticTextSearching;
			wxStaticText* m_staticTextLookingIn;
			wxButton* m_buttonSearch;
			wxButton* m_buttonClear;
			UIToolKit::SortableListView* m_listCtrlResults;
			wxScrolledWindow* m_DetailsScrolledWindow;
			
			wxPanel* m_panelDialogButtons;
			
			wxStaticLine* m_staticlineButtonLine;
			wxButton* m_buttonOperation;
			wxButton* m_buttonCancel;
		
		public:
			wxPanel* m_panelBasic;
			wxStaticBitmap* m_bitmapThumbnail;
			wxTextCtrl* m_textCtrlDetails;
			FileBrowserGenerated( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxString& title = wxT("Asset Finder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_DIALOG_STYLE|wxWANTS_CHARS );
			~FileBrowserGenerated();
		
	};
	
} // namespace File

#endif //__FileBrowserGenerated__
