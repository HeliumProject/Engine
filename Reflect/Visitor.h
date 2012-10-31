#pragma once

#include <set>
#include <vector>

#include "API.h"
#include "Foundation/SmartPtr.h"

namespace Helium
{
    namespace Reflect
    {
        class Structure;
        class Field;

        class HELIUM_REFLECT_API Visitor : public Helium::AtomicRefCountBase< Visitor >, NonCopyable
        {
        public:
            virtual bool VisitObject(Object* /*object*/)
            {
                return true;
            }

            virtual bool VisitPointer(ObjectPtr& /*pointer*/)
            {
                return true;
            }

            virtual bool VisitField(void* /*instance*/, const Field* /*field*/)
            {
                return true; 
            }
        };

        typedef Helium::SmartPtr<Visitor> VisitorPtr;

        class HELIUM_REFLECT_API FindByTypeVisitor : public Visitor
        {
        public:
            const Reflect::Class* m_Class;

            std::vector< ObjectPtr >& m_Found;

            FindByTypeVisitor(const Reflect::Class* type, std::vector< ObjectPtr >& found);

            virtual bool VisitObject(Object* object) HELIUM_OVERRIDE;
        };

        class HELIUM_REFLECT_API FindByTypeSetVisitor : public Visitor
        {
        public:
            const std::set< const Reflect::Class* >& m_Classes;

            std::vector< ObjectPtr >& m_Found;

            FindByTypeSetVisitor(const std::set< const Reflect::Class* >& classes, std::vector< ObjectPtr >& found);

            virtual bool VisitObject(Object* object) HELIUM_OVERRIDE;
        };
    }
}