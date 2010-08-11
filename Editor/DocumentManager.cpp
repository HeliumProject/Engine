#include "Precompile.h"
#include "Editor/DocumentManager.h"

#include "Application/RCS/RCS.h"
#include "Application/UI/FileDialog.h"

#include "Editor/UI/YesNoAllDlg.h"

#include <algorithm>
#include <cctype>
#include <sstream>

using namespace Helium;
using namespace Helium::Editor;

DocumentManager::DocumentManager( MessageSignature::Delegate message )
: m_Message( message )
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the first document found with the specified path.
// 
Document* DocumentManager::FindDocument( const Helium::Path& path ) const
{
    OS_DocumentSmartPtr::Iterator docItr = m_Documents.Begin();
    OS_DocumentSmartPtr::Iterator docEnd = m_Documents.End();
    for ( ; docItr != docEnd; ++docItr )
    {
        Document* document = *docItr;
        if ( document->GetFilePath() == path )
        {
            return document;
        }
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if it successfully opens a document with the specified path.
// 
bool DocumentManager::OpenDocument( const DocumentPtr& document, tstring& error )
{
    if ( !document->GetFilePath().empty() )
    {
        if ( FindDocument( document->GetFilePath() ) )
        {
            error = TXT( "The specified file (" ) + document->GetFilePath() + TXT( ") is already open." );
            return false;
        }

        if ( !IsUpToDate( document ) )
        {
            error = TXT( "The version of '" ) + document->GetFilePath() + TXT( "' on your computer is out of date.  You will not be able to check it out." );
            return false;
        }
    }

    AddDocument( document );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the documents, calling save on each one.
// 
bool DocumentManager::SaveAll( tstring& error )
{
    bool savedAll = true;
    bool prompt = true;
    OS_DocumentSmartPtr::Iterator docItr = m_Documents.Begin();
    OS_DocumentSmartPtr::Iterator docEnd = m_Documents.End();
    for ( ; docItr != docEnd; ++docItr )
    {
        Document* document = *docItr;

        bool abort = false;
        bool save = true;
        if ( prompt )
        {
            switch ( QuerySave( document ) )
            {
            case SaveActions::SaveAll:
                save = true;
                prompt = false;
                break;

            case SaveActions::Save:
                save = true;
                prompt = true;
                break;

            case SaveActions::Skip:
                save = false;
                prompt = true;
                break;

            case SaveActions::SkipAll:
                save = false;
            case SaveActions::Abort:
            default:
                abort = true;
                break; 
            }
        }

        if ( abort )
        {
            break;
        }

        if ( save )
        {
            tstring msg;
            if ( !SaveDocument( document, msg ) )
            {
                savedAll = false;
                if ( !error.empty() )
                {
                    error += TXT( "\n" );
                }
                error += msg;
            }
        }
    }

    return savedAll;
}

///////////////////////////////////////////////////////////////////////////////
// Prompt the user to save a file to a new location.  Returns the path to the
// new file location, or an empty string if the user cancels the operation.
// 
#pragma TODO("This need to be externalized -Geoff")
static tstring PromptSaveAs( const DocumentPtr& file, wxWindow* window = NULL )
{
    tstring path;
    tstring defaultDir = Helium::Path( file->GetFilePath() ).Directory();
    tstring defaultFile = file->GetFilePath();

    Helium::FileDialog saveDlg( window, TXT( "Save As..." ), defaultDir.c_str(), defaultFile.c_str(), TXT( "" ), Helium::FileDialogStyles::DefaultSave );
    
    std::set< tstring > extensions;
    Reflect::Archive::GetExtensions( extensions );
    for ( std::set< tstring >::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
    {
        saveDlg.AddFilter( TXT( "Scene (*.scene." ) + *itr + TXT( ")|*.scene." ) + *itr );
    }

    if ( saveDlg.ShowModal() == wxID_OK )
    {
        path = saveDlg.GetPath();
    }

    return path;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified document and returns true if successful.
//
// Derived classes should HELIUM_OVERRIDE this function to actually perform saving 
// data to disk as appropriate.  The base implementation fires the appropriate
// events.  A derived class may want to call this implementation if the save
// is successful.
//
bool DocumentManager::SaveDocument( DocumentPtr document, tstring& error )
{
    // Check for "save as"
    if ( document->GetFilePath().empty() )
    {
#ifdef UI_REFACTOR
        tstring savePath = PromptSaveAs( document, m_Editor );
#else
        tstring savePath = PromptSaveAs( document );
#endif
        if ( !savePath.empty() )
        {
            document->SetFilePath( savePath );
        }
        else
        {
            // No error, operation cancelled
            return true;
        }
    }

    if ( document->Save( error ) )
    {
        document->SetModified( false );
        return true;
    }

    if ( error.empty() )
    {
       error = TXT( "Failed to save " ) + document->GetFilePath();
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Closes all currently open documents.
// 
bool DocumentManager::CloseAll()
{
    return CloseDocuments( m_Documents );
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the documents, closing each one.  The user will be prompted
// to save any modified documents before closing them.  Returns true if all 
// documents were successfully closed.
// 
bool DocumentManager::CloseDocuments( OS_DocumentSmartPtr documents )
{
    // NOTE: We purposefully make a copy of the files to iterate over
    // because the original list may be changing as we close files,
    // which will invalidate our iterator.

    if ( documents.Empty() )
    {
        return true;
    }

    bool prompt = true;
    bool save = true;
    bool abort = false;

    OS_DocumentSmartPtr::ReverseIterator docItr = documents.ReverseBegin();
    OS_DocumentSmartPtr::ReverseIterator docEnd = documents.ReverseEnd();
    for ( ; docItr != docEnd && !abort; ++docItr )
    {
        const DocumentPtr& document = *docItr;

        if ( prompt )
        {
            SaveActions::SaveAction action = SaveActions::Abort;
            if ( documents.Size() == 1 )
            {
                action = QueryClose( document );
            }
            else
            {
                action = QueryCloseAll( document );
            }

            switch ( action )
            {
            case SaveActions::SaveAll:
                prompt = false;
                save = true;
                break;

            case SaveActions::Save:
                prompt = true;
                save = true;
                break;

            case SaveActions::SkipAll:
                prompt = false;
                save = false;
                break;

            case SaveActions::Skip:
                prompt = true;
                save = false;
                break;

            case SaveActions::Abort:
                abort = true;
                break;
            }
        }

        if ( abort )
        {
            return false;
        }

        if ( save )
        {
            if ( RCS::PathIsManaged( document->GetFilePath() ) )
            {
                if ( !CheckOut( document ) )
                {
                    return false;
                }
            }

            tstring error;
            if ( !SaveDocument( document, error ) )
            {
                error += TXT( "\nAborting operation." );
                m_Message.Invoke( MessageArgs( TXT( "Error" ), error, MessagePriorities::Error, MessageAppearances::Ok ) );
                return false;
            }
        }

        if ( !CloseDocument( document, false ) )
        {
            tstring error;
            error = TXT( "Failed to close '" ) + document->GetFileName() + TXT( "'.  Aborting operation." );
            m_Message.Invoke( MessageArgs( TXT( "Error" ), error, MessagePriorities::Error, MessageAppearances::Ok ) );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Closes the specified file.  Determines if the file needs to be saved, and
// handles any other revision control interactions.  Notifies any interested 
// listeners if the file is successfully closed.  If prompt is set to true, 
// the user will be prompted to save their file before it is closed.
// 
// Fires an event to notify listeners that this document is now closed.  Any
// objects that are holding pointers to this document should release them.
//
bool DocumentManager::CloseDocument( DocumentPtr document, bool prompt )
{
    HELIUM_ASSERT( document.ReferencesObject() );

    bool shouldClose = !prompt;
    bool wasClosed = false;

    if ( prompt )
    {
        tstring unused;
        switch ( QueryClose( document ) )
        {
        case SaveActions::Save:
            shouldClose = SaveDocument( document, unused );
            break;

        case SaveActions::Skip:
            shouldClose = true;
            break;

        case SaveActions::Abort:
            shouldClose = false;
            break;
        }
    }

    if ( shouldClose )
    {
        document->RaiseClosed();
        wasClosed = true;
    }

    return wasClosed;
}

///////////////////////////////////////////////////////////////////////////////
// Call this function if the user fails to checkout a file, or attempts to edit
// a file without checking it out.  The user is prompted as to whether they want
// to edit the file (even though they won't be able to save).  Returns true if the
// user wants to allow changes, false otherwise.
// 
bool DocumentManager::QueryAllowChanges( Document* document ) const
{
    if ( !document->AllowChanges() && !IsCheckedOut( document ) )
    {
        QueryCheckOut( document );
        if ( !IsCheckedOut( document ) )
        {
            if ( MessageResults::Yes == m_Message.Invoke( MessageArgs( TXT( "Edit anyway?" ), TXT( "Would you like to edit this file anyway?\n(NOTE: You may not be able to save your changes)" ), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
            {
                document->SetAllowChanges( true );
            }
        }
    }

    return document->AllowChanges() || IsCheckedOut( document );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this file is allowed to be changed.  The file is allowed to 
// be changed if the user has it checked out, or if they chose to edit the file
// anyway when their attempt to check out the file failed.  See QueryAllowChanges.
// 
bool DocumentManager::AllowChanges( Document* document ) const
{
    if ( IsCheckedOut( document ) || document->AllowChanges() )
    {
        return true;
    }

    return QueryAllowChanges( document );
}

///////////////////////////////////////////////////////////////////////////////
// Asks the user if they want to check out the file.  Returns true if the user
// opts not to check out the file.  Also returns true if the user chooses to 
// check out the file, and it is successfully checked out.  Use this function
// when opening a file to see if it should be opened for edit.
// 
bool DocumentManager::QueryCheckOut( Document* document ) const
{
    if ( !IsUpToDate( document ) )
    {
        tostringstream str;
        str << "The version of " << document->GetFileName() << " on your computer is out of date.  You will not be able to check it out.";
        m_Message.Invoke( MessageArgs( TXT( "Warning" ), str.str(), MessagePriorities::Warning, MessageAppearances::Ok ) );
    }
    else
    {
        if ( !IsCheckedOut( document ) )
        {
            tostringstream str;
            str << "Do you wish to check out " << document->GetFileName() << "?";
            if ( MessageResults::Yes == m_Message.Invoke( MessageArgs( TXT( "Check Out?" ), str.str(), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
            {
                return CheckOut( document );
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Checks a file out from revision control.  Returns false if unable to check
// out the file.
// 
bool DocumentManager::CheckOut( Document* document ) const
{
    if ( !RCS::PathIsManaged( document->GetFilePath() ) )
    {
        return true;
    }

    RCS::File rcsFile( document->GetFilePath() );

    try
    {
        rcsFile.GetInfo();
    }
    catch ( Helium::Exception& ex )
    {
        tstringstream str;
        str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
        m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
        return false;
    }

    if ( rcsFile.ExistsInDepot() && !rcsFile.IsUpToDate() )
    {
        tostringstream str;
        str << "The version of " << document->GetFileName() << " on your computer is out of date.  You will not be able to check it out.";
        m_Message.Invoke( MessageArgs( TXT( "Warning" ), str.str(), MessagePriorities::Warning, MessageAppearances::Ok ) );
        return false;
    }

    if ( rcsFile.IsCheckedOutByMe() )
    {
        return true;
    }
    else if ( rcsFile.IsCheckedOutBySomeoneElse() )
    {
        tstring usernames;
        rcsFile.GetOpenedByUsers( usernames );

        tostringstream str;
        str << "Unable to check out " << document->GetFileName() << ", it's currently checked out by " << usernames << ".";
        m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
        return false;
    }

    try
    {
        rcsFile.Open();
    }
    catch ( Helium::Exception& ex )
    {
        tstringstream str;
        str << "Unable to open '" << document->GetFilePath() << "': " << ex.What();
        m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
        return false;
    }

    document->RaiseCheckedOut();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a flag indicating whether or not to continue the open process 
// (not the checkout status).
// 
bool DocumentManager::QueryOpen( Document* document ) const
{
    if ( RCS::PathIsManaged( document->GetFilePath() ) )
    {
        RCS::File rcsFile( document->GetFilePath() );
        try
        {
            rcsFile.GetInfo();
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
            m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
        }

        // Is the file already managed?
        if ( rcsFile.ExistsInDepot() )
        {
            if ( rcsFile.IsCheckedOut() && !rcsFile.IsCheckedOutByMe() )
            {
                tstring usernames;
                rcsFile.GetOpenedByUsers( usernames );

                tostringstream str;
                tstring capitalized = document->GetFileName();
                capitalized[0] = toupper( capitalized[0] );
                str << capitalized << " is already checked out by \"" << usernames << "\"\nDo you still wish to open the file?";
                if ( MessageResults::Yes == m_Message.Invoke( MessageArgs( TXT( "Checked Out by Another User" ), str.str(), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return QueryCheckOut( document );
            }
        }
        // Should the file be added to a pending changelist?
        else
        {
            if ( !QueryAdd( document ) )
            {
                tostringstream str;
                str << "Unable to add " << document->GetFileName() << " to revision control.  Would you like to continue opening the file?";
                if ( MessageResults::Yes == m_Message.Invoke( MessageArgs( TXT( "Continue Opening?" ), str.str(), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
    }

    // File is not in source control, and shouldn't be (perhaps it's on the user's desktop
    // or something, so just let it be opened).
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Checks to see if this file should be in revision control.  If the file should
// be in revision control, but isn't, the user is prompted to add the file.
// Returns false if the user attempts to add a file, but it fails for some reason.
// 
bool DocumentManager::QueryAdd( Document* document ) const
{
    bool isOk = true;
    if ( RCS::PathIsManaged( document->GetFilePath() ) )
    {
        // Is the file already managed?
        RCS::File rcsFile( document->GetFilePath() );
        rcsFile.GetInfo();

        if ( !rcsFile.ExistsInDepot() )
        {
            tostringstream msg;
            msg << "Would you like to add \"" << document->GetFileName() << "\" to revision control?";
            if ( MessageResults::Yes == m_Message.Invoke( MessageArgs( TXT( "Add to Revision Control?" ), msg.str(), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
            {
                try
                {
                    rcsFile.Open();
                }
                catch ( Helium::Exception& ex )
                {
                    tstringstream str;
                    str << "Unable to open '" << document->GetFilePath() << "': " << ex.What();
                    m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
                    isOk = false;
                }
            }
        }
    }
    return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Opens a dialog prompting the user whether they want to save the current file
// or not.  The user has the option to specifiy that they want the same action
// taken for all subsequent files.
// 
SaveAction DocumentManager::QueryCloseAll( Document* document ) const
{
    if ( document->IsModified() )
    {
        bool attemptCheckOut = false;
        SaveActions::SaveAction action = SaveActions::Save;

        tostringstream msg;
        msg << "You are attempting to close file " << document->GetFileName() << " which has changed. Would you like to save your changes before closing?";       
        switch ( m_Message.Invoke( MessageArgs( TXT( "Save Changes?" ), msg.str(), MessagePriorities::Question, MessageAppearances::YesNoCancelToAll ) ) )
        {
        case wxID_YES:
            action = SaveActions::Save;
            attemptCheckOut = true;
            break;

        case wxID_YESTOALL:
            action = SaveActions::SaveAll;
            attemptCheckOut = true;
            break;

        case wxID_NO:
            action = SaveActions::Skip;
            break;

        case wxID_NOTOALL:
            action = SaveActions::SkipAll;
            break;

        case wxID_CANCEL:
            action = SaveActions::Abort;
            break;
        }

        if ( attemptCheckOut )
        {
            if ( RCS::PathIsManaged( document->GetFilePath() ) )
            {
                if ( !CheckOut( document ) )
                {
                    action = SaveActions::Abort;
                }
            }
        }

        return action;
    }

    return QueryClose( document );
}

///////////////////////////////////////////////////////////////////////////////
// Prompts for whether or not to save the file before closing.
// 
SaveAction DocumentManager::QueryClose( Document* document ) const
{
    if ( !document->IsModified() )
    {
        return SaveActions::Skip;
    }

    if ( IsCheckedOut( document ) || !RCS::PathIsManaged( document->GetFilePath() ) )
    {
        tstring msg( TXT( "Would you like to save changes to " ) );
        msg += TXT( "'" ) + document->GetFileName() + TXT( "' before closing?" );
        switch (  m_Message.Invoke( MessageArgs( TXT( "Save Changes?" ), msg.c_str(), MessagePriorities::Question, MessageAppearances::YesNoCancel ) ) )
        {
        case MessageResults::Yes:
            return SaveActions::Save;
            break;

        case MessageResults::No:
            return SaveActions::Skip;
            break;

        case MessageResults::Cancel:
        default:
            return SaveActions::Abort;
        }
    }

    return QuerySave( document );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a value indicating whether the save operation should continue or not
// (not the checkout status).
// 
SaveAction DocumentManager::QuerySave( Document* document ) const
{
    if ( !document->IsModified() )
    {
        return SaveActions::Skip;
    }

    if ( !RCS::PathIsManaged( document->GetFilePath() ) )
    {
        return SaveActions::Save;
    }

    if ( !IsCheckedOut( document ) )
    {
        if ( document->IsModified() )
        {
            tstring msg;

            if ( !IsUpToDate( document ) )
            {
                msg = TXT( "Unfortunately, the file '" ) + document->GetFileName() + TXT( "' has been modified in revsion control since you opened it.\n\nYou cannot save the changes you have made.\n\nTo fix this:\n1) Close the file\n2) Get updated assets\n3) Make your changes again\n\nSorry for the inconvenience." );
                m_Message.Invoke( MessageArgs( TXT( "Cannot save" ), msg, MessagePriorities::Error, MessageAppearances::Ok ) );
                return SaveActions::Skip;
            }

            msg = TXT( "File '" ) + document->GetFileName() + TXT( "' has been changed, but is not checked out.  Would you like to check out and save this file?" );
            if ( MessageResults::No == m_Message.Invoke( MessageArgs( TXT( "Check out and save?" ), msg.c_str(), MessagePriorities::Question, MessageAppearances::YesNo ) ) )
            {
                return SaveActions::Skip;
            }
        }

        if ( !CheckOut( document ) )
        {
            return SaveActions::Abort;
        }
    }

    // File was already checked out, or was successfully checked out
    return SaveActions::Save;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the file is currently checked out by this user.
// 
bool DocumentManager::IsCheckedOut( Document* document ) const
{
    if ( !document->GetFilePath().empty() && RCS::PathIsManaged( document->GetFilePath() ) )
    {
        RCS::File rcsFile( document->GetFilePath() );

        try
        {
            rcsFile.GetInfo();
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
            m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
        }

        return rcsFile.IsCheckedOutByMe();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Checks to see if the local revision of this file is the same as the head
// revision, and returns true if so.
// 
bool DocumentManager::IsUpToDate( Document* document ) const
{
    if ( !document->GetFilePath().empty() )
    {
        if ( RCS::PathIsManaged( document->GetFilePath() ) )
        {
            RCS::File rcsFile( document->GetFilePath() );

            try
            {
                rcsFile.GetInfo();
            }
            catch ( Helium::Exception& ex )
            {
                tstringstream str;
                str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
                m_Message.Invoke( MessageArgs( TXT( "Error" ), str.str(), MessagePriorities::Error, MessageAppearances::Ok ) );
            }

            if ( rcsFile.ExistsInDepot() )
            {
                return rcsFile.IsUpToDate() && ( rcsFile.m_LocalRevision == document->GetRevision() );
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a document to the list managed by this object.  Returns false if the 
// document was already in the list belonging to this manager.
// 
bool DocumentManager::AddDocument( const DocumentPtr& document )
{
    if ( m_Documents.Append( document ) )
    {
        document->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &DocumentManager::DocumentClosed ) );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the document from the list managed by this object.
// NOTE: The document may be deleted if no one is holding a smart pointer to it
// after this function is called.
// 
bool DocumentManager::RemoveDocument( const DocumentPtr& document )
{
    document->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &DocumentManager::DocumentClosed ) );
    if ( m_Documents.Remove( document ) )
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document is closed.  Removes the document from the list
// managed by this class.
// 
void DocumentManager::DocumentClosed( const DocumentChangedArgs& args )
{
    RemoveDocument( args.m_Document );
}
