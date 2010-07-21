#include "Precompile.h"
#include "Project.h"

#include "Foundation/Reflect/Serializers.h"

using namespace Luna;

void ProjectFile::Initialize( Project* project, ProjectFolder* parent, ProjectNode* nextSibling, ProjectNode* previousSibling )
{
    Base::Initialize( project, parent, nextSibling, previousSibling );

    m_Project->AddPath( m_Path );
}

void ProjectFolder::AddChild( ProjectNodePtr node )
{
    if ( std::find( m_Children.begin(), m_Children.end(), node ) == m_Children.end() )
    {
        if ( m_ChildAdding.RaiseWithReturn( node ) )
        {
            ProjectFolder* parent = node->GetParent();
            if ( parent )
            {
                parent->RemoveChild( node );
            }

            if ( m_Children.size() )
            {
                node->SetPreviousSibling( m_Children.back() );
            }

            node->SetNextSibling( NULL );
            node->SetParent( this );

            m_Children.push_back( node );
            m_ChildAdded.Raise( node );
        }
    }
}

void ProjectFolder::RemoveChild( ProjectNodePtr node )
{
    size_t size = m_Children.size();
    std::remove( m_Children.begin(), m_Children.end(), node );
    if ( m_Children.size() < size )
    {
        if ( m_ChildRemoving.RaiseWithReturn( node ) )
        {
            ProjectNode* nextSibling = node->GetNextSibling();
            ProjectNode* previousSibling = node->GetPreviousSibling();

            if ( nextSibling )
            {
                node->SetNextSibling( NULL );

                if ( previousSibling )
                {
                    nextSibling->SetPreviousSibling( previousSibling );
                }
            }

            if ( previousSibling )
            {
                node->SetPreviousSibling( NULL );

                if ( nextSibling )
                {
                    previousSibling->SetNextSibling( nextSibling );
                }
            }

            node->SetParent( NULL );

            m_ChildRemoved.Raise( node );
        }
    }
}

void ProjectFolder::Initialize( Project* project, ProjectFolder* parent, ProjectNode* nextSibling, ProjectNode* previousSibling )
{
    Base::Initialize( project, parent, nextSibling, previousSibling );

    for ( std::vector< ProjectNodePtr >::const_iterator itr = m_Children.begin()
        , begin = m_Children.begin()
        , end = m_Children.end()
        ; itr != end
        ; ++itr )
    {
        if ( itr+1 != end )
        {
            nextSibling = *(itr+1);
        }
        else
        {
            nextSibling = NULL;
        }

        if ( itr-1 != begin )
        {
            previousSibling = *(itr-1);
        }
        else
        {
            previousSibling = NULL;
        }

        (*itr)->Initialize( project, this, nextSibling, previousSibling );
    }
}