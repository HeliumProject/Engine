#include "Composite.h"
#include "Element.h"
#include "Registry.h"
#include "Serializers.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;

Composite::Composite()
: m_Enumerator (NULL)
, m_Enumerated (false)
, m_FirstFieldID (-1)
, m_NextFieldID (0)
, m_LastFieldID (-1)
{

}

Composite::~Composite()
{

}

Reflect::Field* Composite::AddField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, int32_t serializerID, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    HELIUM_ASSERT(m_FieldIDToInfo.find( m_NextFieldID ) == m_FieldIDToInfo.end()); 

    // if you are here, maybe you repeated a field variable name twice in the class or its inheritance hierarchy?
    HELIUM_ASSERT(m_FieldNameToInfo.find( convertedName ) == m_FieldNameToInfo.end());

    Field* field = Field::Create( this );

    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_FieldID = m_NextFieldID;
    field->m_SerializerID = serializerID;

    m_FieldNameToInfo[convertedName] = field;
    m_FieldIDToInfo[m_NextFieldID] = field;
    m_FieldOffsetToInfo[offset] = field;

    m_NextFieldID++;

    SerializerPtr def = field->CreateSerializer( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateSerializer();

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

Reflect::ElementField* Composite::AddElementField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, int32_t serializerID, int32_t typeID, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    HELIUM_ASSERT(m_FieldIDToInfo.find( m_NextFieldID ) == m_FieldIDToInfo.end());

    // if you are here, maybe you repeated a field variable name twice in the class or its inheritance hierarchy?
    HELIUM_ASSERT(m_FieldNameToInfo.find( convertedName ) == m_FieldNameToInfo.end());

    ElementField* field = ElementField::Create( this );

    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_FieldID = m_NextFieldID;
    field->m_SerializerID = serializerID < 0 ? GetType<PointerSerializer>() : serializerID;
    field->m_TypeID = typeID;

    m_FieldNameToInfo[convertedName] = field;
    m_FieldIDToInfo[m_NextFieldID] = field;
    m_FieldOffsetToInfo[offset] = field;

    m_NextFieldID++;

    SerializerPtr def = field->CreateSerializer( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateSerializer();

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

Reflect::EnumerationField* Composite::AddEnumerationField(Element& instance, const std::string& name, const uint32_t offset, uint32_t size, int32_t serializerID, const Enumeration* enumeration, int32_t flags)
{
    tstring convertedName;
    {
        bool converted = Helium::ConvertString( name, convertedName );
        HELIUM_ASSERT( converted );
    }

    HELIUM_ASSERT(m_FieldIDToInfo.find( m_NextFieldID ) == m_FieldIDToInfo.end());

    // if you are here, maybe you repeated a field variable name twice in the class or its inheritance hierarchy?
    HELIUM_ASSERT(m_FieldNameToInfo.find( convertedName ) == m_FieldNameToInfo.end());

    // if you hit this, then you need to make sure you register your enums before you register elements that use them
    HELIUM_ASSERT(enumeration != NULL);

    EnumerationField* field = EnumerationField::Create( this, enumeration );

    field->SetName( convertedName );
    field->m_Size = size;
    field->m_Offset = offset;
    field->m_Flags = flags;
    field->m_FieldID = m_NextFieldID;
    field->m_SerializerID = serializerID;

    m_FieldNameToInfo[convertedName] = field;
    m_FieldIDToInfo[m_NextFieldID] = field;
    m_FieldOffsetToInfo[offset] = field;

    m_NextFieldID++;

    SerializerPtr def = field->CreateSerializer( &instance );
    if (def.ReferencesObject())
    {
        field->m_Default = field->CreateSerializer();

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

void Composite::Report() const
{
    static tchar_t buf[8192];

    _sntprintf(buf, sizeof(buf), TXT( "Reflect Type ID: %3d, Size: %4d, Name: `%s`\n" ), m_TypeID, m_Size, m_Name.c_str());
    buf[ sizeof(buf) - 1] = 0; 

    Log::Debug(Log::Levels::Verbose,  buf );

    uint32_t computedSize = 0;
    M_FieldIDToInfo::const_iterator itr = m_FieldIDToInfo.begin();
    M_FieldIDToInfo::const_iterator end = m_FieldIDToInfo.end();
    for ( ; itr != end; ++itr )
    {
        computedSize += itr->second->m_Size;
        Log::Debug(Log::Levels::Verbose, TXT( "  Field ID: %3d, Size %4d, Name: `%s`\n" ), itr->first, itr->second->m_Size, itr->second->m_Name.c_str());
    }

    if (computedSize != m_Size)
    {
        Log::Debug(Log::Levels::Verbose, TXT( " %d bytes of hidden fields\n" ), m_Size - computedSize);
    }
}

bool Composite::HasType(int32_t type) const
{
    const Composite* typeInfo = this;

    while ( typeInfo )
    {
        if ( typeInfo->m_TypeID == type )
        {
            return true;
        }

        typeInfo = ReflectionCast<const Composite>( Reflect::Registry::GetInstance()->GetType( typeInfo->m_Base ) );
    }

    return false;
}

tstring Composite::ShortenName(const tstring& name)
{
    if (name.find( TXT("<") ) != tstring::npos)
    {
        HELIUM_BREAK();
    }
    else
    {
        // look for the space after "struct " or "class "
        size_t offset = name.rfind( TXT(" ") );
        if (offset != std::string::npos)
        {
            return name.substr(offset+1);
        }
    }

    return name;
}

const Field* Composite::FindFieldByName(const tstring& name) const
{
    M_FieldNameToInfo::const_iterator iter = m_FieldNameToInfo.find( name );

    if ( iter != m_FieldNameToInfo.end() )
    {
        return iter->second;
    }

    return NULL;
}

const Field* Composite::FindFieldByOffset(uint32_t offset) const
{
    M_FieldOffsetToInfo::const_iterator found = m_FieldOffsetToInfo.find( offset );
    if ( found != m_FieldOffsetToInfo.end() )
    {
        return found->second;
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

    if (a->HasType(Reflect::GetType<Serializer>()))
    {
        const Serializer* aSerializer = static_cast<const Serializer*>(a);
        const Serializer* bSerializer = static_cast<const Serializer*>(b);

        return aSerializer->Equals(bSerializer);
    }
    else
    {
        M_FieldIDToInfo::const_iterator itr = type->m_FieldIDToInfo.begin();
        M_FieldIDToInfo::const_iterator end = type->m_FieldIDToInfo.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = itr->second;

            // create serializers
            SerializerPtr aSerializer = field->CreateSerializer();
            SerializerPtr bSerializer = field->CreateSerializer();

            // connnect
            aSerializer->ConnectField(a, field);
            bSerializer->ConnectField(b, field);

            bool serializersEqual = aSerializer->Equals( bSerializer );

            // disconnect
            aSerializer->Disconnect();
            bSerializer->Disconnect();

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
        M_FieldIDToInfo::const_iterator itr = type->m_FieldIDToInfo.begin();
        M_FieldIDToInfo::const_iterator end = type->m_FieldIDToInfo.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = itr->second;

            if (!visitor.VisitField(element, field))
            {
                continue;
            }

            SerializerPtr serializer = field->CreateSerializer();

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
        for ( const Class* currentType = srcType; currentType && !type; currentType = registry->GetClass( currentType->m_Base ) )
        {
            if ( dest->HasType( currentType->m_TypeID ) )
            {
                // We found the match (which breaks out of this loop)
                type = currentType;
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

    if (src->HasType(Reflect::GetType<Serializer>()))
    {
        const Serializer* ser = static_cast<const Serializer*>(src);
        Serializer* cln = static_cast<Serializer*>(dest);

        cln->Set(ser);
    }
    else
    {
        M_FieldIDToInfo::const_iterator itr = type->m_FieldIDToInfo.begin();
        M_FieldIDToInfo::const_iterator end = type->m_FieldIDToInfo.end();
        for ( ; itr != end; ++itr )
        {
            const Field* field = itr->second;

            // create serializers
            SerializerPtr lhs = field->CreateSerializer();
            SerializerPtr rhs = field->CreateSerializer();

            // connnect
            lhs->ConnectField(dest, field);
            rhs->ConnectField(src, field);

            // for normal data types, run overloaded assignement operator via serializer's vtable
            // for reference container types, this deep copies containers (which is bad for 
            //  non-cloneable (FieldFlags::Share) reference containers)
            bool result = lhs->Set(rhs, field->m_Flags & FieldFlags::Share ? SerializerFlags::Shallow : 0);
            HELIUM_ASSERT(result);

            // disconnect
            lhs->Disconnect();
            rhs->Disconnect();
        }
    }
}
