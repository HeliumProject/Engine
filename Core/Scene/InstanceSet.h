#pragma once

#include "Core/Scene/Viewport.h"
#include "Foundation/Reflect/Object.h"

namespace Helium
{
    namespace Core
    {
        class Instance;
        typedef std::set< Core::Instance* > S_InstanceDumbPtr;

        class InstanceSet;

        class InstanceType;

        struct InstanceSetChangeArgs
        {
            Core::InstanceSet* m_InstanceSet;
            Core::Instance* m_Instance;

            InstanceSetChangeArgs( Core::InstanceSet* set, Core::Instance* entity )
                : m_InstanceSet( set )
                , m_Instance( entity )
            {
            }
        };

        typedef Helium::Signature< const InstanceSetChangeArgs& > InstanceSetChangeSignature;

        class InstanceSet : public Reflect::Object
        {
        protected:
            Core::InstanceType* m_Type;
            S_InstanceDumbPtr m_Instances;

        public:
            REFLECT_DECLARE_ABSTRACT( InstanceSet, Reflect::Object );
            static void InitializeType();
            static void CleanupType();

        public:
            InstanceSet(Core::InstanceType* type);

            virtual ~InstanceSet();

            Core::InstanceType* GetInstanceType();

            virtual void AddInstance(Core::Instance* i);

            virtual void RemoveInstance(Core::Instance* i);

            virtual bool ContainsInstance(Core::Instance* i);

            virtual const S_InstanceDumbPtr& GetInstances() const;

            // Must be overridden in derived classes
            virtual const tstring& GetName() const = 0;

            // 
            // Listeners
            // 

        protected:
            InstanceSetChangeSignature::Event m_InstanceAdded;
        public:
            void AddInstanceAddedListener( const InstanceSetChangeSignature::Delegate& listener )
            {
                m_InstanceAdded.Add( listener );
            }
            void RemoveInstanceAddedListener( const InstanceSetChangeSignature::Delegate& listener )
            {
                m_InstanceAdded.Remove( listener );
            }

        protected:
            InstanceSetChangeSignature::Event m_InstanceRemoved;
        public:
            void AddInstanceRemovedListener( const InstanceSetChangeSignature::Delegate& listener )
            {
                m_InstanceRemoved.Add( listener );
            }
            void RemoveInstanceRemovedListener( const InstanceSetChangeSignature::Delegate& listener )
            {
                m_InstanceRemoved.Remove( listener );
            }
        };

        typedef Helium::SmartPtr< Core::InstanceSet > InstanceSetPtr;
        typedef std::map< tstring, InstanceSetPtr > M_InstanceSetSmartPtr;
    }
}