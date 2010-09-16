#pragma once

#include "Foundation/Undo/Command.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Container/OrderedSet.h"

#include "Core/API.h"

namespace Helium
{
    namespace Core
    {
        class PropertiesGenerator;
        struct EnumerateElementArgs;

        class CORE_API Persistent : public Reflect::Object
        {
        public:
            Persistent(Reflect::Element* data);
            virtual ~Persistent();

            Reflect::Element* GetPackage()
            {
                return m_Package;
            }

            const Reflect::Element* GetPackage() const
            {
                return m_Package;
            }

            template <class T>
            T* GetPackage()
            {
#ifdef _DEBUG
                T* t = Reflect::ObjectCast<T>(m_Package);
                HELIUM_ASSERT( t );
                return t;
#else
                return static_cast<T*>(m_Package.Ptr());
#endif
            }

            template <class T>
            const T* GetPackage() const
            {
#ifdef _DEBUG
                const T* t = Reflect::ObjectCast<T>(m_Package);
                HELIUM_ASSERT( t );
                return t;
#else
                return static_cast<const T*>(m_Package.Ptr());
#endif
            }

            // Retrieve serialzed data for this object into the parameter
            void GetState( Reflect::ElementPtr& state ) const;

            // Restore serialized data from the element for this object
            void SetState( const Reflect::ElementPtr& state );

            // Get undo command for this object's state (uses GetState/SetState above)
            virtual Undo::CommandPtr SnapShot( Reflect::Element* newState = NULL );

            // Pack any application-cached data into the packed data
            virtual void Pack() {}

            // Unpack data from the packed information into the application object
            virtual void Unpack() {}

            // Is this object currently selectable?
            //  Sometimes objects can on a per-instance or per-type basis decided to NOT be selectable
            //  This prototype exposes the capability to HELIUM_OVERRIDE the selection of an object
            virtual bool IsSelectable() const;

            // Get/Set selected state
            virtual bool IsSelected() const;
            virtual void SetSelected(bool);

            // do enumeration of applicable attributes on this object
            virtual void ConnectProperties(EnumerateElementArgs& args);

            // validate a named panel as usable
            virtual bool ValidatePanel(const tstring& name);

        protected:
            bool                m_Selected;
            Reflect::ElementPtr m_Package;

        public:
            REFLECT_DECLARE_ABSTRACT( Persistent, Reflect::Object );
        };

        typedef SmartPtr< Persistent > PersistentPtr;
        typedef OrderedSet< Persistent* > OS_PersistentDumbPtr;
    }
}