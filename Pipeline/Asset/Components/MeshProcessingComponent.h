#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        namespace MeshCompressionFactors
        {
            enum MeshCompressionFactor
            {
                None,
                Low,
                Medium,
                High
            };

            static void MeshComressionFactorEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement( None,   TXT( "None" ),   TXT( "None" ) );
                info->AddElement( Low,    TXT( "Low" ),    TXT( "Low" ) );
                info->AddElement( Medium, TXT( "Medium" ), TXT( "Medium" ) );
                info->AddElement( High,   TXT( "High" ),   TXT( "High" ) );
            }
        }
        typedef MeshCompressionFactors::MeshCompressionFactor MeshCompressionFactor;

        class PIPELINE_API MeshProcessingComponent : public Component::ComponentBase
        {
        private:
            f32                   m_ScalingFactor;
            MeshCompressionFactor m_MeshCompressionFactor;


        public:

            REFLECT_DECLARE_CLASS( MeshProcessingComponent, Component::ComponentBase );

            static void EnumerateClass( Reflect::Compositor< MeshProcessingComponent >& comp );

        public:
            MeshProcessingComponent()
                : m_ScalingFactor( 1.0f )
                , m_MeshCompressionFactor( MeshCompressionFactors::None )
            {
            }

            virtual ~MeshProcessingComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        public:

            f32 GetScalingFactor() const
            {
                return m_ScalingFactor;
            }
            void SetScalingFactor( const f32 scalingFactor )
            {
                m_ScalingFactor = scalingFactor;
            }

            MeshCompressionFactor GetMeshCompressionFactor() const
            {
                return m_MeshCompressionFactor;
            }
            void SetMeshCompressionFactor( const MeshCompressionFactor& meshCompressionFactor )
            {
                m_MeshCompressionFactor = meshCompressionFactor;
            }

        };
    }
}