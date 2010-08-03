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
                : m_PathAttr( m_Path )
            {
                AddChangedListeners();
            }

            ProjectFile( const Helium::Path& path )
                : m_Path( path )
                , m_PathAttr( m_Path )
            {
                AddChangedListeners();
            }

            Helium::Attribute<Helium::Path>& Path()
            {
                return m_PathAttr;
            }

        private:
            Helium::Path                    m_Path;
            Helium::Attribute<Helium::Path> m_PathAttr;

            void AddChangedListeners()
            {
                m_PathAttr.Changed().AddMethod( this, &Element::AttributeChanged<Helium::Path> );
            }

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
                : m_NameAttr( m_Name )
            {

            }

            ProjectFolder( const tstring& name )
                : m_Name ( name )
                , m_NameAttr( m_Name )
            {

            }

            Helium::Attribute<tstring>& Name()
            {
                return m_NameAttr;
            }

        private:
            tstring                     m_Name;
            Helium::Attribute<tstring>  m_NameAttr;

            void AddChangedListeners()
            {
                m_NameAttr.Changed().AddMethod( this, &Element::AttributeChanged<tstring> );
            }

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
            Project()
                : m_PathAttr( m_Path )
            {
                AddChangedListeners();
            }

            Helium::Attribute<tstring>& Path()
            {
                return m_PathAttr;
            }

        private:
            tstring                     m_Path;
            Helium::Attribute<tstring>  m_PathAttr;

            void AddChangedListeners()
            {
                m_PathAttr.Changed().AddMethod( this, &Element::AttributeChanged<tstring> );
            }

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Path, "Path", Reflect::FieldFlags::Discard );
            }
        };

        typedef Helium::SmartPtr<Project> ProjectPtr;
    }
}