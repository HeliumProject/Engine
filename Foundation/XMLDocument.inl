Helium::XMLElement::XMLElement()
: m_Index( -1 )
, m_Parent( -1 )
, m_FirstChild( -1 )
, m_NextSibling( -1 )
{

}

const Helium::String* Helium::XMLElement::GetAttributeValue( const Name& name )
{
    AttributeMap::ConstIterator found = m_Attributes.Find( name );
    if ( found != m_Attributes.End() )
    {
        return &found->Second();
    }

    return NULL;
}

Helium::XMLDocument* Helium::XMLElement::GetDocument()
{
    return m_Document;
}

Helium::XMLElement* Helium::XMLElement::GetParent()
{
    return m_Document->GetElement( m_Parent );
}

Helium::XMLElement* Helium::XMLElement::GetFirstChild()
{
    return m_Document->GetElement( m_FirstChild );
}

Helium::XMLElement* Helium::XMLElement::GetNextSibling()
{
    return m_Document->GetElement( m_NextSibling );
}

Helium::XMLElement* Helium::XMLDocument::GetRoot()
{
    return &m_Elements[ 0 ];
}

Helium::XMLElement* Helium::XMLDocument::GetElement( int32_t index )
{
    return &m_Elements[ index ];
}