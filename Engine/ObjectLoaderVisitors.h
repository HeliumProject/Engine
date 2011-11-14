
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Reflect/Composite.h"
#include "Foundation/Container/DynArray.h"

namespace Helium
{
    // Called it something bad happens during deserialization
    class HELIUM_ENGINE_API ClearLinkIndicesFromObject : public Helium::Reflect::Visitor
    {        
    public:
        ClearLinkIndicesFromObject();
        virtual ~ClearLinkIndicesFromObject();

        virtual bool VisitField(void* instance, const Helium::Reflect::Field* field) HELIUM_OVERRIDE;
    };
}