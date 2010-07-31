#pragma once

#include "Core/API.h"
#include "Foundation/Reflect/Document.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API ProjectFile : public Reflect::ConcreteInheritor< ProjectFile, Reflect::DocumentElement >
        {
        public:
            ProjectFile()
            {

            }

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
                comp.AddField( &This::m_Path, "Path" );
            }
        };

        typedef Helium::SmartPtr< ProjectFile > ProjectFilePtr;

        class CORE_API ProjectFolder : public Reflect::ConcreteInheritor< ProjectFolder, Reflect::DocumentElement >
        {
        public:
            ProjectFolder()
            {

            }

            ProjectFolder( const tstring& name )
                : m_Name ( name )
            {

            }

            const tstring& GetName()
            {
                return m_Name;
            }

        private:
            tstring m_Name;

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Name, "Name" );
            }
        };

        class CORE_API Project : public Reflect::ConcreteInheritor< Project, Reflect::Document >
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