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
    return m_Parent != -1 ? m_Document->GetElement( m_Parent ) : NULL;
}

Helium::XMLElement* Helium::XMLElement::GetFirstChild()
{
    return m_FirstChild != -1 ? m_Document->GetElement( m_FirstChild ) : NULL;
}

Helium::XMLElement* Helium::XMLElement::GetNextSibling()
{
    return m_NextSibling != -1 ? m_Document->GetElement( m_NextSibling ) : NULL;
}

Helium::XMLDocument::Iterator::Iterator()
: m_Current( NULL )
{

}

Helium::XMLDocument::Iterator::Iterator( XMLDocument* document )
: m_Current( document->GetRoot() )
{

}

bool Helium::XMLDocument::Iterator::IsDone()
{
    return m_Current == NULL;
}

Helium::XMLElement* Helium::XMLDocument::Iterator::Advance( bool skipChildren )
{
    HELIUM_ASSERT( !IsDone() );

    XMLElement* previous = m_Current;

    if ( skipChildren )
    {
        // always decend to the children first
        m_Current = m_Current->GetFirstChild();
        if ( m_Current )
        {
            return m_Current;
        }
    }

    // no more children, next sibling
    m_Current = m_Current->GetNextSibling();
    if ( m_Current )
    {
        return m_Current;
    }

    // no more siblings, find the next uncle
    while ( m_Current == NULL )
    {
        // get the previous location's parent
        m_Current = previous->GetParent();

        // if we have a parent (we aren't the root)
        if ( m_Current )
        {
            // set the previous location to be this parent
            previous = m_Current;

            // get the next sibling of of this parent (uncle)
            m_Current = previous->GetNextSibling();
        }
        else
        {
            break;
        }
    }

    return m_Current;
}

Helium::XMLElement* Helium::XMLDocument::Iterator::GetCurrent()
{
    return m_Current;
}

void Helium::XMLDocument::Iterator::SetCurrent( XMLElement* element )
{
    m_Current = element;
}

Helium::XMLElement* Helium::XMLDocument::GetRoot()
{
    return &m_Elements[ 0 ];
}

Helium::XMLElement* Helium::XMLDocument::GetElement( int32_t index )
{
    return &m_Elements[ index ];
}