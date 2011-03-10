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

Helium::XMLElement* Helium::XMLElement::GetNext( bool skipChildren )
{
    HELIUM_ASSERT( this );

    XMLElement* next = NULL;

    if ( !skipChildren )
    {
        // always decend to the children first
        next = GetFirstChild();
    }

    if ( !next )
    {
        // no more children, next sibling
        next = GetNextSibling();
    }

    XMLElement* previous = this;

    // no more siblings, find the next uncle
    while ( !next )
    {
        // get the relative location's parent
        next = previous->GetParent();

        // if we have a parent (we aren't the root)
        if ( next )
        {
            // set the relative location to be this parent
            previous = next;

            // get the next sibling of of this parent (uncle)
            next = previous->GetNextSibling();
        }
        else
        {
            break;
        }
    }

    return next;
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

    return m_Current = m_Current->GetNext( skipChildren );
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