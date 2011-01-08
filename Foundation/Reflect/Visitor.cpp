#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

FindByTypeVisitor::FindByTypeVisitor(const Reflect::Type* type, std::vector< ObjectPtr >& found)
: m_Type( type )
, m_Found( found )
{

}

bool FindByTypeVisitor::VisitObject(Object* element)
{
    if (element->HasType(m_Type))
    {
        m_Found.push_back(element);
    }

    return true;
}

FindByTypeSetVisitor::FindByTypeSetVisitor(const std::set< const Reflect::Type* >& types, std::vector< ObjectPtr >& found)
: m_Types( types )
, m_Found( found )
{

}

bool FindByTypeSetVisitor::VisitObject(Object* element)
{
    std::set< const Reflect::Type* >::const_iterator itr = m_Types.begin();
    std::set< const Reflect::Type* >::const_iterator end = m_Types.end();
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