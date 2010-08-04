#pragma once

#include "Editor/API.h"
#include "Document.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"
#include "Foundation/TUID.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class DocumentManager;

        struct DocumentManagerChangeArgs
        {
            DocumentManager*  m_Manager;
            Document*         m_Document;

            DocumentManagerChangeArgs (DocumentManager* manager, Document* document)
                : m_Manager( manager )
                , m_Document( document )
            {
            }
        };
        typedef Helium::Signature< void, const DocumentManagerChangeArgs& > DocumentChangeSignature;

        // Enumeration of actions that can be requested during a save operation
        namespace SaveActions
        {
            enum SaveAction
            {
                Save,     // Carry out the save 
                SaveAll,  // Save this file and all others without prompting
                Skip,     // Don't save this file (it's already saved)
                SkipAll,  // Skip this file and all others without prompting
                Abort     // Don't save this file (and don't close it either)
            };
        }
        typedef SaveActions::SaveAction SaveAction;

        typedef Helium::OrderedSet< DocumentPtr > OS_DocumentSmartPtr;

        /////////////////////////////////////////////////////////////////////////////
        // Manages a group of documents, providing convenience functions for saving,
        // closing, etc. all of them at once.
        // 
        class EDITOR_EDITOR_API DocumentManager
        {
        private:
            OS_DocumentSmartPtr m_Documents;
            wxWindow*           m_ParentWindow;

        public:
            DocumentManager( wxWindow* parentWindow );
            virtual ~DocumentManager();

            const OS_DocumentSmartPtr& GetDocuments();

            bool Contains( const tstring& path ) const;

            virtual bool ValidateDocument( Document* document, tstring& error ) const;
            virtual bool ValidatePath( const tstring& path, tstring& error ) const;

            virtual DocumentPtr OpenPath( const tstring& path, tstring& error );

            virtual bool Save( DocumentPtr document, tstring& error );
            virtual bool SaveAll( tstring& error );

            virtual bool CloseAll();
            virtual bool CloseDocument( DocumentPtr document, bool prompt = true );
            bool CloseDocuments( OS_DocumentSmartPtr documents );

            bool QueryCheckOut( Document* document ) const;
            virtual bool IsCheckedOut( Document* document ) const;
            bool CheckOut( Document* document ) const;

            bool QueryAllowChanges( Document* document ) const;
            bool AttemptChanges( Document* document ) const;

            bool IsUpToDate( Document* document ) const;

            bool QueryAdd( Document* document ) const;
            bool QueryOpen( Document* document ) const;
            SaveAction QuerySave( Document* document ) const;
            SaveAction QueryClose( Document* document ) const;
            SaveAction QueryCloseAll( Document* document ) const;

        protected:
            bool AddDocument( const DocumentPtr& document );
            bool RemoveDocument( const DocumentPtr& document );
            Document* FindDocument( const tstring& path ) const;

        private:
            void OnDocumentClosed( const DocumentChangedArgs& args );

            //
            // Events
            //

        private:
            static DocumentChangeSignature::Event s_DocumentChange;
        public:
            static void AddDocumentChangeListener( DocumentChangeSignature::Delegate& delegate )
            {
                s_DocumentChange.Add( delegate );
            }
            static void RemoveDocumentChangeListener( DocumentChangeSignature::Delegate& delegate )
            {
                s_DocumentChange.Remove( delegate );
            }
        };
    }
}