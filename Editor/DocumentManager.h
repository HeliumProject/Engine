#pragma once

#include "Editor/API.h"
#include "Editor/Document.h"

#include "Foundation/TUID.h"
#include "Foundation/Message.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"

namespace Helium
{
    namespace Editor
    {
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
        public:
            DocumentManager( MessageSignature::Delegate displayMessage );

            const OS_DocumentSmartPtr& GetDocuments()
            {
                return m_Documents;
            }

            bool Contains( const tstring& path ) const;

            bool ValidateDocument( Document* document, tstring& error ) const;

            virtual DocumentPtr OpenPath( const tstring& path, tstring& error );
            virtual bool Save( DocumentPtr document, tstring& error );
            bool SaveAll( tstring& error );

            bool CloseAll();
            bool CloseDocument( DocumentPtr document, bool prompt = true );
            bool CloseDocuments( OS_DocumentSmartPtr documents );

            bool QueryCheckOut( Document* document ) const;
            bool IsCheckedOut( Document* document ) const;
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
            void DocumentClosed( const DocumentChangedArgs& args );

            OS_DocumentSmartPtr         m_Documents;
            MessageSignature::Delegate  m_Message;
        };
    }
}