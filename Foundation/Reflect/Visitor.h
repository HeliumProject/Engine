#pragma once

#include <set>
#include <vector>

#include "API.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Reflect
    {
        class Structure;
        class Field;

        class FOUNDATION_API Visitor : public Helium::AtomicRefCountBase< Visitor >, NonCopyable
        {
        public:
            virtual bool VisitObject(Object* object)
            {
                return true;
            }

            virtual bool VisitPointer(ObjectPtr& /*pointer*/)
            {
                return true;
            }

            virtual bool VisitField(void* instance, const Field* field)
            {
                return true; 
            }
        };

        typedef Helium::SmartPtr<Visitor> VisitorPtr;

        class FOUNDATION_API FindByTypeVisitor : public Visitor
        {
        public:
            const Reflect::Type* m_Type;

            std::vector< ObjectPtr >& m_Found;

            FindByTypeVisitor(const Reflect::Type* type, std::vector< ObjectPtr >& found);

            virtual bool VisitObject(Object* object) HELIUM_OVERRIDE;
        };

        class FOUNDATION_API FindByTypeSetVisitor : public Visitor
        {
        public:
            const std::set< const Reflect::Type* >& m_Types;

            std::vector< ObjectPtr >& m_Found;

            FindByTypeSetVisitor(const std::set< const Reflect::Type* >& types, std::vector< ObjectPtr >& found);

            virtual bool VisitObject(Object* object) HELIUM_OVERRIDE;
        };
    }
}