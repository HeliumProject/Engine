#pragma once

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/Interpreter.h"
#include "ReflectFieldInterpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API ReflectInterpreter : public Interpreter
        {
        public:
            ReflectInterpreter (Container* container);

            void Interpret(const std::vector<Reflect::Object*>& instances, int32_t includeFlags = 0xFFFFFFFF, int32_t excludeFlags = 0x0, bool expandPanel = true);
            void InterpretType(const std::vector<Reflect::Object*>& instances, Container* parent, int32_t includeFlags = 0xFFFFFFFF, int32_t excludeFlags = 0x0, bool expandPanel = true);

        private:
            std::vector<Reflect::Object*> m_Instances;
            std::vector<Reflect::DataPtr> m_Datas;
            V_ReflectFieldInterpreter m_Interpreters;
        };

        typedef Helium::StrongPtr<ReflectInterpreter> ReflectInterpreterPtr;

        class FOUNDATION_API ReflectFieldInterpreterFactory
        {
        public:
            template< class T >
            static ReflectFieldInterpreterPtr CreateInterpreter(Container* container)
            {
                return new T ( container );
            }

            typedef ReflectFieldInterpreterPtr (*Creator)(Container* container);
            typedef std::vector< std::pair<uint32_t, Creator> > V_Creator;
            typedef std::map< const Reflect::Class*, V_Creator > M_Creator;

            static void Register(const Reflect::Class* type, uint32_t mask, Creator creator);

            template< class T >
            static void Register(const Reflect::Class* type, uint32_t mask = 0x0)
            {
                Register( type, mask, &CreateInterpreter<T> );
            }

            static void Unregister(const Reflect::Class* type, uint32_t mask, Creator creator);

            template< class T >
            static void Unregister(const Reflect::Class* type, uint32_t mask = 0x0)
            {
                Unregister( type, mask, &CreateInterpreter<T> );
            }

            static ReflectFieldInterpreterPtr Create( const Reflect::Class* type, uint32_t flags, Container* container);
            static void Clear();

        private:
            static M_Creator m_Map;
        };
    }
}