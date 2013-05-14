#pragma once

#include "Inspect/API.h"

#include "ReflectFieldInterpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_INSPECT_API ReflectValueInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectValueInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent);
        };
    }
}