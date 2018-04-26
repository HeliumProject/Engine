#pragma once

#include "Foundation/TUID.h"
#include "Foundation/Event.h"
#include "Foundation/FilePath.h"
#include "Foundation/SmartPtr.h"
#include "Application/OrderedSet.h"

#include "Application/API.h"
#include "Application/Message.h"
#include "Application/FileDialog.h"

namespace Helium
{
    class Document;

    ///////////////////////////////////////////////////////////////////////////
    // Arguments for file save, open, close, etc...
    class DocumentObjectChangedArgs
    {
    public:
        const bool m_HasChanged;

        DocumentObjectChangedArgs( const bool hasChanged )
            : m_HasChanged( hasChanged )
        {
        }
    };
    typedef Helium::Signature< const DocumentObjectChangedArgs& > DocumentObjectChangedSignature;

    ///////////////////////////////////////////////////////////////////////////
    // Arguments for file save, open, close, etc...
    class DocumentEventArgs
    {
    public:
        const Document* m_Document;
        mutable bool m_Veto;
        std::string* m_Error;
        mutable bool m_Result;

        DocumentEventArgs( const Document* document, std::string* error = NULL )
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
        const Helium::FilePath& m_OldPath;

        DocumentPathChangedArgs( Document* document, const Helium::FilePath& oldPath )
            : DocumentEventArgs( document )
            , m_OldPath( oldPath )
        {
        }
    };

    // Event delegate for functions that take DocumentEventArgs
    typedef Helium::Signature< const DocumentPathChangedArgs& > DocumentPathChangedSignature;


    ///////////////////////////////////////////////////////////////////////////
    namespace DocumentStatus
    {
        enum Status
        {
            Changed = 1 << 0,
            Loading = 1 << 1,
            Saving  = 1 << 2,
        };
        const uint32_t Default = 0;
    }   


    ///////////////////////////////////////////////////////////////////////////
    // Wraps all files manipulated by editors in Editor.  Handles all interaction
    // with revision control.
    // 
    class HELIUM_APPLICATION_API Document : public Helium::RefCountBase< Document >
    {
    public:
        Document( const std::string& path );
        virtual ~Document();

        //
        // API
        //
        bool Save( std::string& error );
        void Close();

        void Checkout() const;

        bool HasChanged() const;
        void HasChanged( bool changed );
        void OnObjectChanged( const DocumentObjectChangedArgs& args );

        const Helium::FilePath& GetPath() const
        {
            return m_Path;
        }
        void SetPath( const Helium::FilePath& path );

        uint32_t GetStatus() const;

        int GetRevision() const
        {
            return m_Revision;
        }

        bool IsCheckedOut() const;
        bool IsUpToDate() const;

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
        mutable DocumentEventSignature::Event e_ModifiedOnDiskStateChanged;

        // FilePath
        mutable DocumentPathChangedSignature::Event e_PathChanged;

        // Revision Control
        mutable DocumentEventSignature::Event e_CheckedOut;

    private:
        Helium::FilePath m_Path;
        uint32_t m_DocumentStatus;
        bool m_AllowUnsavableChanges;        //<! allows override of checkout (but you can't save)
        int32_t m_Revision;

        void UpdateRCSFileInfo();
    };
    typedef Helium::SmartPtr< Document > DocumentPtr;

    // MetaEnum of actions that can be requested during a save operation
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
    class HELIUM_APPLICATION_API DocumentManager
    {
    public:
        DocumentManager( MessageSignature::Delegate displayMessage, FileDialogSignature::Delegate fileDialog );

        const OS_DocumentSmartPtr& GetDocuments()
        {
            return m_Documents;
        }

        bool                OpenDocument( const DocumentPtr& document, std::string& error );
        Document*           FindDocument( const Helium::FilePath& path ) const;

        bool                SaveAll( std::string& error );
        bool                SaveDocument( DocumentPtr document, std::string& error );

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
        mutable DocumentEventSignature::Event e_DocumentOpened;
        mutable DocumentEventSignature::Event e_DocumenClosed;

    private:
        bool AddDocument( const DocumentPtr& document );
        bool RemoveDocument( const DocumentPtr& document );
        void OnDocumentClosed( const DocumentEventArgs& args );

        OS_DocumentSmartPtr m_Documents;
        MessageSignature::Delegate m_Message;
        FileDialogSignature::Delegate m_FileDialog;
    };
}