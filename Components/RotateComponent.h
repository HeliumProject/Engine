
#pragma once

#include "Components/Components.h"
#include "Framework/ComponentDefinition.h"

namespace Helium
{
    class RotateComponentDefinition;

	class HELIUM_COMPONENTS_API RotateComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::RotateComponent, Helium::Component );
        static void PopulateComposite( Reflect::Composite& comp );

        void Finalize(Components::IHasComponents &rHasComponents, const RotateComponentDefinition *pDefinition) { }
        
        void Helium::RotateComponent::ApplyRotation( class TransformComponent *pTransform );
	};

	class HELIUM_COMPONENTS_API RotateComponentDefinition : public Helium::ComponentDefinitionHelper<RotateComponent, RotateComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::RotateComponentDefinition, Helium::ComponentDefinition );
        static void PopulateComposite( Reflect::Composite& comp );
	};
    typedef StrongPtr<RotateComponentDefinition> RotateComponentDefinitionPtr;
}