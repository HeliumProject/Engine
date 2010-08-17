#pragma once

#include "Application/API.h"

#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/InspectInterpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API ReflectFieldInterpreter : public Interpreter
        {
        public:
            ReflectFieldInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent) = 0;

        protected:
            std::vector<Reflect::Element*> m_Instances;
            std::vector<Reflect::SerializerPtr> m_Serializers;
        };

        typedef Helium::SmartPtr<ReflectFieldInterpreter> ReflectFieldInterpreterPtr;
        typedef std::vector< ReflectFieldInterpreterPtr > V_ReflectFieldInterpreter;
    }
}