#pragma once

#include "InstanceSet.h"
#include "HierarchyNodeType.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Primitive;
        class PrimitivePointer;

        class InstanceType;

        struct InstanceTypeChangeArgs
        {
            SceneGraph::InstanceType* m_InstanceType;
            SceneGraph::InstanceSet* m_InstanceSet;

            InstanceTypeChangeArgs( SceneGraph::InstanceType* type, SceneGraph::InstanceSet* set )
                : m_InstanceType( type )
                , m_InstanceSet( set )
            {
            }
        };

        typedef Helium::Signature< const InstanceTypeChangeArgs& > InstanceTypeChangeSignature;

        class PIPELINE_API InstanceType : public SceneGraph::HierarchyNodeType
        {
        protected:
            // material that describes the color settings of the configuration
            D3DMATERIAL9 m_Material;

            // shared ui pointer is the basic visualization for all instances
            SceneGraph::PrimitivePointer* m_Pointer;

            // the sets for this type
            M_InstanceSetSmartPtr m_Sets;

        public:
            REFLECT_DECLARE_ABSTRACT( SceneGraph::InstanceType, SceneGraph::HierarchyNodeType );
            static void InitializeType();
            static void CleanupType();

        public:
            InstanceType( SceneGraph::Scene* scene, int32_t instanceType );
            virtual ~InstanceType();

            //
            // Graphics
            //

            const SceneGraph::PrimitivePointer* GetPointer() const
            {
                return m_Pointer;
            }

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            bool IsTransparent() HELIUM_OVERRIDE;
            const D3DMATERIAL9& GetMaterial() const;

            //
            // Sets
            //

            const M_InstanceSetSmartPtr& GetSets() const
            {
                return m_Sets;
            }

            virtual void AddSet(SceneGraph::InstanceSet* set);
            virtual void RemoveSet(SceneGraph::InstanceSet* set);

            //
            // Events
            //

        protected:
            InstanceTypeChangeSignature::Event m_SetAdded;
        public:
            void AddSetAddedListener( const InstanceTypeChangeSignature::Delegate& listener )
            {
                m_SetAdded.Add( listener );
            }
            void RemoveSetAddedListener( const InstanceTypeChangeSignature::Delegate& listener )
            {
                m_SetAdded.Remove( listener );
            }

        protected:
            InstanceTypeChangeSignature::Event m_SetRemoved;
        public:
            void AddSetRemovedListener( const InstanceTypeChangeSignature::Delegate& listener )
            {
                m_SetRemoved.Add( listener );
            }
            void RemoveSetRemovedListener( const InstanceTypeChangeSignature::Delegate& listener )
            {
                m_SetRemoved.Remove( listener );
            }
        };
    }
}