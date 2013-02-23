
#pragma once

#include "Components/Components.h"
#include "Graphics/Mesh.h"
#include "Framework/ComponentDefinition.h"

namespace Helium
{
    struct MeshComponentDefinition;

	class HELIUM_COMPONENTS_API MeshComponent : public Component
	{
    public:
		HELIUM_DECLARE_COMPONENT( Helium::MeshComponent, Helium::Component );
        static void PopulateComposite( Reflect::Composite& comp );

        void Finalize( const Helium::MeshComponentDefinition* pDefinition );

    private:
        StrongPtr<Mesh> m_Mesh;
	};
    
	struct HELIUM_COMPONENTS_API MeshComponentDefinition : public Helium::ComponentDefinitionHelper<MeshComponent, MeshComponentDefinition>
	{
    public:
		HELIUM_DECLARE_ASSET( Helium::MeshComponentDefinition, Helium::ComponentDefinition );
        static void PopulateComposite( Reflect::Composite& comp );
                
        StrongPtr<Mesh> m_Mesh;
	};
    typedef StrongPtr<MeshComponentDefinition> MeshComponentDefinitionPtr;
}

//#include "TransformComponent.inl"
