#pragma once

#include "Luna/API.h"
#include "Core/Object.h"
#include "Foundation/File/Path.h"

namespace Luna
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
    typedef Nocturnal::Signature< void, const DocumentChangedArgs& > DocumentChangedSignature;

    // Arguments for a file being renamed (contains the new and old names)
    struct DocumentPathChangedArgs : public DocumentChangedArgs
    {
        std::string m_OldFilePath;
        std::string m_OldFileName;

        DocumentPathChangedArgs( Document* document, const std::string& oldFilePath, const std::string& oldFileName )
            : DocumentChangedArgs( document )
            , m_OldFilePath( oldFilePath )
            , m_OldFileName( oldFileName )
        {
        }
    };

    // Event delegate for functions that take DocumentChangedArgs
    typedef Nocturnal::Signature< void, const DocumentPathChangedArgs& > DocumentPathChangedSignature;


    /////////////////////////////////////////////////////////////////////////////
    // Wraps all files manipulated by editors in Luna.  Handles all interaction
    // with revision control.
    // 
    class LUNA_EDITOR_API Document : public Object
    {
        //
        // Member variables
        //

    private:
        Nocturnal::Path  m_Path;
        std::string         m_Name;          // the friendly name (for dialogs)
        bool                m_IsModified;    // have we been changed since we saved?
        bool                m_AllowChanges;  // allows override of checkout (but you can't save)
        i32                 m_Revision;

        // 
        // RTTI
        // 

        LUNA_DECLARE_TYPE( Document, Object );
        static void InitializeType();
        static void CleanupType();


        // 
        // Member functions
        // 

    public:
        Document( const std::string& path, const std::string& name = std::string( "" ) );
        virtual ~Document();

        //
        // Access
        //

        const Nocturnal::Path& GetPath() const;
        std::string GetFilePath() const
        {
            return m_Path.Get();
        }
        void SetFilePath( const std::string& newFilePath, const std::string& newName = "" );

        const std::string& GetFileName() const;

        u64 GetHash() const;
        int GetRevision() const;

        bool GetIsWriteable() const;
        void SetIsWriteable( bool writeable );

        virtual bool AllowChanges() const;
        void SetAllowChanges( bool allowChanges );

        bool IsModified() const;
        void SetModified( bool modified );

    private:
        void UpdateFileInfo();

        // 
        // Listeners
        // 

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
    };
    typedef Nocturnal::SmartPtr< Document > DocumentPtr;
}
