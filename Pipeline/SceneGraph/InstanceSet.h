#pragma once

#include "Pipeline/SceneGraph/Viewport.h"
#include "Foundation/Reflect/Object.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Instance;
        typedef std::set< SceneGraph::Instance* > S_InstanceDumbPtr;

        class InstanceSet;

        class InstanceType;

        struct InstanceSetChangeArgs
        {
            SceneGraph::InstanceSet* m_InstanceSet;
            SceneGraph::Instance* m_Instance;

            InstanceSetChangeArgs( SceneGraph::InstanceSet* set, SceneGraph::Instance* entity )
                : m_InstanceSet( set )
                , m_Instance( entity )
            {
            }
        };

        typedef Helium::Signature< const InstanceSetChangeArgs& > InstanceSetChangeSignature;

        class PIPELINE_API InstanceSet : public Reflect::Object
        {
        protected:
            SceneGraph::InstanceType* m_Type;
            S_InstanceDumbPtr m_Instances;

        public:
            REFLECT_DECLARE_ABSTRACT( InstanceSet, Reflect::Object );
            static void InitializeType();
            static void CleanupType();

        public:
            InstanceSet(SceneGraph::InstanceType* type);

            virtual ~InstanceSet();

            SceneGraph::InstanceType* GetInstanceClass();

            virtual void AddInstance(SceneGraph::Instance* i);

            virtual void RemoveInstance(SceneGraph::Instance* i);

            virtual bool ContainsInstance(SceneGraph::Instance* i);

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

        typedef Helium::StrongPtr< SceneGraph::InstanceSet > InstanceSetPtr;
        typedef std::map< tstring, InstanceSetPtr > M_InstanceSetSmartPtr;
    }
}