#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

#include "Foundation/Automation/Attribute.h"

namespace Helium
{
    namespace Asset
    {
        class MeshCompressionFactor
        {
        public:
            enum Enum
            {
                None,
                Low,
                Medium,
                High
            };

            REFLECT_DECLARE_ENUMERATION( MeshCompressionFactor );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( None,   TXT( "None" ) );
                info.AddElement( Low,    TXT( "Low" ) );
                info.AddElement( Medium, TXT( "Medium" ) );
                info.AddElement( High,   TXT( "High" ) );
            }
        };

        class PIPELINE_API MeshProcessingComponent : public Component::ComponentBase
        {
        public:
            Helium::Attribute< float32_t >              a_ScalingFactor;
            Helium::Attribute< MeshCompressionFactor >  a_MeshCompressionFactor;
            Helium::Attribute< bool >                   a_FlipWinding;

        public:

            REFLECT_DECLARE_CLASS( MeshProcessingComponent, Component::ComponentBase );

            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        public:
            MeshProcessingComponent()
            {
                a_ScalingFactor.Set( 1.0f );
                a_MeshCompressionFactor.Set( MeshCompressionFactor::None );
                a_FlipWinding.Set( false );
            }

            virtual ~MeshProcessingComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        };

        typedef Helium::StrongPtr< MeshProcessingComponent > MeshProcessingComponentPtr;
    }
}