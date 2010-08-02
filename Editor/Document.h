#pragma once

#include "Editor/API.h"
#include "Object.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Editor
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
        typedef Helium::Signature< void, const DocumentChangedArgs& > DocumentChangedSignature;

        // Arguments for a file being renamed (contains the new and old names)
        struct DocumentPathChangedArgs : public DocumentChangedArgs
        {
            tstring m_OldFilePath;
            tstring m_OldFileName;

            DocumentPathChangedArgs( Document* document, const tstring& oldFilePath, const tstring& oldFileName )
                : DocumentChangedArgs( document )
                , m_OldFilePath( oldFilePath )
                , m_OldFileName( oldFileName )
            {
            }
        };

        // Event delegate for functions that take DocumentChangedArgs
        typedef Helium::Signature< void, const DocumentPathChangedArgs& > DocumentPathChangedSignature;


        /////////////////////////////////////////////////////////////////////////////
        // Wraps all files manipulated by editors in Editor.  Handles all interaction
        // with revision control.
        // 
        class EDITOR_EDITOR_API Document : public Reflect::AbstractInheritor< Editor::Document, Editor::Object >
        {
            //
            // Member variables
            //

        private:
            Helium::Path        m_Path;
            tstring             m_Name;          // the friendly name (for dialogs)
            bool                m_IsModified;    // have we been changed since we saved?
            bool                m_AllowChanges;  // allows override of checkout (but you can't save)
            i32                 m_Revision;

            // 
            // RTTI
            // 
        public:
            static void InitializeType();
            static void CleanupType();


            // 
            // Member functions
            // 

        public:
            Document( const tstring& path, const tstring& name = TXT( "" ) );
            virtual ~Document();

            //
            // Access
            //

            const Helium::Path& GetPath() const;
            tstring GetFilePath() const
            {
                return m_Path.Get();
            }
            void SetFilePath( const tstring& newFilePath, const tstring& newName = TXT( "" ) );

            const tstring& GetFileName() const;

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
        typedef Helium::SmartPtr< Document > DocumentPtr;
    }
}