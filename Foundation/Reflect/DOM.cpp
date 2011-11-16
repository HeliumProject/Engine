#include "FoundationPch.h"
#include "DOM.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::DocumentNode );
REFLECT_DEFINE_OBJECT( Helium::Reflect::DocumentAttribute );
REFLECT_DEFINE_OBJECT( Helium::Reflect::DocumentObject );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Document );

using namespace Helium;
using namespace Helium::Reflect;

void DocumentAttribute::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &This::m_Name, TXT( "Name" ) );
    comp.AddField( &This::m_Value, TXT( "Value" ) );
}

void DocumentObject::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &This::m_Children, TXT( "Children" ) );
}

void DocumentNode::SetDocument( Document* document )
{
    m_Document = document;
}

void DocumentNode::Initialize( Document* document, DocumentObject* parent, DocumentNode* nextSibling, DocumentNode* previousSibling )
{
    if ( this != document )
    {
        SetDocument( document );
    }

    if ( this != parent )
    {
        SetParent( parent );
    }

    if ( this != nextSibling )
    {
        SetNextSibling( nextSibling );
    }

    if ( this != previousSibling )
    {
        SetPreviousSibling( previousSibling );
    }
}

void DocumentObject::SetDocument( Document* document )
{
    if ( m_Document != document )
    {
        if ( m_Document )
        {
            m_Document->ChildAdding().RemoveMethod( m_Document, &DocumentObject::RaiseChildAdding );
            m_Document->ChildAdded().RemoveMethod( m_Document, &DocumentObject::RaiseChildAdded );
            m_Document->ChildRemoving().RemoveMethod( m_Document, &DocumentObject::RaiseChildRemoving );
            m_Document->ChildRemoved().RemoveMethod( m_Document, &DocumentObject::RaiseChildRemoved );
        }

        Base::SetDocument( document );

        if ( m_Document )
        {
            m_Document->ChildAdding().AddMethod( m_Document, &DocumentObject::RaiseChildAdding );
            m_Document->ChildAdded().AddMethod( m_Document, &DocumentObject::RaiseChildAdded );
            m_Document->ChildRemoving().AddMethod( m_Document, &DocumentObject::RaiseChildRemoving );
            m_Document->ChildRemoved().AddMethod( m_Document, &DocumentObject::RaiseChildRemoved );
        }
    }
}

void DocumentObject::Initialize( Document* document, DocumentObject* parent, DocumentNode* nextSibling, DocumentNode* previousSibling )
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

void DocumentObject::AddChild( DocumentNodePtr node )
{
    if ( std::find( m_Children.begin(), m_Children.end(), node ) == m_Children.end() )
    {
        DocumentObject* parent = node->GetParent();

        DocumentHierarchyChangingArgs args ( node, parent, this );
        m_ChildAdding.Raise( args );
        if ( !args.m_Veto )
        {
            if ( parent )
            {
                parent->RemoveChild( node );
            }

            if ( m_Children.size() )
            {
                node->SetPreviousSibling( m_Children.back() );
            }

            node->SetNextSibling( NULL );
            node->SetDocument( m_Document );
            node->SetParent( this );

            m_Children.push_back( node );
            m_ChildAdded.Raise( DocumentHierarchyChangeArgs ( node, parent, this ) );
        }
    }
}

void DocumentObject::RemoveChild( DocumentNodePtr node )
{
    size_t size = m_Children.size();
    std::remove( m_Children.begin(), m_Children.end(), node );
    if ( m_Children.size() < size )
    {
        DocumentObject* parent = node->GetParent();

        DocumentHierarchyChangingArgs args ( node, parent, NULL );
        m_ChildRemoving.Raise( args );
        if ( !args.m_Veto )
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

            node->SetDocument( NULL );
            node->SetParent( NULL );

            m_ChildRemoved.Raise( DocumentHierarchyChangeArgs ( node, parent, NULL ) );
        }
    }
}
