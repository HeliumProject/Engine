#include "Document.h"
#include "Serializers.h"

using namespace Helium::Reflect;

void DocumentNode::Initialize( Document* document, DocumentElement* parent, DocumentNode* nextSibling, DocumentNode* previousSibling )
{
    SetDocument( document );
    SetParent( parent );
    SetNextSibling( nextSibling );
    SetPreviousSibling( previousSibling );
}

void DocumentElement::Initialize( Document* document, DocumentElement* parent, DocumentNode* nextSibling, DocumentNode* previousSibling )
{
    Base::Initialize( document, parent, nextSibling, previousSibling );

    for ( std::vector< DocumentNodePtr >::const_iterator itr = m_Children.begin()
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

        (*itr)->Initialize( document, this, nextSibling, previousSibling );
    }
}

void DocumentElement::AddChild( DocumentNodePtr node )
{
    if ( std::find( m_Children.begin(), m_Children.end(), node ) == m_Children.end() )
    {
        if ( m_ChildAdding.RaiseWithReturn( node ) )
        {
            DocumentElement* parent = node->GetParent();
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

void DocumentElement::RemoveChild( DocumentNodePtr node )
{
    size_t size = m_Children.size();
    std::remove( m_Children.begin(), m_Children.end(), node );
    if ( m_Children.size() < size )
    {
        if ( m_ChildRemoving.RaiseWithReturn( node ) )
        {
            DocumentNode* nextSibling = node->GetNextSibling();
            DocumentNode* previousSibling = node->GetPreviousSibling();

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
