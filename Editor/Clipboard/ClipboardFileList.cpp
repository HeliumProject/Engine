#include "EditorPch.h"
#include "ClipboardFileList.h"
#include "Foundation/Insert.h"

REFLECT_DEFINE_OBJECT( Helium::Editor::ClipboardFileList );

using namespace Helium;
using namespace Helium::Editor;

void ClipboardFileList::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ClipboardFileList::m_Files, TXT( "m_Files" ) );
    comp.AddField( &ClipboardFileList::m_IsDirty, TXT( "m_IsDirty" ), Reflect::FieldFlags::Discard );
}

ClipboardFileList::ClipboardFileList()
: m_IsDirty( false )
{
}

ClipboardFileList::~ClipboardFileList()
{
}

///////////////////////////////////////////////////////////////////////////////
// Add a file path to the list managed by this class.
// 
bool ClipboardFileList::AddFilePath( const tstring& file )
{
    Helium::StdInsert< std::set< tstring > >::Result inserted = m_Files.insert( file );
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
    if ( !source->IsClass( Reflect::GetClass< ClipboardFileList >() ) )
    {
        return false;
    }

    //ClipboardFileList* src = Reflect::AssertCast< ClipboardFileList >( source );

    // Finish this function
    HELIUM_BREAK();
    return false;
}