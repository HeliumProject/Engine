#include "ReflectPch.h"
#include "Reflect/Visitor.h"
#include "Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

FindByTypeVisitor::FindByTypeVisitor(const Reflect::Class* type, std::vector< ObjectPtr >& found)
: m_Class( type )
, m_Found( found )
{

}

bool FindByTypeVisitor::VisitObject(Object* object)
{
    if (object->IsClass(m_Class))
    {
        m_Found.push_back(object);
    }

    return true;
}

FindByTypeSetVisitor::FindByTypeSetVisitor(const std::set< const Reflect::Class* >& classes, std::vector< ObjectPtr >& found)
: m_Classes( classes )
, m_Found( found )
{

}

bool FindByTypeSetVisitor::VisitObject(Object* object)
{
    std::set< const Reflect::Class* >::const_iterator itr = m_Classes.begin();
    std::set< const Reflect::Class* >::const_iterator end = m_Classes.end();
    for ( ; itr != end; ++itr )
    {
        if (object->IsClass(*itr))
        {
            m_Found.push_back(object);
            break;
        }
    }

    return true;
}