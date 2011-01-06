#include "Foundation/Reflect/Composite.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/ArchiveBinary.h"

using namespace Helium;
using namespace Helium::Reflect;

Field::Field()
: m_Composite( NULL )
, m_Flags( 0 )
, m_Index( -1 )
, m_Type( NULL )
, m_DataClass( NULL )
, m_Offset( -1 )
, m_Creator( NULL )
{

}

DataPtr Field::CreateData(Element* instance) const
{
    DataPtr ser;

    if ( m_DataClass != NULL )
    {
        ObjectPtr object = Registry::GetInstance()->CreateInstance( m_DataClass );

        if (object.ReferencesObject())
        {
            ser = AssertCast<Data>(object);
        }
    }

    if ( ser.ReferencesObject() )
    {
        if ( instance )
        {
            ser->ConnectField( instance, this );
        }

        const Class* classType = ReflectionCast< Class >( m_Type );
        if ( classType )
        {
            PointerData* pointerData = ObjectCast<PointerData>( ser );
            if ( pointerData )
            {
                pointerData->m_Type = m_Type;
            }
            else
            {
                ElementContainerData* containerData = ObjectCast<ElementContainerData>( ser );
                if ( containerData )
                {
                    containerData->m_Type = m_Type;
                }
            }
        }
    }

    return ser;
}

bool Field::HasDefaultValue(Element* instance) const
{
#ifdef REFLECT_REFACTOR
    // if we don't have a default value, we can never be at the default value
    if (!m_Default.ReferencesObject())
    {
        return false;
    }

    // get a serializer
    DataPtr serializer = CreateData();

    if (serializer.ReferencesObject())
    {
        // set data pointer
        serializer->ConnectField(instance, this);

        // return equality
        bool result = m_Default->Equals(serializer);

        // disconnect
        serializer->Disconnect();

        // result
        return result;
    }
#endif

    return false;
}

bool Field::SetDefaultValue(Element* instance) const
{
#ifdef REFLECT_REFACTOR
    // if we don't have a default value, we can never be at the default value
    if (!m_Default.ReferencesObject())
    {
        return false;
    }

    // get a serializer
    DataPtr serializer = CreateData();

    if (serializer.ReferencesObject())
    {
        // set data pointer
        serializer->ConnectField(instance, this);

        // copy the data
        serializer->Set(m_Default);

        // disconnect
        serializer->Disconnect();

        return true;
    }
#endif

    return false;
}

Composite::Composite()
: m_Base( NULL )
, m_FirstDerived( NULL )
, m_NextSibling( NULL )
, m_Accept( NULL )
{

}

Composite::~Composite()
{
    HELIUM_ASSERT( m_FirstDerived == NULL );
    HELIUM_ASSERT( m_NextSibling == NULL );
}

void Composite::Report() const
{
    Log::Debug( TXT( "Reflect Type: 0x%p, Size: %4d, Name: %s (0x%08x)\n" ), this, m_Size, *m_Name, Crc32( *m_Name ) );

    uint32_t computedSize = 0;
    DynArray< Field >::ConstIterator itr = m_Fields.Begin();
    DynArray< Field >::ConstIterator end = m_Fields.End();
    for ( ; itr != end; ++itr )
    {
        computedSize += itr->m_Size;
        Log::Debug( TXT( "  Index: %3d, Size %4d, Name: `%s`\n" ), itr->m_Index, itr->m_Size, itr->m_Name.c_str() );
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

void Composite::AddDerived( const Composite* derived ) const
{
    const Composite* last = m_FirstDerived;

    while ( last && last->m_NextSibling )
    {
        last = last->m_NextSibling;
    }

    if ( last )
    {
        last->m_NextSibling = derived;
    }
    else
    {
        m_FirstDerived = derived;
    }
}

void Composite::RemoveDerived( const Composite* derived ) const
{
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
                sibling->m_NextSibling = sibling->m_NextSibling ? sibling->m_NextSibling->m_NextSibling : NULL;
                derived->m_NextSibling = NULL;
                break;
            }
        }
    }
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

Reflect::Field* Composite::AddField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags )
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    Field field;
    field.m_Name = convertedName;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_DataClass = dataClass;
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}

Reflect::Field* Composite::AddElementField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags )
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    Field field;
    field.m_Name = convertedName;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_Type = type;
    field.m_DataClass = dataClass ? dataClass : GetClass<PointerData>();
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}

Reflect::Field* Composite::AddEnumerationField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags )
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    // if you hit this, then you need to make sure you register your enums before you register elements that use them
    HELIUM_ASSERT(enumeration != NULL);

    Field field;
    field.m_Name = convertedName;
    field.m_Size = size;
    field.m_Offset = offset;
    field.m_Flags = flags;
    field.m_Index = GetBaseFieldCount() + (uint32_t)m_Fields.GetSize();
    field.m_Type = enumeration;
    field.m_DataClass = dataClass;
    m_Fields.Add( field );

    return &m_Fields.GetLast();
}

bool Composite::HasType(const Type* type) const
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

const Field* Composite::FindFieldByName(uint32_t crc) const
{
    for ( const Composite* current = this; current != NULL; current = current->m_Base )
    {
        DynArray< Field >::ConstIterator itr = current->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = current->m_Fields.End();
        for ( ; itr != end; ++itr )
        {
            if ( Crc32( itr->m_Name.c_str() ) == crc )
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

bool Composite::Equals(const Element* a, const Element* b)
{
    if (a == b)
    {
        return true;
    }

    if (!a || !b)
    {
        return false;
    }

    const Class* type = a->GetClass();

    if ( type != b->GetClass() )
    {
        return false;
    }

    if (a->HasType(Reflect::GetType<Data>()))
    {
        const Data* aData = static_cast<const Data*>(a);
        const Data* bData = static_cast<const Data*>(b);

        return aData->Equals(bData);
    }
    else
    {
        DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = type->m_Fields.End();
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
    }

    return true;
}

void Composite::Visit(Element* element, Visitor& visitor)
{
    if (!element)
    {
        return;
    }

    if (!visitor.VisitElement(element))
    {
        return;
    }

    const Class* type = element->GetClass();

    {
        DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = type->m_Fields.End();
        for ( ; itr != end; ++itr )
        {
            const Field* field = &*itr;

            if (!visitor.VisitField(element, field))
            {
                continue;
            }

            DataPtr serializer = field->CreateData();

            serializer->ConnectField( element, field );

            serializer->Accept( visitor );

            serializer->Disconnect();
        }
    }
}

void Composite::Copy( const Element* src, Element* dest )
{
    if ( src == dest )
    {
        throw Reflect::LogisticException( TXT( "Internal error (attempted to copy element %s into itself)" ), *src->GetClass()->m_Name );
    }

    // 
    // Find common base class
    // 

    // This is the common base class type
    const Class* type = NULL; 
    const Class* srcType = src->GetClass();
    const Class* destType = dest->GetClass();

    // Simplest case: the types are the same
    if ( srcType == destType )
    {
        type = srcType;
    }
    else
    {
        // Types are not the same, we have to search...
        // Iterate up inheritance of src, and look check to see if dest HasType for each one
        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        for ( const Composite* base = srcType; base && !type; base = base->m_Base )
        {
            if ( dest->HasType( base ) )
            {
                // We found the match (which breaks out of this loop)
                type = ReflectionCast<const Class>( base );
            }
        }

        if ( !type )
        {
            // This should be impossible... at the very least, Element is a common base class for both pointers.
            // This exeception means there's a bug in this function.
            throw Reflect::TypeInformationException( TXT( "Internal error (could not find common base class for %s and %s)" ), *srcType->m_Name, *destType->m_Name );
        }
    }

    // 
    // Carry out the copy operation
    // 

    if (src->HasType(Reflect::GetType<Data>()))
    {
        const Data* ser = static_cast<const Data*>(src);
        Data* cln = static_cast<Data*>(dest);

        cln->Set(ser);
    }
    else
    {
        DynArray< Field >::ConstIterator itr = type->m_Fields.Begin();
        DynArray< Field >::ConstIterator end = type->m_Fields.End();
        for ( ; itr != end; ++itr )
        {
            const Field* field = &*itr;

            // create serializers
            DataPtr lhs = field->CreateData();
            DataPtr rhs = field->CreateData();

            // connnect
            lhs->ConnectField(dest, field);
            rhs->ConnectField(src, field);

            // for normal data types, run overloaded assignement operator via serializer's vtable
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
