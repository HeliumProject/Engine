const Helium::Reflect::Type* Helium::Reflect::Registry::GetType( Name name ) const
{
    uint32_t crc = Crc32( *name );
    return GetType( crc );
}

const Helium::Reflect::Class* Helium::Reflect::Registry::GetClass( Name name ) const
{
    uint32_t crc = Crc32( *name );
    return GetClass( crc );
}

const Helium::Reflect::Enumeration* Helium::Reflect::Registry::GetEnumeration( Name name ) const
{
    uint32_t crc = Crc32( *name );
    return GetEnumeration( crc );
}

Helium::Reflect::ObjectPtr Helium::Reflect::Registry::CreateInstance( Name name ) const
{
    uint32_t crc = Crc32( *name );
    return CreateInstance( crc );
}