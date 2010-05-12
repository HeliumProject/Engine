#pragma once

#include <wx/statline.h>
#include "API.h"
#include "Common/Types.h"
#include "Common/Compiler.h"


namespace Luna
{
  // Forwards
  class ClassChooserPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog for showing a hierarchy of classes (by name), and allowing the 
  // user to select a class.  All of the UI is contained within the 
  // ClassChooserPanel that is a member of this dialog.
  // 
  class LUNA_EDITOR_API ClassChooserDlg : public wxDialog 
  {
  private:
    typedef std::map< std::string, wxTreeItemId > M_TreeItems;

  private:
    ClassChooserPanel* m_Panel;
    std::string m_SelectedClass;
    M_TreeItems m_TreeItems;

  public:
    ClassChooserDlg( wxWindow* parent, const S_string& baseClasses );
    virtual ~ClassChooserDlg();

    const std::string& GetSelectedClass() const;
    void SetSelectedClass( const std::string& className );

    // Overrides
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

  private:
    void RecurseDerived( const wxTreeItemId& root, const std::string& baseClass );
    void PopulateDropdown(const S_string& baseClasses); 

    // UI Callbacks
  private:
    void OnTreeItemActivated( wxTreeEvent& args );
    void OnTreeItemSelected( wxTreeEvent& args );
    void OnChoice( wxCommandEvent& event );

  private:
    DECLARE_EVENT_TABLE();
  };
}

