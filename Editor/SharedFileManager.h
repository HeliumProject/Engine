#pragma once

#include "Editor/API.h"

#include "Foundation/Automation/Event.h"
#include "Foundation/File/Path.h"

#include <map>

namespace Editor
{
    /////////////////////////////////////////////////////////////////////////////
    // Enumeration of different events that can occur to a shared file.
    // 
    namespace FileActions
    {
        enum FileAction
        {
            Saved,
            // TODO:
            //Opened,
            //Closed,
        };
    }


    /////////////////////////////////////////////////////////////////////////////
    // Defines the arguments that will be passed to listeners when an event is
    // raised on a shared file resource.
    // 
    struct LUNA_EDITOR_API SharedFileChangeArgs
    {
        Helium::Path m_Path;
        FileActions::FileAction m_Action;

        SharedFileChangeArgs( const Helium::Path& path, FileActions::FileAction action )
            : m_Path( path )
            , m_Action( action )
        {
        }
    };

    typedef Helium::Signature< void, const SharedFileChangeArgs& > SharedFileChangedSignature;
    typedef std::map< u64, SharedFileChangedSignature::Event > M_SharedFileChangedEvent;


    /////////////////////////////////////////////////////////////////////////////
    // This is a singleton class that allows you to listen for events on any TUID-
    // based file in Editor.  The Editors will fire events when they make changes to
    // a file, and you can listen for any of these events.
    // 
    class LUNA_EDITOR_API SharedFileManager
    {
    private:
        M_SharedFileChangedEvent m_Events;

    private:
        SharedFileManager();

    public:
        virtual ~SharedFileManager();

        static SharedFileManager* GetInstance();

        void AddFileListener( const Helium::Path& path, const SharedFileChangedSignature::Delegate& listener );
        void RemoveFileListener( const Helium::Path& path, const SharedFileChangedSignature::Delegate& listener );
        void RemoveListenerFromAllFiles( const SharedFileChangedSignature::Delegate& listener );
        void FireEvent( const Helium::Path& path, FileActions::FileAction action, const SharedFileChangedSignature::Delegate& emitter = SharedFileChangedSignature::Delegate () );
    };
}
