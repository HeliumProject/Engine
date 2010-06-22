#pragma once

#include "Luna/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Luna
{
    class Project;
    typedef Nocturnal::SmartPtr< Project> ProjectPtr;

    class LUNA_EDITOR_API Project : public Reflect::Element
    {
    private:
        REFLECT_DECLARE_CLASS( Project, Reflect::Element );
        static void EnumerateClass( Reflect::Compositor< Project >& comp );

    public:
        Project();

    private:
        std::set< Nocturnal::Path > m_Paths;

    public:

        const std::set< Nocturnal::Path >& GetPaths() const;
    };
}