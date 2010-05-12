#pragma once

#include "API.h"
#include "EditorInfo.h"
#include "Inspect/InspectInit.h"
#include "Common/Automation/Event.h"

namespace Luna
{
  // Forwards
  class Editor;
  struct PropertiesPanelEditCommandArgs;
  struct EditFilePathArgs;

  struct ViewerControlChangeArgs
  {
    Editor* m_Editor;

    ViewerControlChangeArgs( Editor* editor )
    : m_Editor( editor )
    {
    }
  };
  typedef Nocturnal::Signature< void, const ViewerControlChangeArgs& > ViewerControlChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Singleton class that keeps track of all the Luna Editors and the files that
  // they have open.  The session manager can save and load session files that
  // store data about all of the state of each editor.
  // 
  class LUNA_EDITOR_API SessionManager 
  {
    typedef std::map< EditorType, Editor* > M_EditorDumbPtr;

  private:
    M_EditorInfo m_RegisteredEditors;
    M_EditorDumbPtr m_RunningEditors;
    std::string m_Path;
    bool m_UseTracker;

  private:
    SessionManager();
  public:
    virtual ~SessionManager();

    // singleton instance
    static SessionManager* GetInstance();

    // persistence of session
    void LoadSession( const std::string& file );
    void SaveSession( const std::string& file, const EditorType saveEditorType = EditorTypes::Invalid );
    const std::string& GetSessionPath() const;

    void RegisterEditor( const EditorInfoPtr& info );
    void RegisterEditorInstance( Editor* editor );
    void UnregisterEditorInstance( Editor* editor );
    Editor* LaunchEditor( EditorTypes::EditorType whichEditor );
    u32 GetRunningEditorCount() const;

    bool SaveAllOpenDocuments();
    bool SaveAllOpenDocuments( Editor* currentEditor, bool& showPrompts );

    // Launch a Luna Editor or an external editor to open a file
    void Edit( const std::string& file );
    void Edit( const tuid& fileID );

    bool CheckOut( const std::string& file, bool prompt = false );

    // Viewer-related functions
    void GiveViewerControl( Editor* editor );

    bool UseTracker() const { return m_UseTracker; }
    void UseTracker( bool useTracker ) { m_UseTracker = useTracker; }

  private:
    EditorTypes::EditorType FindEditorForFile( const std::string& path );

    // 
    // Listeners
    // 
  private:
    ViewerControlChangeSignature::Event m_ViewerControl;
  public:
    void AddViewerControlChangedListener( const ViewerControlChangeSignature::Delegate& listener )
    {
      m_ViewerControl.Add( listener );
    }

    void RemoveViewerControlChangedListener( const ViewerControlChangeSignature::Delegate& listener )
    {
      m_ViewerControl.Remove( listener );
    }

  private:
    void PropertiesPanelEdit( const Inspect::EditFilePathArgs& args );
  };
}
