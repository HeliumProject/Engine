#pragma once

#include <vector>
#include <algorithm>

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Reflect
{
    class Document;

    class DocumentNode;
    typedef Nocturnal::SmartPtr< DocumentNode > DocumentNodePtr;

    class DocumentAttribute;
    typedef Nocturnal::SmartPtr< DocumentAttribute > DocumentAttributePtr;

    class DocumentElement;
    typedef Nocturnal::SmartPtr< DocumentElement > DocumentElementPtr;

    class FOUNDATION_API DocumentNode : public Reflect::AbstractInheritor< DocumentNode, Reflect::Element >
    {
    public:
        DocumentNode()
            : m_Document( NULL )
            , m_Parent( NULL )
        {

        }

        Document* GetDocument() const
        {
            return m_Document;
        }

        void SetDocument( Document* document )
        {
            m_Document = document;
        }

        DocumentElement* GetParent() const
        {
            return m_Parent;
        }

        void SetParent( DocumentElement* node )
        {
            m_Parent = node;
        }

    protected:
        Document*           m_Document;
        DocumentElement*    m_Parent;

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {

        }
    };

    class FOUNDATION_API DocumentAttribute : public Reflect::ConcreteInheritor< DocumentAttribute, DocumentNode >
    {
    public:
        virtual const tstring& GetName()
        {
            return m_Name;
        }

        virtual const tstring& GetValue()
        {
            return m_Value;
        }

        virtual void SetValue( const tstring& value )
        {
            m_Value = value;
        }

        template< class T >
        bool Get(T& value)
        {
            tostringstream str( GetValue() );
            str >> value;
            return !str.fail();
        }

        template< class T >
        bool Set(const T& value)
        {
            tistringstream str;
            str << value;
            SetValue( str.str() );
        }

    protected:
        tstring m_Name;
        tstring m_Value;

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {
            comp.AddField( &This::m_Name, "m_Name" );
            comp.AddField( &This::m_Value, "m_Value" );
        }
    };

    typedef Nocturnal::Signature< bool, DocumentElement* > DocumentHierarchyChangingSignature;
    typedef Nocturnal::Signature< void, DocumentElement* > DocumentHierarchyChangedSignature;

    class FOUNDATION_API DocumentElement : public Reflect::ConcreteInheritor< DocumentElement, DocumentNode >
    {
    public:
        DocumentElement()
            : m_Next( NULL )
            , m_Previous( NULL )
        {

        }

        DocumentElement* GetNextSibling() const
        {
            return m_Next;
        }

        void SetNextSibling( DocumentElement* node )
        {
            m_Next = node;
        }

        DocumentElement* GetPreviousSibling() const
        {
            return m_Previous;
        }

        void SetPreviousSibling( DocumentElement* node )
        {
            m_Previous = node;
        }

        std::vector< DocumentElementPtr >& GetChildren()
        {
            return m_Children;
        }

        const std::vector< DocumentElementPtr >& GetChildren() const
        {
            return m_Children;
        }

        template< class T >
        void FindChildren( std::vector< Nocturnal::SmartPtr< T > >& children )
        {
            for ( std::vector< DocumentElementPtr >::const_iterator itr = m_Children.begin()
                , end = m_Children.end()
                ; itr != end
                ; ++itr )
            {
                T* item = Reflect::ObjectCast< T >( *itr );
                if ( item )
                {
                    children.push_back( item );                    
                }
            }
        }

        virtual void Initialize( Document* document, DocumentElement* parent, DocumentElement* nextSibling = NULL, DocumentElement* previousSibling = NULL );

        virtual void AddChild( DocumentElementPtr node );

        DocumentHierarchyChangingSignature::Event GetChildAdding()
        {
            return m_ChildAdding;
        }

        DocumentHierarchyChangedSignature::Event GetChildAdded()
        {
            return m_ChildAdded;
        }

        virtual void RemoveChild( DocumentElementPtr node );

        DocumentHierarchyChangingSignature::Event GetChildRemoving()
        {
            return m_ChildRemoving;
        }

        DocumentHierarchyChangedSignature::Event GetChildRemoved()
        {
            return m_ChildRemoved;
        }

    protected:
        DocumentElement*                            m_Next;
        DocumentElement*                            m_Previous;
        std::vector< DocumentElementPtr >           m_Children;
        DocumentHierarchyChangingSignature::Event   m_ChildAdding;
        DocumentHierarchyChangedSignature::Event    m_ChildAdded;
        DocumentHierarchyChangingSignature::Event   m_ChildRemoving;
        DocumentHierarchyChangedSignature::Event    m_ChildRemoved;

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {
            comp.AddField( &This::m_Children, "m_Children" );
        }
    };

    class FOUNDATION_API Document : public Reflect::ConcreteInheritor< Document, DocumentElement >
    {
    public:
        virtual void PostDeserialize() NOC_OVERRIDE
        {
            Base::PostDeserialize();

            Initialize( this, this, NULL, NULL );
        }

    public:
        static void EnumerateClass( Reflect::Compositor< This >& comp )
        {

        }
    };

    typedef Nocturnal::SmartPtr<Document> DocumentPtr;
}