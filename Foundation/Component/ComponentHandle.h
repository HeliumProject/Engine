#pragma once

#include "Component.h"
#include "ComponentCollection.h"
#include "ComponentExceptions.h"

namespace Helium
{
    namespace Component
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  ComponentHandle - Base class
        //
        /////////////////////////////////////////////////////////////////////////////////////////////////
        template<class ComponentType, class CollectionType>
        class ComponentHandle
        {
        protected:
            CollectionType* m_Collection;
            Helium::SmartPtr<ComponentType> m_Component;

        public:
            ComponentHandle( CollectionType* collection )
                : m_Collection ( collection )
            {

            }

            ComponentHandle( CollectionType* collection, const Helium::SmartPtr<ComponentType>& attribute )
                : m_Collection ( collection )
                , m_Component ( attribute )
            {

            }

            bool Valid()
            {
                return m_Component.ReferencesObject() && m_Component->m_IsEnabled;
            }

            ComponentType* operator -> ()
            {
                if (!Valid())
                {
                    const tchar_t* attributeTypeName = TXT("Unknown");
                    const Reflect::Class* attributeClass = Reflect::GetClass< ComponentType >();
                    if ( attributeClass )
                    {
                        attributeTypeName = attributeClass->m_Name.c_str();
                    }

                    if (m_Component.ReferencesObject())
                    {
                        throw DisabledComponentException( attributeTypeName );
                    }
                    else
                    {
                        throw MissingComponentException( attributeTypeName );
                    }
                }

                return m_Component.Ptr();
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  ComponentEditor
        //
        /////////////////////////////////////////////////////////////////////////////////////////////////
        template<class ComponentType>
        class ComponentEditor : public ComponentHandle<ComponentType, ComponentCollection>
        {
        public:
            ComponentEditor( ComponentCollection* collection )
                : ComponentHandle ( collection )
            {
                // downcast attribute from the collection, if it exists
                Helium::SmartPtr<ComponentType> attr = Reflect::SafeCast<ComponentType>( m_Collection->GetComponent( Reflect::GetType<ComponentType>() ) );

                // if it exists
                if (attr.ReferencesObject())
                {
                    // if its already in another collection (the override case)
                    if ( attr->GetCollection() && attr->GetCollection() != collection )
                    {
                        // clone the attribute from the other collection for use in this collection
                        m_Component = Reflect::ThrowCast<ComponentType>( attr->Clone() );
                    }
                    else
                    {
                        // no need to type check again in release
                        m_Component = Reflect::ThrowCast<ComponentType>( attr );
                    }
                }
                else
                {
                    // no existing attribute, create a new one
                    m_Component = Reflect::ThrowCast<ComponentType>( Reflect::Registry::GetInstance()->CreateInstance( Reflect::GetType<ComponentType>() ) );
                }

                // ensure its part of the collection
                m_Collection->SetComponent( m_Component );
            }

            ComponentEditor( ComponentCollection* collection, const Helium::SmartPtr<ComponentType>& attribute )
                : ComponentHandle ( collection, attribute )
            {

            }

            void Commit()
            {
                // this is fishy, but for completeness
                m_Component->m_IsEnabled = true;

                // Fire change event
                m_Component->RaiseChanged();
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  ComponentViewer
        //
        /////////////////////////////////////////////////////////////////////////////////////////////////
        template<class ComponentType>
        class ComponentViewer : public ComponentHandle<ComponentType, const ComponentCollection>
        {
        public:
            ComponentViewer()
                : ComponentHandle ( NULL )
            {

            }

            ComponentViewer( const ComponentCollection* collection, bool useDefault = false )
                : ComponentHandle ( collection )
            {
                View( collection, useDefault );
            }

            void View( const ComponentCollection* collection, bool useDefault = false )
            {
                m_Collection = collection;

                m_Component = Reflect::SafeCast<ComponentType>( m_Collection->GetComponent( Reflect::GetType<ComponentType>() ) );

                if ( ( !m_Component.ReferencesObject() || !m_Component->m_IsEnabled ) && useDefault )
                {
                    m_Component = new ComponentType();
                }
            }
        };
    }
}