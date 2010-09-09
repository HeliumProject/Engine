#include "Precompile.h"
#include "ClipboardFileList.h"
#include "Foundation/Container/Insert.h"

using namespace Helium;
using namespace Helium::Editor;

// Definition
REFLECT_DEFINE_CLASS( ClipboardFileList );

void ClipboardFileList::EnumerateClass( Reflect::Compositor<ClipboardFileList>& comp )
{
  Reflect::Field* fieldFiles = comp.AddField( &ClipboardFileList::m_Files, "m_Files" );
  Reflect::Field* fieldIsDirty = comp.AddField( &ClipboardFileList::m_IsDirty, "m_IsDirty", Reflect::FieldFlags::Discard );
}


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ClipboardFileList::InitializeType()
{
  Reflect::RegisterClassType<ClipboardFileList>( TXT( "ClipboardFileList" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ClipboardFileList::CleanupType()
{
  Reflect::UnregisterClassType<ClipboardFileList>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ClipboardFileList::ClipboardFileList()
: m_IsDirty( false )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ClipboardFileList::~ClipboardFileList()
{
}

///////////////////////////////////////////////////////////////////////////////
// Add a file path to the list managed by this class.
// 
bool ClipboardFileList::AddFilePath( const tstring& file )
{
  tstring clean = file;
  Helium::Path::Normalize( clean );
  Helium::Insert< std::set< tstring > >::Result inserted = m_Files.insert( clean );
  m_IsDirty = inserted.second;
  return m_IsDirty;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of file paths managed by this class.
// 
const std::set< tstring >& ClipboardFileList::GetFilePaths() const
{
  return m_Files;
}

///////////////////////////////////////////////////////////////////////////////
// Combines the source data into this class if possible.
// 
bool ClipboardFileList::Merge( const ReflectClipboardData* source )
{
  if ( !source->HasType( Reflect::GetType< ClipboardFileList >() ) )
  {
    return false;
  }

  //ClipboardFileList* src = Reflect::AssertCast< ClipboardFileList >( source );

  // Finish this function
  HELIUM_BREAK();
  return false;
}