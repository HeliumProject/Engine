#pragma once

#include <set>
#include <vector>

#include "API.h"
#include "Foundation/Atomic.h"

namespace Helium
{
    namespace Reflect
    {
        class Structure;
        class Field;

        class FOUNDATION_API Visitor : public Helium::AtomicRefCountBase
        {
        public:
            virtual bool VisitElement(Element* element)
            {
                return true;
            }

            virtual bool VisitPointer(ElementPtr& pointer)
            {
                return true;
            }

            virtual bool VisitField(Element* element, const Field* field)
            {
                return true; 
            }
        };

        typedef Helium::SmartPtr<Visitor> VisitorPtr;

        class FOUNDATION_API FindByTypeVisitor : public Visitor
        {
        public:
            int32_t m_Type;

            std::vector< ElementPtr >& m_Found;

            FindByTypeVisitor(int32_t type, std::vector< ElementPtr >& found);

            virtual bool VisitElement(Element* element) HELIUM_OVERRIDE;
        };

        class FOUNDATION_API FindByTypeSetVisitor : public Visitor
        {
        public:
            const std::set< int32_t >& m_Types;

            std::vector< ElementPtr >& m_Found;

            FindByTypeSetVisitor(const std::set< int32_t >& types, std::vector< ElementPtr >& found);

            virtual bool VisitElement(Element* element) HELIUM_OVERRIDE;
        };
    }
}