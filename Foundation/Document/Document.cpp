#include "Document.h"

#include "Platform/Assert.h"
#include "Foundation/Log.h"
#include "Foundation/RCS/RCS.h"

using namespace Helium;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Document::Document( const tstring& path )
: m_Path( path )
, m_HasChanged( false )
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
bool Document::Save( tstring& error ) const
{
    DocumentEventArgs savingArgs( this );
    e_Saving.Raise( savingArgs );

    if ( savingArgs.m_Veto )
    {
        return false;
    }

    DocumentEventArgs saveArgs( this, &error );
    d_Save.Invoke( saveArgs );
    if ( saveArgs.m_Result )
    {
        e_Saved.Raise( DocumentEventArgs( this ) );
    }

    return saveArgs.m_Result;
}

///////////////////////////////////////////////////////////////////////////////
void Document::Close() const
{
    e_Closing.Raise( DocumentEventArgs( this ) );

    d_Close.Invoke( DocumentEventArgs( this ) );

    e_Closed.Raise( DocumentEventArgs( this ) );
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
// Sets the internal flag indicating the the file has been modified (thus it
// should probably be saved before closing).
// 
void Document::HasChanged( bool changed )
{
    if ( m_HasChanged != changed )
    {
        m_HasChanged = changed;

        e_Changed.Raise( DocumentEventArgs( this ) );
    }
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