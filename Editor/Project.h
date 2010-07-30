#pragma once

#include "Editor/API.h"
#include "Foundation/Reflect/Document.h"

namespace Helium
{
    namespace Editor
    {
        class ProjectFile : public Reflect::ConcreteInheritor< ProjectFile, Reflect::DocumentElement >
        {
        public:
            ProjectFile( const Helium::Path& path )
                : m_Path ( path )
            {

            }

            Helium::Path GetPath()
            {
                return m_Path;
            }

        private:
            Helium::Path m_Path;

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Path, "m_Path" );
            }
        };

        typedef Helium::SmartPtr< ProjectFile > ProjectFilePtr;

        class Project : public Reflect::ConcreteInheritor< Project, Reflect::Document >
        {
        public:
            const Helium::Path& GetPath()
            {
                return m_Path;
            }

            void SetPath(const Helium::Path& path)
            {
                m_Path = path;
            }

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {

            }

        private:
            Helium::Path m_Path;
        };

        typedef Helium::SmartPtr<Project> ProjectPtr;
    }
}