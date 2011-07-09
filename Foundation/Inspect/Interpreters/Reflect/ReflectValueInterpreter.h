#pragma once

#include "Foundation/API.h"

#include "ReflectFieldInterpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_FOUNDATION_API ReflectValueInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectValueInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent);
        };

        typedef Helium::SmartPtr<ReflectValueInterpreter> ReflectValueInterpreterPtr;
        typedef std::vector< ReflectValueInterpreterPtr > V_ReflectValueInterpreter;
    }
}