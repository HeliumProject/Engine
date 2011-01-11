#include "Foundation/Reflect/Composite.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/ObjectCache.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/ArchiveBinary.h"

using namespace Helium;
using namespace Helium::Reflect;

Field::Field()
: m_Composite( NULL )
, m_Name( NULL )
, m_Flags( 0 )
, m_Index( -1 )
, m_Type( NULL )
, m_DataClass( NULL )
, m_Offset( -1 )
{

}

DataPtr Field::CreateData() const
{
    DataPtr data;

    if ( m_DataClass != NULL )
    {
        ObjectPtr object = Registry::GetInstance()->CreateInstance( m_DataClass );

        if (object.ReferencesObject())
        {
            data = AssertCast<Data>(object);
        }
    }

    return data;
}

DataPtr Field::CreateData(void* instance) const
{
    DataPtr data = CreateData();

    if ( data.ReferencesObject() )
    {
        if ( instance )
        {
            data->ConnectField( instance, this );
        }

        const Class* classType = ReflectionCast< Class >( m_Type );
        if ( classType )
        {
            PointerData* pointerData = ObjectCast<PointerData>( data );
            if ( pointerData )
            {
                pointerData->m_Type = m_Type;
            }
            else
            {
                ObjectContainerData* containerData = ObjectCast<ObjectContainerData>( data );
                if ( containerData )
                {
                    containerData->m_Type = m_Type;
                }
            }
        }
    }

    return data;
}

DataPtr Field::CreateData(const void* instance) const
{
    DataPtr data = CreateData();

    if ( data.ReferencesObject() )
    {
        if ( instance )
        {
            data->ConnectField( instance, this );
        }

        const Class* classType = ReflectionCast< Class >( m_Type );
        if ( classType )
        {
            PointerData* pointerData = ObjectCast<PointerData>( data );
            if ( pointerData )
            {
                pointerData->m_Type = m_Type;
            }
            else
            {
                ObjectContainerData* containerData = ObjectCast<ObjectContainerData>( data );
                if ( containerData )
                {
                    containerData->m_Type = m_Type;
                }
            }
        }
    }

    return data;
}

DataPtr Field::CreateTemplateData() const
{
    return CreateData( m_Composite->m_Template );
}

DataPtr Field::ShouldSerialize(const void* instance, ObjectCache* cache) const
{
    // never write discard fields
    if ( m_Flags & FieldFlags::Discard )
    {
        return NULL;
    }

    ObjectPtr object;

    if ( cache )
    {
        cache->Create( m_DataClass, object );
    }
    else
    {
        object = Registry::GetInstance()->CreateInstance( m_DataClass );
    }

    DataPtr data = TryCast< Data >( object );
    data->ConnectField( instance, this );

    // always write force fields
    if ( m_Flags & FieldFlags::Force )
    {
        return data;
    }

    // check for empty/null/invalid state
    if ( !data->ShouldSerialize() )
    {
        return NULL;
    }

    // don't write field at the default value
    DataPtr templateData = CreateTemplateData();
    if ( templateData.ReferencesObject() && templateData->Equals(data) )
    {
        return NULL;
    }

    return data;
}

Composite::Composite()
: m_Base( NULL )
, m_FirstDerived( NULL )
, m_NextSibling( NULL )
, m_Accept( NULL )
, m_Template( NULL )
{

}

Composite::~Composite()
{
    HELIUM_ASSERT( m_FirstDerived == NULL );
    HELIUM_ASSERT( m_NextSibling == NULL );
}

void Composite::Report() const
{
    Log::Debug( TXT( "Reflect Type: 0x%p, Size: %4d, Name: %s (0x%08x)\n" ), this, m_Size, m_Name, Crc32( m_Name ) );

    uint32_t computedSize = 0;
    DynArray< Field >::ConstIterator itr = m_Fields.Begin();
    DynArray< Field >::ConstIterator end = m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        computedSize += itr->m_Size;
        Log::Debug( TXT( "  Index: %3d, Size %4d, Name: %s\n" ), itr->m_Index, itr->m_Size, itr->m_Name );
    }

    if (computedSize != m_Size)
    {
        Log::Debug( TXT( " %d bytes of hidden fields\n" ), m_Size - computedSize );
    }
}

void Composite::Unregister() const
{
    if ( m_Base )
    {
        m_Base->RemoveDerived( this );
    }
}

bool Composite::IsType(const Composite* type) const
{
    for ( const Composite* base = this; base; base = base->m_Base )
    {
        if ( base == type )
        {
            return true;
        }
    }

    return false;
}

void Composite::AddDerived( const Composite* derived ) const
{
    HELIUM_ASSERT( derived );

    derived->m_NextSibling = m_FirstDerived;
    m_FirstDerived = derived;
}

void Composite::RemoveDerived( const Composite* derived ) const
{
    HELIUM_ASSERT( derived );

    if ( m_FirstDerived == derived )
    {
        m_FirstDerived = derived->m_NextSibling;
    }
    else
    {
        for ( const Composite* sibling = m_FirstDerived; sibling; sibling = sibling->m_NextSibling )
        {
            if ( sibling->m_NextSibling == derived )
            {
                sibling->m_NextSibling = derived->m_NextSibling;
                break;
            }
        }
    }

    derived->m_NextSibling = NULL;
}

bool Composite::Equals(const void* a, const void* b) const
{
    if (a == b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    DynArray< Field >::ConstIterator itr = m_Fields.Begin();
    DynArray< Field >::ConstIterator end = m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        const Field* field = &*itr;

        // create serializers
        DataPtr aData = field->CreateData();
        DataPtr bData = field->CreateData();

        // connnect
        aData->ConnectField(a, field);
        bData->ConnectField(b, field);

        bool serializersEqual = aData->Equals( bData );

        // disconnect
        aData->Disconnect();
        bData->Disconnect();

        // If the serialziers aren't equal, the elements can't be equal
        if ( !serializersEqual )
        {
            return false;
        }
    }

    return true;
}

void Composite::Visit(void* instance, Visitor& visitor) const
{
    if (!instance)
    {
        return;
    }

    DynArray< Field >::ConstIterator itr = m_Fields.Begin();
    DynArray< Field >::ConstIterator end = m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        const Field* field = &*itr;

        if ( !visitor.VisitField( instance, field ) )
        {
            continue;
        }

        DataPtr data = field->CreateData();

        data->ConnectField( instance, field );

        data->Accept( visitor );

        data->Disconnect();
    }
}

void Composite::Copy( const void* source, void* destination ) const
{
    if ( source != destination )
    {
#pragma TODO("This should be inside a virtual function (like CopyTo) instead of a type check conditional")
        if ( IsType( GetClass<Data>() ) )
        {
            const Data* data = static_cast<const Data*>(source);
            Data* cln = static_cast<Data*>(destination);
            cln->Set(data);
        }
        else
        {
            DynArray< Field >::ConstIterator itr = m_Fields.Begin();
            DynArray< Field >::ConstIterator end = m_Fields.End();
            for ( ; itr != end; ++itr )
            {
                const Field* field = &*itr;

                // create serializers
                DataPtr lhs = field->CreateData();
                DataPtr rhs = field->CreateData();

                // connnect
                lhs->ConnectField(destination, field);
                rhs->ConnectField(source, field);

                // for normal data types, run overloaded assignement operator via data's vtable
                // for reference container types, this deep copies containers (which is bad for 
                //  non-cloneable (FieldFlags::Share) reference containers)
                bool result = lhs->Set(rhs, field->m_Flags & FieldFlags::Share ? DataFlags::Shallow : 0);
                HELIUM_ASSERT(result);

                // disconnect
                lhs->Disconnect();
                rhs->Disconnect();
            }
        }
    }
}

const Field* Composite::FindFieldByName(uint32_t crc) const
{
    for ( const Composite* current = this; current != NULL; current = current->m_Base )
    {
        DynArray< Field >::ConstIterator itr = current->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = current->m_Fields.End();
        for ( ; itr != end; ++itr )
        {
            if ( Crc32( itr->m_Name ) == crc )
            {
                return &*itr;
            }
        }
    }

    return NULL;
}

const Field* Composite::FindFieldByIndex(uint32_t index) const
{
    for ( const Composite* current = this; current != NULL; current = current->m_Base )
    {
        if ( current->m_Fields.GetSize() && index >= current->m_Fields.GetFirst().m_Index && index <= current->m_Fields.GetFirst().m_Index )
        {
            return &current->m_Fields[ index - current->m_Fields.GetFirst().m_Index ];
        }
    }

    return NULL;
}

const Field* Composite::FindFieldByOffset(uint32_t offset) const
{
#pragma TODO("Implement binary search")
    for ( const Composite* current = this; current != NULL; current = current->m_Base )
    {
        if ( current->m_Fields.GetSize() && offset >= current->m_Fields.GetFirst().m_Offset && offset <= current->m_Fields.GetFirst().m_Offset )
        {
            DynArray< Field >::ConstIterator itr = current->m_Fields.Begin();
            DynArray< Field >::ConstIterator end = current->m_Fields.End();
            for ( ; itr != end; ++itr )
            {
                if ( itr->m_Offset == offset )
                {
                    return &*itr;
                }
            }
        }
    }

    return NULL;
}

uint32_t Composite::GetBaseFieldCount() const
{
    uint32_t count = 0;

    for ( const Composite* base = m_Base; base; base = base->m_Base )
    {
        if ( m_Base->m_Fields.GetSize() )
        {
            count = m_Base->m_Fields.GetLast().m_Index + 1;
            break;
        }
    }

    return count;
}

Reflect::Field* Composite::AddField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags )
{
    Field field;
    field.m_Composite = this;
    field.m_Name = name;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_DataClass = dataClass;
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}

Reflect::Field* Composite::AddObjectField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags )
{
    Field field;
    field.m_Composite = this;
    field.m_Name = name;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_Type = type;
    field.m_DataClass = dataClass ? dataClass : GetClass<PointerData>();
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}

Reflect::Field* Composite::AddEnumerationField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags )
{
    // if you hit this, then you need to make sure you register your enums before you register elements that use them
    HELIUM_ASSERT(enumeration != NULL);

    Field field;
    field.m_Composite = this;
    field.m_Name = name;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_Type = enumeration;
    field.m_DataClass = dataClass;
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}
