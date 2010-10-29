#pragma once

#include "Foundation/API.h"
#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    class Document;

    ///////////////////////////////////////////////////////////////////////////
    // Arguments for file save, open, close, etc...
    class DocumentEventArgs
    {
    public:
        const Document* m_Document;
        bool m_Veto;
        tstring* m_Error;
        bool m_Result;

        DocumentEventArgs( const Document* document, tstring* error = NULL )
            : m_Document( document )
            , m_Veto( false )
            , m_Error( error )
            , m_Result( true )
        {
        }
    };
    typedef Helium::Signature< const DocumentEventArgs& > DocumentEventSignature;


    ///////////////////////////////////////////////////////////////////////////
    // Arguments for a file being renamed (contains the new and old names)
    class DocumentPathChangedArgs : public DocumentEventArgs
    {
    public:
        const Helium::Path& m_OldPath;

        DocumentPathChangedArgs( Document* document, const Helium::Path& oldPath )
            : DocumentEventArgs( document )
            , m_OldPath( oldPath )
        {
        }
    };

    // Event delegate for functions that take DocumentEventArgs
    typedef Helium::Signature< const DocumentPathChangedArgs& > DocumentPathChangedSignature;


    ///////////////////////////////////////////////////////////////////////////
    // Wraps all files manipulated by editors in Editor.  Handles all interaction
    // with revision control.
    // 
    class FOUNDATION_API Document : public Helium::RefCountBase< Document >
    {
    public:
        Document( const tstring& path );
        virtual ~Document();

        //
        // API
        //
        bool Save( tstring& error ) const;
        void Close() const;

        bool HasChanged() const
        {
            return m_HasChanged;
        }
        void HasChanged( bool changed );

        const Helium::Path& GetPath() const
        {
            return m_Path;
        }
        void SetPath( const Helium::Path& path );

        int GetRevision() const
        {
            return m_Revision;
        }

        bool AllowUnsavableChanges() const;
        void AllowUnsavableChanges( bool allowUnsavableChanges );

        //void RaiseCheckedOut() const
        //{
        //    m_CheckedOut.Raise( DocumentEventArgs( this ) );
        //}

        //
        // Events
        //
    public:
        // Save
        mutable DocumentEventSignature::Event e_Saving;
        mutable DocumentEventSignature::Delegate d_Save;
        mutable DocumentEventSignature::Event e_Saved;

        // Close
        mutable DocumentEventSignature::Event e_Closing;
        mutable DocumentEventSignature::Delegate d_Close; 
        mutable DocumentEventSignature::Event e_Closed; 

        // Change
        mutable DocumentEventSignature::Event e_Changing; 
        mutable DocumentEventSignature::Event e_Changed; 

        // Modified On Disk
        mutable DocumentPathChangedSignature::Event e_ModifiedOnDiskStateChanged;

        // Path
        mutable DocumentPathChangedSignature::Event e_PathChanged;

        // Revision Control
        mutable DocumentEventSignature::Event e_CheckedOut;

    private:
        mutable bool m_HasChanged;  //<! have we been changed since we opened or last saved?
        Helium::Path m_Path;
        int32_t                 m_Revision;
        
        bool m_AllowUnsavableChanges;        //<! allows override of checkout (but you can't save)

        void UpdateRCSFileInfo();
    };
    typedef Helium::SmartPtr< Document > DocumentPtr;
}