#include "Precompile.h"
#include "Document.h"

#include "Common/Assert.h"
#include "RCS/RCS.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

using namespace Luna;

// 
// RTTI
// 
LUNA_DEFINE_TYPE( Document );

void Document::InitializeType()
{
  Reflect::RegisterClass<Document>( "Document" );
}

void Document::CleanupType()
{
  Reflect::UnregisterClass<Document>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Document::Document( const std::string& path, const std::string& name )
: m_Path( path )
, m_Name( name )
, m_IsModified( false )
, m_AllowChanges( false )
, m_Revision( -1 )
{
  UpdateFileInfo();
}

void Document::UpdateFileInfo()
{
  m_Revision = -1;

  if ( !m_Path.empty() )
  {
    if ( RCS::PathIsManaged( m_Path ) )
    {
      RCS::File rcsFile( m_Path );

      try
      {
        rcsFile.GetInfo();
      }
      catch ( Nocturnal::Exception& ex )
      {
        std::stringstream str;
        str << "Unable to get info for '" << m_Path << "': " << ex.what();
        wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
      }

      m_Revision = rcsFile.m_LocalRevision;
    }

    if ( m_Name.empty() )
    {
      m_Name = FileSystem::GetLeaf( m_Path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Document::~Document()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full path for the file.
// 
const std::string& Document::GetFilePath() const
{
  return m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path to this file.  The name of the file is also updated.  Notifies
// any interested listeners about this event.
// 
void Document::SetFilePath( const std::string& newFilePath, const std::string& newName )
{
  if ( m_Path != newFilePath)
  {
    std::string oldFilePath = m_Path;
    std::string oldFileName = m_Name;

    m_Path = newFilePath; 
    m_Name = newName;

    UpdateFileInfo();

    m_PathChanged.Raise( DocumentPathChangedArgs( this, oldFilePath, oldFileName ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the File ID for this document, if there is one.
// 
tuid Document::GetFileID() const
{
  // This could be cached for faster access
  tuid fileID = TUID::Null;

  if ( !m_Path.empty() )
  {
    try
    {
      fileID = File::GlobalManager().GetID( m_Path );
    }
    catch ( const Nocturnal::Exception& )
    {
      // Do nothing, not every document has to have a TUID
    }
  }
  return fileID;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the friendly name of the file.
// 
const std::string& Document::GetFileName() const
{
  return m_Name;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the revision number of the file when it was instanciated
// 
int Document::GetRevision() const
{
  return m_Revision;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the user has specified that they want to make changes to
// this file even if it is not checked out by them.
// 
bool Document::AllowChanges() const
{
  return m_AllowChanges;
}

///////////////////////////////////////////////////////////////////////////////
// Sets whether to allow changes regardless of file check out state.
// 
void Document::SetAllowChanges( bool allowChanges )
{
  m_AllowChanges = allowChanges;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the file has been modified.  Note, this independent of 
// whether or not the file is checked out.
// 
bool Document::IsModified() const
{
  return m_IsModified;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the internal flag indicating the the file has been modified (thus it
// should probably be saved before closing).
// 
void Document::SetModified( bool modified )
{
  if ( m_IsModified != modified )
  {
    m_IsModified = modified;

    m_Modified.Raise( DocumentChangedArgs( this ) );
  }
}
