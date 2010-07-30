#pragma once

#include "Selectable.h"

#include "Application/Undo/Command.h"

namespace Helium
{
    namespace Editor
    {
        //
        // Persistent is a basic application object
        //  It provides:
        //   o Support for the Internal package / Application object pattern
        //   o Support for Serialization through Reflect
        //   o Support for Undo/Redo on a general level using serialization
        //

        class LUNA_CORE_API Persistent : public Selectable
        {
            //
            // Persistent Data is the packed version of this application object
            //  Serialization occurs like this:
            //   API calls Pack() to pack data into the persistent block
            //   API calls GetPackage() to get packed data for this object
            //  Deserialzation occurs like this:
            //   API calls Unpack() to re-initialize application object members from persistent data
            //

        protected:
            Reflect::ElementPtr m_Package;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Persistent, Selectable );
            static void InitializeType();
            static void CleanupType();


            //
            // Constructor
            //

        public:
            Persistent(Reflect::Element* data);
            virtual ~Persistent();


            //
            // We do allow other APIs to access our persistent data, as in step 2 in above
            //

            // Get
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

            // Pack any application-cached data into the packed data
            virtual void Pack()
            {

            }

            // Unpack data from the packed information into the application object
            virtual void Unpack()
            {

            }

            // 
            // Callback for when data is changed on the package
            // 

        protected:
            virtual void PackageChanged( const Reflect::ElementChangeArgs& args );

            //
            // State Functionality (used by undo)
            //

        private:
            // Retrieve serialzed data for this object into the parameter
            void GetState( Reflect::ElementPtr& state ) const;

            // Restore serialized data from the element for this object
            void SetState( const Reflect::ElementPtr& state );

        public:
            // Get undo command for this object's state
            virtual Undo::CommandPtr SnapShot( Reflect::Element* newState = NULL );
        };

        typedef Helium::SmartPtr< Persistent > PersistentPtr;
    }
}