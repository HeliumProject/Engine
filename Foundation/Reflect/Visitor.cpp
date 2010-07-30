#include "Visitor.h"
#include "Serializers.h"

using namespace Helium::Reflect;

FindByTypeVisitor::FindByTypeVisitor(i32 type, V_Element& found)
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

FindByTypeSetVisitor::FindByTypeSetVisitor(const std::set< i32 >& types, V_Element& found)
: m_Types (types)
, m_Found (found)
{

}

bool FindByTypeSetVisitor::VisitElement(Element* element)
{
    std::set< i32 >::const_iterator itr = m_Types.begin();
    std::set< i32 >::const_iterator end = m_Types.end();
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