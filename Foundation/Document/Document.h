#pragma once

#include "Foundation/API.h"
#include "Foundation/File/Path.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    class Document;
    class Frame;

    // Arguments for file open, close, etc.
    struct DocumentChangedArgs
    {
        const Document* m_Document;

        DocumentChangedArgs( const Document* document )
            : m_Document( document )
        {
        }
    };
    // Event delegate for functions that take DocumentChangedArgs
    typedef Helium::Signature< const DocumentChangedArgs& > DocumentChangedSignature;

    // Arguments for a file being renamed (contains the new and old names)
    struct DocumentPathChangedArgs : public DocumentChangedArgs
    {
        const Helium::Path& m_OldFilePath;
        const tstring&      m_OldFileName;

        DocumentPathChangedArgs( Document* document, const Helium::Path& oldFilePath, const tstring& oldFileName )
            : DocumentChangedArgs( document )
            , m_OldFilePath( oldFilePath )
            , m_OldFileName( oldFileName )
        {
        }
    };

    // Event delegate for functions that take DocumentChangedArgs
    typedef Helium::Signature< const DocumentPathChangedArgs& > DocumentPathChangedSignature;

    /////////////////////////////////////////////////////////////////////////////
    // Wraps all files manipulated by editors in Editor.  Handles all interaction
    // with revision control.
    // 
    class FOUNDATION_API Document : public Helium::RefCountBase< Document >
    {
    public:
        Document( const tstring& path, const tstring& name = TXT( "" ) );
        virtual ~Document();

        const Helium::Path& GetPath() const
        {
            return m_Path;
        }
        void SetPath( const Helium::Path& path, const tstring& newName = TXT( "" ) );

        const tstring& GetFilePath() const
        {
            return m_Path.Get();
        }
        void SetFilePath( const tstring& newFilePath, const tstring& newName = TXT( "" ) )
        {
            SetPath( newFilePath, newName );
        }

        const tstring& GetFileName() const;

        u64 GetHash() const;
        int GetRevision() const;

        virtual bool AllowChanges() const;
        void SetAllowChanges( bool allowChanges );

        bool IsModified() const;
        void SetModified( bool modified );

        virtual bool Save( tstring& error ) = 0;

    private:
        void UpdateFileInfo();

    private:
        mutable DocumentPathChangedSignature::Event m_PathChanged; // event for file path being changed
    public:
        void AddDocumentPathChangedListener( const DocumentPathChangedSignature::Delegate& listener ) const
        {
            m_PathChanged.Add( listener );
        }
        void RemoveDocumentPathChangedListener( const DocumentPathChangedSignature::Delegate& listener ) const
        {
            m_PathChanged.Remove( listener );
        }

    private:
        mutable DocumentChangedSignature::Event m_Modified;        // event for m_IsModified being set to true
    public:
        void AddDocumentModifiedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Modified.Add( listener );
        }
        void RemoveDocumentModifiedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Modified.Remove( listener );
        }

    private:
        mutable DocumentChangedSignature::Event m_CheckedOut; // Event for when a file is checked out
    public:
        void RaiseCheckedOut() const
        {
            m_CheckedOut.Raise( DocumentChangedArgs( this ) );
        }
        void AddDocumentCheckedOutListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_CheckedOut.Add( listener );
        }
        void RemoveDocumentCheckedOutListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_CheckedOut.Remove( listener );
        }

    private:
        mutable DocumentChangedSignature::Event m_Saved; // Event for when a file is saved
    public:
        void RaiseSaved() const 
        {
            m_Saved.Raise( DocumentChangedArgs( this ) );
        }
        void AddDocumentSavedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Saved.Add( listener );
        }
        void RemoveDocumentSavedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Saved.Remove( listener );
        }

    private:
        mutable DocumentChangedSignature::Event m_Closed; // Event for when a file is closed
    public:
        void RaiseClosed() const
        {
            m_Closed.Raise( DocumentChangedArgs( this ) );
        }
        void AddDocumentClosedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Closed.Add( listener );
        }
        void RemoveDocumentClosedListener( const DocumentChangedSignature::Delegate& listener ) const
        {
            m_Closed.Remove( listener );
        }

    private:
        Helium::Path        m_Path;
        tstring             m_Name;          // the friendly name (for dialogs)
        bool                m_IsModified;    // have we been changed since we saved?
        bool                m_AllowChanges;  // allows override of checkout (but you can't save)
        i32                 m_Revision;
    };
    typedef Helium::SmartPtr< Document > DocumentPtr;
}