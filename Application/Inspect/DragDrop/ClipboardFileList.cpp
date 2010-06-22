#include "ClipboardFileList.h"
#include "Foundation/Container/Insert.h"

using namespace Inspect;

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
  Reflect::RegisterClass<ClipboardFileList>( "ClipboardFileList" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ClipboardFileList::CleanupType()
{
  Reflect::UnregisterClass<ClipboardFileList>();
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
bool ClipboardFileList::AddFilePath( const std::string& file )
{
  std::string clean = file;
  Nocturnal::Path::Normalize( clean );
  Nocturnal::Insert< std::set< std::string > >::Result inserted = m_Files.insert( clean );
  m_IsDirty = inserted.second;
  return m_IsDirty;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of file paths managed by this class.
// 
const std::set< std::string >& ClipboardFileList::GetFilePaths() const
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
  NOC_BREAK();
  return false;
}