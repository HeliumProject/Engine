#pragma once

#include "Luna/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Luna
{
    class LUNA_EDITOR_API Project : public Reflect::ConcreteInheritor< Project, Reflect::Element >
    {
    public:
        static void EnumerateClass( Reflect::Compositor< Project >& comp );

        Project();

    public:
        const std::set< Nocturnal::Path >& GetFiles() const
        {
            return m_Files;
        }

    private:
        std::set< Nocturnal::Path > m_Files;
    };

    typedef Nocturnal::SmartPtr< Project> ProjectPtr;
}