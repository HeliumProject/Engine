#pragma once

#include "Platform/Types.h"

namespace Luna
{
  // Function pointer for a file creation callback.
  typedef bool (*CreateFileCallback)( const tstring& path, tstring& error );

  // Forwards
  class PromptNewExistingPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog that prompts a user to either create a new file or use an existing
  // one.  This dialog is generic and can be configured for many different file
  // types.  If the user chooses to create a new file, the callback function that
  // is specified in the constructor will be called.
  // 
  class PromptNewExistingDlg : public wxDialog
  {
  private:
    PromptNewExistingPanel* m_Panel;
    CreateFileCallback m_CreateFileCallback;
    tstring m_FileFilter;
    
  public:
    PromptNewExistingDlg( wxWindow* parent, CreateFileCallback callback, const tstring& title, const tstring& desc, const tstring& createLabel, const tstring& existingLabel );
    virtual ~PromptNewExistingDlg();
    virtual int ShowModal();
    void SetFileFilter( const tstring& filter );
    void SetNewFile( const tstring& file );
    void SetExistingFile( const tstring& file );

    tstring GetFilePath() const;

  private:
    void UpdateEnableState();
    void OnRadioButtonSelected( wxCommandEvent& args );
    void OnButtonExistingClicked( wxCommandEvent& args );
    void OnButtonExistingFinderClicked( wxCommandEvent& args );
  };
}
