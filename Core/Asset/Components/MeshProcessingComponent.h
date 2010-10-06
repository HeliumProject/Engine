#pragma once

#include "Core/API.h"
#include "Foundation/Component/Component.h"

#include "Foundation/Automation/Attribute.h"

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

            static void MeshComressionFactorEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( None,   TXT( "None" ),   TXT( "None" ) );
                info->AddElement( Low,    TXT( "Low" ),    TXT( "Low" ) );
                info->AddElement( Medium, TXT( "Medium" ), TXT( "Medium" ) );
                info->AddElement( High,   TXT( "High" ),   TXT( "High" ) );
            }
        }
        typedef MeshCompressionFactors::MeshCompressionFactor MeshCompressionFactor;

        class CORE_API MeshProcessingComponent : public Component::ComponentBase
        {
        public:
            Helium::Attribute< f32 >                   a_ScalingFactor;
            Helium::Attribute< MeshCompressionFactor > a_MeshCompressionFactor;
            Helium::Attribute< bool >                  a_FlipWinding;

        public:

            REFLECT_DECLARE_CLASS( MeshProcessingComponent, Component::ComponentBase );

            static void EnumerateClass( Reflect::Compositor< MeshProcessingComponent >& comp );

        public:
            MeshProcessingComponent()
            {
                a_ScalingFactor.Set( 1.0f );
                a_MeshCompressionFactor.Set( MeshCompressionFactors::None );
                a_FlipWinding.Set( false );
            }

            virtual ~MeshProcessingComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        };

        typedef Helium::SmartPtr< MeshProcessingComponent > MeshProcessingComponentPtr;
    }
}