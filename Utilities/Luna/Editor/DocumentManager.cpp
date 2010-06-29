#include "Precompile.h"
#include "DocumentManager.h"

#include "YesNoAllDlg.h"

#include "Application/RCS/RCS.h"

#include <algorithm>
#include <cctype>
#include <sstream>

// Using
using namespace Luna;

DocumentChangeSignature::Event DocumentManager::s_DocumentChange;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
DocumentManager::DocumentManager( wxWindow* parentWindow )
: m_ParentWindow( parentWindow )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
DocumentManager::~DocumentManager()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds a document to the list managed by this object.  Returns false if the 
// document was already in the list belonging to this manager.
// 
bool DocumentManager::AddDocument( const DocumentPtr& document )
{
    if ( m_Documents.Append( document ) )
    {
        s_DocumentChange.Raise( DocumentManagerChangeArgs( this, document ) );
        document->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &DocumentManager::OnDocumentClosed ) );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the first document found with the specified path.
// 
Document* DocumentManager::FindDocument( const tstring& path ) const
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
// Removes the document from the list managed by this object.
// NOTE: The document may be deleted if no one is holding a smart pointer to it
// after this function is called.
// 
bool DocumentManager::RemoveDocument( const DocumentPtr& document )
{
    document->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &DocumentManager::OnDocumentClosed ) );
    if ( m_Documents.Remove( document ) )
    {
        s_DocumentChange.Raise( DocumentManagerChangeArgs( this, document ) );
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of documents managed by this class (in the order they were
// opened).
// 
const OS_DocumentSmartPtr& DocumentManager::GetDocuments()
{
    return m_Documents;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this class contains a document with the specified path.
// 
bool DocumentManager::Contains( const tstring& path ) const
{
    return FindDocument( path ) != NULL;
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
            catch ( Nocturnal::Exception& ex )
            {
                tstringstream str;
                str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
                wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
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
// 
// 
bool DocumentManager::ValidateDocument( Document* document, tstring& error ) const
{
    if ( !IsUpToDate( document ) )
    {
        error = TXT( "The version of '" ) + document->GetFileName() + TXT( "' on your computer is out of date.  You will not be able to check it out." );
        return false;
    }

    if ( !ValidatePath( document->GetFilePath(), error ) )
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
bool DocumentManager::ValidatePath( const tstring& path, tstring& error ) const
{
    // TODO: support add duplicates
    if ( Contains( path ) )
    {
        error = TXT( "The specified file (" ) + path + TXT( ") is already open." );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if it successfully opens a document with the specified path.
// 
DocumentPtr DocumentManager::OpenPath( const tstring& path, tstring& error )
{
    DocumentPtr document = new Document( path );

    if ( !AddDocument( document ) )
    {
        document = NULL;
    }

    return document;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified document and returns true if successful.
//
// Derived classes should NOC_OVERRIDE this function to actually perform saving 
// data to disk as appropriate.  The base implementation fires the appropriate
// events.  A derived class may want to call this implementation if the save
// is successful.
//
bool DocumentManager::Save( DocumentPtr document, tstring& error )
{

#pragma TODO( "UMMMMM, shouldn't this actually save?" )
    document->SetModified( false );
    document->RaiseSaved();

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
            if ( !Save( document, msg ) )
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
// Closes all currently open documents.
// 
bool DocumentManager::CloseAll()
{
    return CloseDocuments( m_Documents );
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
    NOC_ASSERT( document.ReferencesObject() );

    bool shouldClose = !prompt;
    bool wasClosed = false;

    if ( prompt )
    {
        tstring unused;
        switch ( QueryClose( document ) )
        {
        case SaveActions::Save:
            shouldClose = Save( document, unused );
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
            if ( !Save( document, error ) )
            {
                error += TXT( "\nAborting operation." );
                wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
                return false;
            }
        }

        if ( !CloseDocument( document, false ) )
        {
            tstring error;
            error = TXT( "Failed to close '" ) + document->GetFileName() + TXT( "'.  Aborting operation." );
            wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
            return false;
        }
    }

    return true;
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
        catch ( Nocturnal::Exception& ex )
        {
            tstringstream str;
            str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
            wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
        }

        return rcsFile.IsCheckedOutByMe();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this file is allowed to be changed.  The file is allowed to 
// be changed if the user has it checked out, or if they chose to edit the file
// anyway when their attempt to check out the file failed.  See QueryAllowChanges.
// 
bool DocumentManager::AttemptChanges( Document* document ) const
{
    if ( IsCheckedOut( document ) || document->AllowChanges() )
    {
        return true;
    }

    return QueryAllowChanges( document );
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
    catch ( Nocturnal::Exception& ex )
    {
        tstringstream str;
        str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
        wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
        return false;
    }

    if ( rcsFile.ExistsInDepot() && !rcsFile.IsUpToDate() )
    {
        tostringstream str;
        str << "The version of " << document->GetFileName() << " on your computer is out of date.  You will not be able to check it out.";
        wxMessageBox( str.str().c_str(), wxT( "Warning" ), wxOK | wxCENTER | wxICON_WARNING, m_ParentWindow );
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
        wxMessageBox( str.str().c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR, m_ParentWindow );
        return false;
    }

    try
    {
        rcsFile.Open();
    }
    catch ( Nocturnal::Exception& ex )
    {
        tstringstream str;
        str << "Unable to open '" << document->GetFilePath() << "': " << ex.What();
        wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
        return false;
    }

    document->RaiseCheckedOut();

    return true;
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
            if ( wxMessageBox( wxT( "Would you like to edit this file anyway?\n(NOTE: You may not be able to save your changes)" ), wxT( "Edit anyway?" ), wxCENTER | wxYES_NO | wxICON_QUESTION, m_ParentWindow ) == wxYES )
            {
                document->SetAllowChanges( true );
            }
        }
    }
    return document->AllowChanges() || IsCheckedOut( document );
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
            if ( wxYES == wxMessageBox( msg.str().c_str(), wxT( "Add to Revision Control?" ), wxYES_NO | wxCENTER | wxICON_QUESTION, m_ParentWindow ) )
            {
                try
                {
                    rcsFile.Open();
                }
                catch ( Nocturnal::Exception& ex )
                {
                    tstringstream str;
                    str << "Unable to open '" << document->GetFilePath() << "': " << ex.What();
                    wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
                    isOk = false;
                }
            }
        }
    }
    return isOk;
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
        catch ( Nocturnal::Exception& ex )
        {
            tstringstream str;
            str << "Unable to get info for '" << document->GetFilePath() << "': " << ex.What();
            wxMessageBox( str.str().c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_ParentWindow );
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
                if ( wxYES == wxMessageBox( str.str().c_str(), wxT( "Checked Out by Another User" ), wxYES_NO | wxCENTER | wxICON_QUESTION, m_ParentWindow ) )
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
                if ( wxYES == wxMessageBox( str.str().c_str(), wxT( "Continue Opening?" ), wxYES_NO | wxCENTER | wxICON_QUESTION, m_ParentWindow ) )
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
        wxMessageBox( str.str().c_str(), wxT( "Warning" ), wxOK | wxCENTER | wxICON_WARNING, m_ParentWindow );
    }
    else
    {
        if ( !IsCheckedOut( document ) )
        {
            tostringstream str;
            str << "Do you wish to check out " << document->GetFileName() << "?";
            if ( wxYES == wxMessageBox( str.str().c_str(), wxT( "Check Out?" ), wxYES_NO | wxCENTER | wxICON_QUESTION, m_ParentWindow ) )
            {
                return CheckOut( document );
            }
        }
    }

    return true;
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
                wxMessageBox( msg.c_str(), wxT( "Cannot save" ), wxOK | wxCENTER | wxICON_ERROR, m_ParentWindow );
                return SaveActions::Skip;
            }

            msg = TXT( "File '" ) + document->GetFileName() + TXT( "' has been changed, but is not checked out.  Would you like to check out and save this file?" );
            if ( wxNO == wxMessageBox( msg.c_str(), wxT( "Check out and save?" ), wxYES_NO | wxCENTER | wxICON_QUESTION, m_ParentWindow ) )
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
        const int result = wxMessageBox( msg.c_str(), wxT( "Save Changes?" ), wxYES_NO | wxCANCEL | wxCENTER | wxICON_QUESTION, m_ParentWindow );
        switch ( result )
        {
        case wxYES:
            return SaveActions::Save;
            break;

        case wxNO:
            return SaveActions::Skip;
            break;

        case wxCANCEL:
        default:
            return SaveActions::Abort;
        }
    }

    return QuerySave( document );
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
        tostringstream msg;
        msg << "You are attempting to close file " << document->GetFileName() << " which has changed. Would you like to save your changes before closing?";
        YesNoAllDlg dlg( m_ParentWindow, wxT( "Save Changes?" ), msg.str().c_str() );
        dlg.SetButtonToolTip( wxID_YES, wxT( "Save and close this file" ) );
        dlg.SetButtonToolTip( wxID_YESTOALL, wxT( "Save and close all files" ) );
        dlg.SetButtonToolTip( wxID_NO, wxT( "Do not save this file (it will still be closed)" ) );
        dlg.SetButtonToolTip( wxID_NOTOALL, wxT( "Do not save any files that have changed (they will still be closed)" ) );
        dlg.SetButtonToolTip( wxID_CANCEL, wxT( "No files will be saved or closed" ) );

        bool attemptCheckOut = false;
        SaveActions::SaveAction action = SaveActions::Save;
        switch ( dlg.ShowModal() )
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
// Callback for when a document is closed.  Removes the document from the list
// managed by this class.
// 
void DocumentManager::OnDocumentClosed( const DocumentChangedArgs& args )
{
    RemoveDocument( args.m_Document );
}
