#pragma once

#include <vector>
#include <algorithm>

#include "Luna/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Luna
{
    //
    // Project: the top level object loaded into Luna
    //  provides a way to organize references to files like scenes
    //

    class ProjectNode : public Reflect::AbstractInheritor< ProjectNode, Reflect::Element >
    {
    public:
        ProjectNode()
            : m_Parent( NULL )
            , m_NextSibling( NULL )
        {

        }

        ProjectNode( class ProjectFolder* parent, ProjectNode* nextSibling = NULL )
            : m_Parent( parent )
            , m_NextSibling( nextSibling )
        {

        }

        class Project* GetProject() const
        {
            return m_Project;
        }

        void SetProject( class Project* project )
        {
            m_Project = project;
        }

        class ProjectFolder* GetParent() const
        {
            return m_Parent;
        }

        void SetParent( class ProjectFolder* node )
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

        ProjectNode* GetPreviousSibling() const
        {
            return m_PreviousSibling;
        }

        void SetPreviousSibling( ProjectNode* node )
        {
            m_PreviousSibling = node;
        }

        virtual void Initialize( Project* project, ProjectFolder* parent, ProjectNode* nextSibling, ProjectNode* previousSibling )
        {
            SetProject( project );
            SetParent( parent );
            SetNextSibling( nextSibling );
            SetPreviousSibling( previousSibling );
        }

    protected:
        Project*        m_Project;
        ProjectFolder*  m_Parent;
        ProjectNode*    m_NextSibling;
        ProjectNode*    m_PreviousSibling;

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

        virtual void Initialize( Project* project, ProjectFolder* parent, ProjectNode* nextSibling, ProjectNode* previousSibling ) NOC_OVERRIDE;

    private:
        Nocturnal::Path m_Path;

    public:
        static void EnumerateClass( Reflect::Compositor< ProjectFile >& comp )
        {
            comp.AddField( &ProjectFile::m_Path, "m_Path" );
        }
    };

    typedef Nocturnal::SmartPtr< ProjectFile > ProjectFilePtr;

    typedef Nocturnal::Signature< bool, ProjectNode* > ProjectHierarchyChangingSignature;
    typedef Nocturnal::Signature< void, ProjectNode* > ProjectHierarchyChangedSignature;

    class ProjectFolder : public Reflect::ConcreteInheritor< ProjectFolder, ProjectNode >
    {
    public:
        virtual void AddChild( ProjectNodePtr node );

        ProjectHierarchyChangingSignature::Event GetChildAdding()
        {
            return m_ChildAdding;
        }

        ProjectHierarchyChangedSignature::Event GetChildAdded()
        {
            return m_ChildAdded;
        }

        virtual void RemoveChild( ProjectNodePtr node );

        ProjectHierarchyChangingSignature::Event GetChildRemoving()
        {
            return m_ChildRemoving;
        }

        ProjectHierarchyChangedSignature::Event GetChildRemoved()
        {
            return m_ChildRemoved;
        }

        virtual void Initialize( Project* project, ProjectFolder* parent, ProjectNode* nextSibling, ProjectNode* previousSibling ) NOC_OVERRIDE;

    private:
        std::vector< ProjectNodePtr >               m_Children;
        ProjectHierarchyChangingSignature::Event    m_ChildAdding;
        ProjectHierarchyChangedSignature::Event     m_ChildAdded;
        ProjectHierarchyChangingSignature::Event    m_ChildRemoving;
        ProjectHierarchyChangedSignature::Event     m_ChildRemoved;

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

        void AddPath( const Nocturnal::Path& path )
        {
            m_Paths.insert( path );
        }

        virtual void PostDeserialize() NOC_OVERRIDE
        {
            Base::PostDeserialize();

            Initialize( this, this, NULL, NULL );
        }

    public:
        static void EnumerateClass( Reflect::Compositor< Project >& comp )
        {

        }

    private:
        std::set< Nocturnal::Path > m_Paths;
    };

    typedef Nocturnal::SmartPtr<Project> ProjectPtr;
}