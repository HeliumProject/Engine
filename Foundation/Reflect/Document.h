#pragma once

#include <vector>
#include <algorithm>

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Reflect
    {
        class Document;

        class DocumentNode;
        typedef Helium::SmartPtr< DocumentNode > DocumentNodePtr;

        class DocumentAttribute;
        typedef Helium::SmartPtr< DocumentAttribute > DocumentAttributePtr;

        class DocumentElement;
        typedef Helium::SmartPtr< DocumentElement > DocumentElementPtr;

        class FOUNDATION_API DocumentNode : public Reflect::AbstractInheritor< DocumentNode, Reflect::Element >
        {
        public:
            DocumentNode()
                : m_Document( NULL )
                , m_Parent( NULL )
                , m_Next( NULL )
                , m_Previous( NULL )
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


            DocumentNode* GetNextSibling() const
            {
                return m_Next;
            }

            void SetNextSibling( DocumentNode* node )
            {
                m_Next = node;
            }

            DocumentNode* GetPreviousSibling() const
            {
                return m_Previous;
            }

            void SetPreviousSibling( DocumentNode* node )
            {
                m_Previous = node;
            }

            virtual void Initialize( Document* document, DocumentElement* parent, DocumentNode* nextSibling = NULL, DocumentNode* previousSibling = NULL );

        protected:
            Document*           m_Document;
            DocumentElement*    m_Parent;
            DocumentNode*       m_Next;
            DocumentNode*       m_Previous;

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
                comp.AddField( &This::m_Name, "Name" );
                comp.AddField( &This::m_Value, "Value" );
            }
        };

        typedef Helium::Signature< bool, DocumentNode* > DocumentHierarchyChangingSignature;
        typedef Helium::Signature< void, DocumentNode* > DocumentHierarchyChangedSignature;

        class FOUNDATION_API DocumentElement : public Reflect::ConcreteInheritor< DocumentElement, DocumentNode >
        {
        public:
            DocumentElement()
            {

            }

            std::vector< DocumentNodePtr >& GetChildren()
            {
                return m_Children;
            }

            const std::vector< DocumentNodePtr >& GetChildren() const
            {
                return m_Children;
            }

            template< class T >
            void FindChildren( std::vector< Helium::SmartPtr< T > >& children )
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

            virtual void Initialize( Document* document, DocumentElement* parent, DocumentNode* nextSibling = NULL, DocumentNode* previousSibling = NULL ) HELIUM_OVERRIDE;

            virtual void AddChild( DocumentNodePtr node );

            DocumentHierarchyChangingSignature::Event GetChildAdding()
            {
                return m_ChildAdding;
            }

            DocumentHierarchyChangedSignature::Event GetChildAdded()
            {
                return m_ChildAdded;
            }

            virtual void RemoveChild( DocumentNodePtr node );

            DocumentHierarchyChangingSignature::Event GetChildRemoving()
            {
                return m_ChildRemoving;
            }

            DocumentHierarchyChangedSignature::Event GetChildRemoved()
            {
                return m_ChildRemoved;
            }

        protected:
            std::vector< DocumentNodePtr >              m_Children;
            DocumentHierarchyChangingSignature::Event   m_ChildAdding;
            DocumentHierarchyChangedSignature::Event    m_ChildAdded;
            DocumentHierarchyChangingSignature::Event   m_ChildRemoving;
            DocumentHierarchyChangedSignature::Event    m_ChildRemoved;

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {
                comp.AddField( &This::m_Children, "Children" );
            }
        };

        class FOUNDATION_API Document : public Reflect::ConcreteInheritor< Document, DocumentElement >
        {
        public:
            virtual void PostDeserialize() HELIUM_OVERRIDE
            {
                Base::PostDeserialize();

                Initialize( this, this, NULL, NULL );
            }

        public:
            static void EnumerateClass( Reflect::Compositor< This >& comp )
            {

            }
        };

        typedef Helium::SmartPtr<Document> DocumentPtr;
    }
}