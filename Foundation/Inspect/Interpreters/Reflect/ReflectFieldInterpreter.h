#pragma once

#include "Foundation/API.h"

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Interpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API ReflectFieldInterpreter : public Interpreter
        {
        public:
            ReflectFieldInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent) = 0;

        protected:
            std::vector<Reflect::Object*> m_Instances;
            std::vector<Reflect::DataPtr> m_Datas;
        };

        typedef Helium::StrongPtr<ReflectFieldInterpreter> ReflectFieldInterpreterPtr;
        typedef std::vector< ReflectFieldInterpreterPtr > V_ReflectFieldInterpreter;
    }
}