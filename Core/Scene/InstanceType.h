#pragma once

#include "InstanceSet.h"
#include "HierarchyNodeType.h"

namespace Helium
{
    namespace Core
    {
        class Primitive;
        class PrimitivePointer;

        class InstanceType;

        struct InstanceTypeChangeArgs
        {
            Core::InstanceType* m_InstanceType;
            Core::InstanceSet* m_InstanceSet;

            InstanceTypeChangeArgs( Core::InstanceType* type, Core::InstanceSet* set )
                : m_InstanceType( type )
                , m_InstanceSet( set )
            {
            }
        };

        typedef Helium::Signature< const InstanceTypeChangeArgs& > InstanceTypeChangeSignature;

        class InstanceType : public Core::HierarchyNodeType
        {
            //
            // Members
            //

        protected:
            // material that describes the color settings of the configuration
            D3DMATERIAL9 m_Material;

            // shared ui pointer is the basic visualization for all instances
            Core::PrimitivePointer* m_Pointer;

            // the sets for this type
            M_InstanceSetSmartPtr m_Sets;


            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::InstanceType, Core::HierarchyNodeType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            InstanceType( Core::Scene* scene, i32 instanceType );
            virtual ~InstanceType();


            //
            // Graphics
            //

            const Core::PrimitivePointer* GetPointer() const
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

            virtual void AddSet(Core::InstanceSet* set);
            virtual void RemoveSet(Core::InstanceSet* set);


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