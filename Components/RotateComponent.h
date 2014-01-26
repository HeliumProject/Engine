
#pragma once

#include "Components/Components.h"
#include "Framework/ComponentDefinition.h"
#include "MathSimd/Quat.h"

namespace Helium
{
	class RotateComponentDefinition;

	class HELIUM_COMPONENTS_API RotateComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::RotateComponent, Helium::Component );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		void Initialize( const RotateComponentDefinition &definition);
		
		void ApplyRotation( class TransformComponent *pTransform );

		float m_Roll;
		float m_Pitch;
		float m_Yaw;
	};

	class HELIUM_COMPONENTS_API RotateComponentDefinition : public Helium::ComponentDefinitionHelper<RotateComponent, RotateComponentDefinition>
	{
		HELIUM_DECLARE_CLASS( Helium::RotateComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		RotateComponentDefinition();

		Simd::Quat m_RotationPerSecond;

		float m_Roll;
		float m_Pitch;
		float m_Yaw;
	};
	typedef StrongPtr<RotateComponentDefinition> RotateComponentDefinitionPtr;
}
