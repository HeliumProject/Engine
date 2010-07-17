#pragma once

#include "Luna/API.h"
#include "Frame.h"
#include "PropertiesManager.h"

#include "Foundation/TUID.h"
#include "Application/Undo/Command.h"

namespace Luna
{
    // Forwards
    class Editor;

    // Typedef for a create function for an Editor
    typedef Editor* (*CreateEditorFunc)();

    // Enumeration of all the different editors.  Add new ones here.
    namespace EditorTypes
    {
        enum EditorType
        {
            Invalid = -1,
            Scene,
            Asset,
        };

        static void EditorTypeEnumerateEnumeration( Reflect::Enumeration* info )
        {
            info->AddElement(Scene, TXT( "Scene" ) );
            info->AddElement(Asset, TXT( "Asset" ) );
        }
    }
    typedef EditorTypes::EditorType EditorType;

    /////////////////////////////////////////////////////////////////////////////
    // Class managing all the information about an editor, including what function
    // to use to create the editor.
    // 
    class LUNA_EDITOR_API EditorInfo : public Nocturnal::RefCountBase< EditorInfo >
    {
    private:
        EditorType m_Type;
        CreateEditorFunc m_CreateFunc;
        tstring m_FileFilter;

    public:
        EditorInfo( EditorType type, CreateEditorFunc createFunc, const tstring& filter )
            : m_Type( type )
            , m_CreateFunc( createFunc )
            , m_FileFilter( filter )
        {
        }

        Editor* Create() const
        {
            NOC_ASSERT( m_CreateFunc );
            return m_CreateFunc();
        }

        EditorTypes::EditorType GetType() const
        {
            return m_Type;
        }

        const tstring& GetFileFilter() const
        {
            return m_FileFilter;
        }
    };

    typedef Nocturnal::SmartPtr< EditorInfo > EditorInfoPtr;

    // Map of editor info keyed by type
    typedef std::map< EditorType, EditorInfoPtr > M_EditorInfo;

    class DocumentManager;

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

        virtual DocumentManager& GetDocumentManager() = 0;

        virtual const tstring& GetPreferencePrefix() const NOC_OVERRIDE;

        void RevisionHistory( const tstring& path );

    protected:
        void OnPropertiesCreated( const PropertiesCreatedArgs& args );

        DECLARE_EVENT_TABLE();
    };

    typedef std::vector< Editor* > V_EditorDumbPtr;
}
