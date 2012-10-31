#pragma once

#include <vector>
#include <algorithm>

#include "Foundation/API.h"
#include "Foundation/SmartPtr.h"
#include "Reflect/Object.h"
#include "Reflect/Data/DataDeduction.h"
#include "Foundation/FilePath.h"

namespace Helium
{
    namespace Reflect
    {
        class Document;

        class DocumentNode;
        typedef Helium::StrongPtr< DocumentNode > DocumentNodePtr;

        class DocumentAttribute;
        typedef Helium::StrongPtr< DocumentAttribute > DocumentAttributePtr;

        class DocumentObject;
        typedef Helium::StrongPtr< DocumentObject > DocumentObjectPtr;

        class HELIUM_REFLECT_API DocumentNode : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_ABSTRACT( DocumentNode, Object );

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

            virtual void SetDocument( Document* document );

            DocumentObject* GetParent() const
            {
                return m_Parent;
            }

            void SetParent( DocumentObject* node )
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

            virtual void Initialize( Document* document, DocumentObject* parent, DocumentNode* nextSibling = NULL, DocumentNode* previousSibling = NULL );

        protected:
            Document*           m_Document;
            DocumentObject*    m_Parent;
            DocumentNode*       m_Next;
            DocumentNode*       m_Previous;
        };

        class HELIUM_REFLECT_API DocumentAttribute : public DocumentNode
        {
        public:
            REFLECT_DECLARE_OBJECT( DocumentAttribute, DocumentNode );
            static void PopulateComposite( Reflect::Composite& comp );

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
        };

        struct DocumentHierarchyChangeArgs
        {
            DocumentHierarchyChangeArgs( DocumentNode* node, DocumentObject* oldParent, DocumentObject* newParent )
                : m_Node( node )
                , m_OldParent( oldParent )
                , m_NewParent( newParent )
            {

            }

            DocumentNode*       m_Node;
            DocumentObject*    m_OldParent;
            DocumentObject*    m_NewParent;
        };

        struct DocumentHierarchyChangingArgs : public DocumentHierarchyChangeArgs
        {
            DocumentHierarchyChangingArgs( DocumentNode* node, DocumentObject* oldParent, DocumentObject* newParent )
                : DocumentHierarchyChangeArgs( node, oldParent, newParent )
                , m_Veto( false )
            {

            }

            mutable bool m_Veto;
        };

        typedef Helium::Signature< const DocumentHierarchyChangingArgs& > DocumentHierarchyChangingSignature;
        typedef Helium::Signature< const DocumentHierarchyChangeArgs& > DocumentHierarchyChangedSignature;

        class HELIUM_REFLECT_API DocumentObject : public DocumentNode
        {
        public:
            REFLECT_DECLARE_OBJECT( DocumentObject, DocumentNode );
            static void PopulateComposite( Reflect::Composite& comp );

            DocumentObject()
            {

            }

            virtual void SetDocument( Document* document ) HELIUM_OVERRIDE;

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
                for ( std::vector< DocumentObjectPtr >::const_iterator itr = m_Children.begin()
                    , end = m_Children.end()
                    ; itr != end
                    ; ++itr )
                {
                    T* item = Reflect::SafeCast< T >( *itr );
                    if ( item )
                    {
                        children.push_back( item );                    
                    }
                }
            }

            virtual void Initialize( Document* document, DocumentObject* parent, DocumentNode* nextSibling = NULL, DocumentNode* previousSibling = NULL ) HELIUM_OVERRIDE;

            virtual void AddChild( DocumentNodePtr node );

            virtual void RemoveChild( DocumentNodePtr node );

            DocumentHierarchyChangingSignature::Event& ChildAdding()
            {
                return m_ChildAdding;
            }

            DocumentHierarchyChangedSignature::Event& ChildAdded()
            {
                return m_ChildAdded;
            }

            DocumentHierarchyChangingSignature::Event& ChildRemoving()
            {
                return m_ChildRemoving;
            }

            DocumentHierarchyChangedSignature::Event& ChildRemoved()
            {
                return m_ChildRemoved;
            }

            void RaiseChildAdding( const DocumentHierarchyChangingArgs& args )
            {
                m_ChildAdding.Raise( args );
            }

            void RaiseChildAdded( const DocumentHierarchyChangeArgs& args )
            {
                m_ChildAdded.Raise( args );
            }

            void RaiseChildRemoving( const DocumentHierarchyChangingArgs& args )
            {
                m_ChildRemoving.Raise( args );
            }

            void RaiseChildRemoved( const DocumentHierarchyChangeArgs& args )
            {
                m_ChildRemoved.Raise( args );
            }

        protected:
            std::vector< DocumentNodePtr >              m_Children;
            DocumentHierarchyChangingSignature::Event   m_ChildAdding;
            DocumentHierarchyChangedSignature::Event    m_ChildAdded;
            DocumentHierarchyChangingSignature::Event   m_ChildRemoving;
            DocumentHierarchyChangedSignature::Event    m_ChildRemoved;
        };

        class HELIUM_REFLECT_API Document : public DocumentObject
        {
        public:
            REFLECT_DECLARE_OBJECT( Document, DocumentObject );

            void Initialize()
            {
                Base::Initialize( this, this, NULL, NULL );
            }
        };

        typedef Helium::SmartPtr<Document> DocumentPtr;
    }
}