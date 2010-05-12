#pragma once

#include "API.h"

#include "TUID/TUID.h"
#include "UIToolKit/FileDialog.h"


namespace File
{
  class FILEUI_API ManagedFileDialog : public UIToolKit::FileDialog
  {
  private:
    // True if this dialog requires a TUID for any file that is chosen.
    bool m_IsTuidRequired;  

    // If IsTuidRequired() this will have the TUIDs for all selected files.
    S_tuid m_FileIDs;

    // If this is a single file picker, and the file picked does not already have a tuid, 
    // this is the tuid to use (as long as it is not assigned to something else).
    tuid m_RequestedFileID;   

  public:
    ManagedFileDialog( wxWindow *parent,
      const wxString& message = wxFileSelectorPromptStr,
      const wxString& defaultDir = wxEmptyString,
      const wxString& defaultFile = wxEmptyString,
      const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
      UIToolKit::FileDialogStyle style = UIToolKit::FileDialogStyles::DefaultOpen,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& sz = wxDefaultSize,
      const wxString& name = wxFileDialogNameStr );

    virtual ~ManagedFileDialog();
    virtual int ShowModal();

    bool IsTuidRequired() const;
    void SetTuidRequired( bool isRequired = true );

    tuid GetFileID() const;
    const S_tuid& GetFileIDs() const;

    void SetRequestedFileID( const tuid& request );

    static bool ValidateFinderSpec( const std::string& path, std::string& error );
    static bool ValidateFileID( const std::string& path, tuid idToTry, tuid& fileID, std::string& error );
  };
}
