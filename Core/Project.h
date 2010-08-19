#pragma once

#include "Core/API.h"
#include "Foundation/Automation/Attribute.h"
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
                : m_Path( path )
            {

            }

            Helium::Attribute<Helium::Path>& Path()
            {
                return m_Path;
            }

        private:
            Helium::Attribute<Helium::Path> m_Path;

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

            Helium::Attribute<tstring>& Name()
            {
                return m_Name;
            }

        private:
            Helium::Attribute<tstring>  m_Name;

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Name, "Name" );
            }
        };

        typedef Helium::SmartPtr< ProjectFolder > ProjectFolderPtr;

        class CORE_API Project : public Reflect::ConcreteInheritor< Project, Reflect::Document >
        {
        public:
            Helium::Attribute< Helium::Path >& Path()
            {
                return m_Path;
            }

        private:
            Helium::Attribute< Helium::Path > m_Path;

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Path, "Path", Reflect::FieldFlags::Discard );
            }
        };

        typedef Helium::SmartPtr<Project> ProjectPtr;
    }
}