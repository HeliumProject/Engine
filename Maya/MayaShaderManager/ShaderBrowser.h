#pragma once

#include "MayaShaderManager.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"


#include "FileBrowser/FileBrowser.h"

namespace Maya
{
  class ShaderBrowser;
  typedef Nocturnal::SmartPtr< ShaderBrowser > ShaderBrowserPtr; 

  class ShaderBrowser : public File::FileBrowser, public Nocturnal::RefCountBase< ShaderBrowser >
  {
  public:
    ShaderBrowser(
      wxWindow* parent = NULL,
      int id = wxID_ANY, 
      wxString title = "Shader Browser", 
      wxPoint pos = wxDefaultPosition, 
      wxSize size = wxSize( 800, 600 ), 
      int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    void Create( 
      wxWindow* parent,
      int id = wxID_ANY, 
      wxString title = wxEmptyString, 
      wxPoint pos = wxDefaultPosition, 
      wxSize size = wxSize( 800, 600 ), 
      int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    virtual ~ShaderBrowser();

    ShaderType GetShaderType() { return m_ShaderType; }
    void       SetShaderType( const ShaderType shaderType ) { m_ShaderType = shaderType; }

  protected:
    DECLARE_EVENT_TABLE()

    //
    // Event processor functions
    //

    void OnCancel( wxCommandEvent& evt );
    void OnClose( wxCloseEvent& args );
    void OnOK( wxCommandEvent& evt );
    void OnItemActivated( wxListEvent& evt );

    virtual void EnableOperationButton( bool enable = true );
    virtual bool ValidatePath( const std::string& path, std::string& error );

  protected:  
    void Init();

    wxButton*   m_buttonCreateLLambertShader;
    wxButton*   m_buttonCreateLBlinnShader;

  private:
    ShaderType  m_ShaderType;
    S_tuid      m_ShaderFileIDs;

  private:
    //
    // Dialog API
    //

    // show the dialog modally and return the value passed to EndModal()
    virtual int ShowModal() { return wxID_CANCEL; }

    // may be called to terminate the dialog with the given return code
    virtual void EndModal( int retCode = wxID_CANCEL ) {}
  };

} // namespace Maya