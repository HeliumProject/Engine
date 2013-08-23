#include "EditorPch.h"
#include "ClipboardElementArray.h"

HELIUM_DEFINE_CLASS( Helium::Editor::ClipboardElementArray );

using namespace Helium;
using namespace Helium::Editor;

void ClipboardElementArray::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ClipboardElementArray::m_CommonBaseClass, TXT( "m_CommonBaseClass" ) );
    comp.AddField( &ClipboardElementArray::m_Elements, TXT( "m_Elements" ) );
}

ClipboardElementArray::ClipboardElementArray()
{
    // By default, all items added to this array should derive from Reflect::Object
    bool converted = Helium::ConvertString( Reflect::GetMetaClass< Reflect::Object >()->m_Name, m_CommonBaseClass );
    HELIUM_ASSERT( converted );
}

ClipboardElementArray::~ClipboardElementArray()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the type ID of the base class that all elements in this collection
// must derive from.
// 
const Reflect::MetaClass* ClipboardElementArray::GetCommonBaseClass() const
{
    return Reflect::Registry::GetInstance()->GetMetaClass( m_CommonBaseClass.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the base class that all elements in this collection must derive from.
// 
void ClipboardElementArray::SetCommonBaseClass( const Reflect::MetaClass* type )
{
    bool converted = Helium::ConvertString( type->m_Name, m_CommonBaseClass );
    HELIUM_ASSERT( converted );
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to add the specified item to this array.  Returns false if the item
// is not allowed to be added.
// 
bool ClipboardElementArray::Add( const Reflect::ObjectPtr& item )
{
    if ( CanAdd( item ) )
    {
        m_Elements.push_back( item );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Attempts to add all the specified source to this collection.  This is an all
// or nothing operation; if one item fails to be added to this collection, none
// of the items will be added to the collection.
// 
bool ClipboardElementArray::Merge( const ReflectClipboardData* source )
{
    const ClipboardElementArray* collection = Reflect::SafeCast< ClipboardElementArray >( source );
    if ( !collection )
    {
        return false;
    }

    size_t originalSize = m_Elements.size();
    for each ( const Reflect::ObjectPtr& item in collection->m_Elements )
    {
        if ( !Add( item ) )
        {
            m_Elements.resize( originalSize );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified item can be added to the array.  This is 
// determined by checking to make sure that the item derived from the common
// base class that was specified earlier.
// 
bool ClipboardElementArray::CanAdd( const Reflect::ObjectPtr& item ) const
{
    if ( !item.ReferencesObject() )
    {
        return false;
    }

    return item->IsA( GetCommonBaseClass() );
}