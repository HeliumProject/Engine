#pragma once

#include "Foundation/TUID.h"
#include "Foundation/Message.h"
#include "Foundation/FileDialog.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Document/Document.h"

namespace Helium
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
    class FOUNDATION_API DocumentManager
    {
    public:
        DocumentManager( MessageSignature::Delegate displayMessage, FileDialogSignature::Delegate fileDialog );

        const OS_DocumentSmartPtr& GetDocuments()
        {
            return m_Documents;
        }

        bool                OpenDocument( const DocumentPtr& document, tstring& error );
        Document*           FindDocument( const Helium::Path& path ) const;

        bool                SaveAll( tstring& error );
        bool                SaveDocument( DocumentPtr document, tstring& error );

        bool                CloseAll();
        bool                CloseDocuments( OS_DocumentSmartPtr documents );
        bool                CloseDocument( DocumentPtr document, bool prompt = true );

        bool                QueryAllowChanges( Document* document ) const;
        bool                AllowChanges( Document* document ) const;

        bool                QueryCheckOut( Document* document ) const;
        bool                CheckOut( Document* document ) const;

        bool                QueryOpen( Document* document ) const;
        bool                QueryAdd( Document* document ) const;

        SaveAction          QueryCloseAll( Document* document ) const;
        SaveAction          QueryClose( Document* document ) const;
        SaveAction          QuerySave( Document* document ) const;

    public:
        mutable DocumentEventSignature::Event e_DocumentAdded;
        mutable DocumentEventSignature::Event e_DocumentRemoved;

    private:
        bool AddDocument( const DocumentPtr& document );
        bool RemoveDocument( const DocumentPtr& document );
        void OnDocumentClosed( const DocumentEventArgs& args );

        OS_DocumentSmartPtr m_Documents;
        MessageSignature::Delegate m_Message;
        FileDialogSignature::Delegate m_FileDialog;

    };
}