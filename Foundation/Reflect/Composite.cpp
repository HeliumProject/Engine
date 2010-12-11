#include "Composite.h"
#include "Element.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;

Composite::Composite()
: m_Enumerator (NULL)
, m_Enumerated (false)
{

}

Composite::~Composite()
{

}

void Composite::Report() const
{
    Log::Debug( Log::Levels::Verbose, TXT( "Reflect Type: 0x%p, Size: %4d, Name: `%s`\n" ), this, m_Size, *m_Name );

    uint32_t computedSize = 0;
    std::vector< ConstFieldPtr >::const_iterator itr = m_Fields.begin();
    std::vector< ConstFieldPtr >::const_iterator end = m_Fields.end();
    for ( ; itr != end; ++itr )
    {
        computedSize += (*itr)->m_Size;
        Log::Debug( Log::Levels::Verbose, TXT( "  Field ID: %3d, Size %4d, Name: `%s`\n" ), (*itr)->m_Index, (*itr)->m_Size, (*itr)->m_Name.c_str() );
    }

    if (computedSize != m_Size)
    {
        Log::Debug( Log::Levels::Verbose, TXT( " %d bytes of hidden fields\n" ), m_Size - computedSize );
    }
}

void Composite::Unregister() const
{
    m_Base->m_Derived.Remove( this );
}

uint32_t Composite::GetBaseFieldCount() const
{
    uint32_t count = 0;

    for ( const Composite* base = m_Base; base; base = base->m_Base )
    {
        if ( m_Base->m_Fields.size() )
        {
            count = m_Base->m_Fields.back()->m_Index;
            break;
        }
    }

    return count;
}

Reflect::Field* Composite::AddField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    Field* field = Field::Create( this );
    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_Index = GetBaseFieldCount();
    field->m_DataClass = dataClass;
    m_Fields.push_back( field );

    DataPtr def = field->CreateData( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateData();

        try
        {
            field->m_Default->Set( def );
        }
        catch (Reflect::Exception&)
        {
            field->m_Default = NULL;
        }
    }

    return field;
}

Reflect::ElementField* Composite::AddElementField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    ElementField* field = ElementField::Create( this );
    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_Index = GetBaseFieldCount();
    field->m_DataClass = dataClass ? dataClass : GetClass<PointerData>();
    field->m_Type = type;
    m_Fields.push_back( field );

    DataPtr def = field->CreateData( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateData();

        try
        {
            field->m_Default->Set( def );
        }
        catch (Reflect::Exception&)
        {
            field->m_Default = NULL;
        }
    }

    return field;
}

Reflect::EnumerationField* Composite::AddEnumerationField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    // if you hit this, then you need to make sure you register your enums before you register elements that use them
    HELIUM_ASSERT(enumeration != NULL);

    EnumerationField* field = EnumerationField::Create( this, enumeration );
    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_Index = GetBaseFieldCount();
    field->m_DataClass = dataClass;
    m_Fields.push_back( field );

    DataPtr def = field->CreateData( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateData();

        try
        {
            field->m_Default->Set( def );
        }
        catch (Reflect::Exception&)
        {
            field->m_Default = NULL;
        }
    }

    return field;
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

const Field* Composite::FindFieldByName(const tstring& name) const
{
    std::vector< ConstFieldPtr >::const_iterator itr = m_Fields.begin();
    std::vector< ConstFieldPtr >::const_iterator end = m_Fields.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->m_Name == name )
        {
            return *itr;
        }
    }

    return NULL;
}

const Field* Composite::FindFieldByIndex(uint32_t index) const
{
    std::vector< ConstFieldPtr >::const_iterator itr = m_Fields.begin();
    std::vector< ConstFieldPtr >::const_iterator end = m_Fields.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->m_Index == index )
        {
            return *itr;
        }
    }

    return NULL;
}

const Field* Composite::FindFieldByOffset(uint32_t offset) const
{
    std::vector< ConstFieldPtr >::const_iterator itr = m_Fields.begin();
    std::vector< ConstFieldPtr >::const_iterator end = m_Fields.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->m_Offset == offset )
        {
            return *itr;
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
        std::vector< ConstFieldPtr >::const_iterator itr = type->m_Fields.begin();
        std::vector< ConstFieldPtr >::const_iterator end = type->m_Fields.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = (*itr);

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
        std::vector< ConstFieldPtr >::const_iterator itr = type->m_Fields.begin();
        std::vector< ConstFieldPtr >::const_iterator end = type->m_Fields.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = (*itr);

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
        throw Reflect::LogisticException( TXT( "Internal error (attempted to copy element %s into itself)" ), src->GetClass()->m_UIName.c_str() );
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
            throw Reflect::TypeInformationException( TXT( "Internal error (could not find common base class for %s and %s)" ), srcType->m_UIName.c_str(), destType->m_UIName.c_str() );
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
        std::vector< ConstFieldPtr >::const_iterator itr = type->m_Fields.begin();
        std::vector< ConstFieldPtr >::const_iterator end = type->m_Fields.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = (*itr);

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
