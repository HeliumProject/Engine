#include "FoundationPch.h"
#include "Document.h"

#include "Platform/Assert.h"
#include "Foundation/Flags.h"
#include "Foundation/Log.h"
#include "Foundation/RCS/RCS.h"

using namespace Helium;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Document::Document( const tstring& path )
: m_Path( path )
, m_DocumentStatus( DocumentStatus::Default )
, m_AllowUnsavableChanges( false )
, m_Revision( -1 )
{
    UpdateRCSFileInfo();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Document::~Document()
{
}

///////////////////////////////////////////////////////////////////////////////
bool Document::Save( tstring& error )
{
    SetFlag<uint32_t>( m_DocumentStatus, DocumentStatus::Saving, true );

    bool result = false;

    DocumentEventArgs savingArgs( this );
    e_Saving.Raise( savingArgs );
    if ( !savingArgs.m_Veto )
    {
        DocumentEventArgs saveArgs( this, &error );
        d_Save.Invoke( saveArgs );
        if ( saveArgs.m_Result )
        {
            SetFlag<uint32_t>( m_DocumentStatus, DocumentStatus::Saving, false );

            e_Saved.Raise( DocumentEventArgs( this ) );

            HasChanged( false );

            result = true;
        }
    }

    SetFlag<uint32_t>( m_DocumentStatus, DocumentStatus::Saving, false );

    return result;
}

///////////////////////////////////////////////////////////////////////////////
void Document::Close()
{
    e_Closing.Raise( DocumentEventArgs( this ) );

    d_Close.Invoke( DocumentEventArgs( this ) );

    e_Closed.Raise( DocumentEventArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
void Document::Checkout() const
{
    e_CheckedOut.Raise( DocumentEventArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path to this file.  The name of the file is also updated.  Notifies
// any interested listeners about this event.
// 
void Document::SetPath( const Helium::Path& newPath )
{
    Helium::Path oldPath = m_Path.Get();

    m_Path = newPath;
    UpdateRCSFileInfo();

    e_PathChanged.Raise( DocumentPathChangedArgs( this, oldPath ) );
}

///////////////////////////////////////////////////////////////////////////////
uint32_t Document::GetStatus() const
{
    return m_DocumentStatus;
}

///////////////////////////////////////////////////////////////////////////////
bool Document::HasChanged() const
{
    return HasFlags<uint32_t>( m_DocumentStatus, DocumentStatus::Changed );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the internal flag indicating the the file has been modified (thus it
// should probably be saved before closing).
// 
void Document::HasChanged( bool changed )
{
    if ( HasFlags<uint32_t>( m_DocumentStatus, DocumentStatus::Changed ) != changed )
    {
        SetFlag<uint32_t>( m_DocumentStatus, DocumentStatus::Changed, changed );

        e_Changed.Raise( DocumentEventArgs( this ) );
    }
}

void Document::OnObjectChanged( const DocumentObjectChangedArgs& args )
{
    HasChanged( args.m_HasChanged );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the file is currently checked out by this user.
// 
bool Document::IsCheckedOut() const
{
    if ( !GetPath().Filename().empty() && RCS::PathIsManaged( GetPath().Filename() ) )
    {
        RCS::File rcsFile( GetPath().Filename() );

        try
        {
            rcsFile.GetInfo();
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "Unable to get info for '" << GetPath().Filename() << "': " << ex.What();
            Log::Error( TXT("%s\n"), str.str().c_str() );
#pragma TODO( "Should trigger RCS error status event" )
        }

        return rcsFile.IsCheckedOutByMe();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Checks to see if the local revision of this file is the same as the head
// revision, and returns true if so.
// 
bool Document::IsUpToDate() const
{
    if ( !GetPath().Filename().empty() )
    {
        if ( RCS::PathIsManaged( GetPath().Filename() ) )
        {
            RCS::File rcsFile( GetPath().Filename() );

            try
            {
                rcsFile.GetInfo();
            }
            catch ( Helium::Exception& ex )
            {
                tstringstream str;
                str << "Unable to get info for '" << GetPath().Filename() << "': " << ex.What();
                Log::Error( TXT("%s\n"), str.str().c_str() );
#pragma TODO( "Should trigger RCS error status event" )
            }

            if ( rcsFile.ExistsInDepot() )
            {
                return rcsFile.IsUpToDate() && ( rcsFile.m_LocalRevision == GetRevision() );
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the user has specified that they want to make changes to
// this file even if it is not checked out by them.
// 
bool Document::AllowUnsavableChanges() const
{
    return m_AllowUnsavableChanges;
}

///////////////////////////////////////////////////////////////////////////////
// Sets whether to allow changes regardless of file check out state.
// 
void Document::AllowUnsavableChanges( bool allowUnsavableChanges )
{
    m_AllowUnsavableChanges = allowUnsavableChanges;
}

///////////////////////////////////////////////////////////////////////////////
void Document::UpdateRCSFileInfo()
{
    m_Revision = -1;

    if ( !m_Path.Get().empty() )
    {
        if ( RCS::PathIsManaged( m_Path.Get() ) )
        {
            RCS::File rcsFile( m_Path.Get() );

            try
            {
                rcsFile.GetInfo();
            }
            catch ( Helium::Exception& ex )
            {
                tstringstream str;
                str << TXT( "Unable to get info for '" ) << m_Path.Get() << TXT( "': " ) << ex.What();
                Log::Warning( TXT("%s\n"), str.str().c_str() );
            }

            m_Revision = rcsFile.m_LocalRevision;
        }
    }
}