#include "Visitor.h"
#include "SerializerDeduction.h"

using namespace Helium::Reflect;

FindByTypeVisitor::FindByTypeVisitor(int32_t type, std::vector< ElementPtr >& found)
: m_Type (type)
, m_Found (found)
{

}

bool FindByTypeVisitor::VisitElement(Element* element)
{
    if (element->HasType(m_Type))
    {
        m_Found.push_back(element);
    }

    return true;
}

FindByTypeSetVisitor::FindByTypeSetVisitor(const std::set< int32_t >& types, std::vector< ElementPtr >& found)
: m_Types (types)
, m_Found (found)
{

}

bool FindByTypeSetVisitor::VisitElement(Element* element)
{
    std::set< int32_t >::const_iterator itr = m_Types.begin();
    std::set< int32_t >::const_iterator end = m_Types.end();
    for ( ; itr != end; ++itr )
    {
        if (element->HasType(*itr))
        {
            m_Found.push_back(element);
            break;
        }
    }

    return true;
}