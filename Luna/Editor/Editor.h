#pragma once

#include "Luna/API.h"
#include "Core/PropertiesManager.h"
#include "Frame.h"
#include "EditorInfo.h"
#include "EditorState.h"
#include "Foundation/TUID.h"
#include "Application/Undo/Command.h"

namespace Luna
{
  class DocumentManager;
  class Editor;

  class PropertiesCreatedCommand : public Undo::Command
  {
  public:
    PropertiesCreatedCommand( PropertiesManager* propertiesManager, u32 selectionId, const Inspect::V_Control& controls ) 
    : m_PropertiesManager( propertiesManager )
    , m_SelectionId( selectionId )
    , m_Controls( controls )
    { 
    }

    virtual void Undo() NOC_OVERRIDE
    {
      // this should never happen
      NOC_BREAK();
    }

    virtual void Redo() NOC_OVERRIDE
    {
      m_PropertiesManager->FinalizeProperties( m_SelectionId, m_Controls );
    }

  private:
     PropertiesManager* m_PropertiesManager;
     u32 m_SelectionId;
     Inspect::V_Control m_Controls;
  };

  /////////////////////////////////////////////////////////////////////////////
  // Base class for different editors in Luna.  Think of each editor as a top
  // level window.
  // 
  class LUNA_EDITOR_API Editor NOC_ABSTRACT : public Frame
  {
  private:
    EditorType  m_EditorType;

    mutable tstring m_PreferencePrefix;

  public:
    Editor( EditorType editorType, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT( "frame" ) );
    virtual ~Editor();

    EditorType GetEditorType() const;

    virtual DocumentManager* GetDocumentManager() = 0;

    virtual const tstring& GetPreferencePrefix() const NOC_OVERRIDE;

    void RevisionHistory( const tstring& path );

  protected:
    void OnPropertiesCreated( const PropertiesCreatedArgs& args );

    DECLARE_EVENT_TABLE();
  };

  typedef std::vector< Editor* > V_EditorDumbPtr;
}
