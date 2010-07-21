#pragma once

#include <vector>
#include <algorithm>

#include "Luna/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Luna
{
    typedef Nocturnal::Signature< void, class ProjectNode* > ProjectNodeSignature;

    //
    // Project: the top level object loaded into Luna
    //  provides a way to organize references to files like scenes
    //

    class ProjectNode : public Reflect::AbstractInheritor< ProjectNode, Reflect::Element >
    {
    public:
        ProjectNode()
            : m_Parent ( NULL )
            , m_NextSibling ( NULL )
        {

        }

        ProjectNode( ProjectNode* parent, ProjectNode* sibling = NULL )
            : m_Parent( parent )
            , m_NextSibling ( sibling )
        {

        }

        ProjectNode* GetParent() const
        {
            return m_Parent;
        }

        void SetParent( ProjectNode* node )
        {
            m_Parent = node;
        }

        ProjectNode* GetNextSibling() const
        {
            return m_NextSibling;
        }

        void SetNextSibling( ProjectNode* node )
        {
            m_NextSibling = node;
        }

        virtual void Initialize( ProjectNode* parent, ProjectNode* sibling )
        {
            SetParent( parent );
            SetNextSibling( sibling );
        }

    private:
        ProjectNode* m_Parent;
        ProjectNode* m_NextSibling;

    public:
        static void EnumerateClass( Reflect::Compositor< ProjectNode >& comp )
        {

        }
    };

    typedef Nocturnal::SmartPtr< ProjectNode > ProjectNodePtr;

    class ProjectFile : public Reflect::ConcreteInheritor< ProjectFile, ProjectNode >
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
        static void EnumerateClass( Reflect::Compositor< ProjectFile >& comp )
        {
            comp.AddField( &ProjectFile::m_Path, "m_Path" );
        }
    };

    typedef Nocturnal::SmartPtr< ProjectFile > ProjectFilePtr;

    class ProjectFolder : public Reflect::ConcreteInheritor< ProjectFolder, ProjectNode >
    {
    public:
        void AddChild( Nocturnal::SmartPtr< ProjectNode > node )
        {
            if ( std::find( m_Children.begin(), m_Children.end(), node ) == m_Children.end() )
            {
                m_Children.push_back( node );
                m_ChildAdded.Raise( node );
            }
        }

        ProjectNodeSignature::Event GetChildAdded()
        {
            return m_ChildAdded;
        }

        void RemoveChild( Nocturnal::SmartPtr< ProjectNode > node )
        {
            size_t size = m_Children.size();
            std::remove( m_Children.begin(), m_Children.end(), node );
            if ( m_Children.size() < size )
            {
                m_ChildRemoved.Raise( node );
            }
        }

        ProjectNodeSignature::Event GetChildRemoved()
        {
            return m_ChildRemoved;
        }

        virtual void Initialize( ProjectNode* parent, ProjectNode* sibling ) NOC_OVERRIDE
        {
            Base::Initialize( parent, sibling );

            for ( std::vector< ProjectNodePtr >::const_iterator itr = m_Children.begin()
                , end = m_Children.end()
                ; itr != end
                ; ++itr )
            {
                if ( itr+1 != end )
                {
                    sibling = *(itr+1);
                }
                else
                {
                    sibling = NULL;
                }

                (*itr)->Initialize( this, sibling );
            }
        }

    private:
        std::vector< ProjectNodePtr >   m_Children;
        ProjectNodeSignature::Event     m_ChildAdded;
        ProjectNodeSignature::Event     m_ChildRemoved;

    public:
        static void EnumerateClass( Reflect::Compositor< ProjectFolder >& comp )
        {
            comp.AddField( &ProjectFolder::m_Children, "m_Children" );
        }
    };

    typedef Nocturnal::SmartPtr< ProjectFolder > ProjectFolderPtr;

    class Project : public Reflect::ConcreteInheritor< Project, ProjectFolder >
    {
    public:
        Project();

        virtual void PostDeserialize() NOC_OVERRIDE
        {
            Base::PostDeserialize();

            Initialize( this, NULL );
        }

    public:
        static void EnumerateClass( Reflect::Compositor< Project >& comp )
        {

        }
    };

    typedef Nocturnal::SmartPtr<Project> ProjectPtr;
}