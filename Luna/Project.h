#pragma once

#include "Luna/API.h"
#include "Foundation/Reflect/Document.h"

namespace Luna
{
    class ProjectFile : public Reflect::ConcreteInheritor< ProjectFile, Reflect::DocumentElement >
    {
    public:
        ProjectFile( const Nocturnal::Path& path )
            : m_Path ( path )
        {

        }

        Nocturnal::Path GetPath()
        {
            return m_Path;
        }

    private:
        Nocturnal::Path m_Path;

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {
            comp.AddField( &This::m_Path, "m_Path" );
        }
    };

    typedef Nocturnal::SmartPtr< ProjectFile > ProjectFilePtr;

    class Project : public Reflect::ConcreteInheritor< Project, Reflect::Document >
    {
    public:
        const Nocturnal::Path& GetPath()
        {
            return m_Path;
        }

        void SetPath(const Nocturnal::Path& path)
        {
            m_Path = path;
        }

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {

        }

    private:
        Nocturnal::Path m_Path;
    };

    typedef Nocturnal::SmartPtr<Project> ProjectPtr;
}