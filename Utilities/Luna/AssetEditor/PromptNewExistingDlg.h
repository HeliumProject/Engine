#pragma once

#include "Finder/Finder.h"

#include "File/File.h"

namespace Luna
{
  // Function pointer for a file creation callback.
  typedef bool (*CreateFileCallback)( const std::string& path, std::string& error );

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
    const Finder::FinderSpec* m_FinderSpec;
    
  public:
    PromptNewExistingDlg( wxWindow* parent, CreateFileCallback callback, const std::string& title, const std::string& desc, const std::string& createLabel, const std::string& existingLabel );
    virtual ~PromptNewExistingDlg();
    virtual int ShowModal() NOC_OVERRIDE;
    void SetFinderSpec( const Finder::FinderSpec* spec );
    void SetNewFile( const std::string& file );
    void SetExistingFile( const std::string& file );

    std::string GetFilePath() const;

  private:
    void UpdateEnableState();
    void OnRadioButtonSelected( wxCommandEvent& args );
    void OnButtonExistingClicked( wxCommandEvent& args );
    void OnButtonExistingFinderClicked( wxCommandEvent& args );
  };
}
