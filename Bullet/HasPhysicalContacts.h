
#pragma once

#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"

namespace Helium
{
	struct ContactInfo
	{
		Simd::Vector3 m_OurPosition;
		Simd::Vector3 m_TheirPosition;
		Simd::Vector3 m_Normal;
		Entity *m_pEntity;
	};

	struct HELIUM_BULLET_API HasPhysicalContactsComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::HasPhysicalContactsComponent, Helium::Component );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		~HasPhysicalContactsComponent();

		// TODO: There's a lot of dynamic allocation here.. maybe there's a clever way we could just have a wad of Entity* or some shorter
		// handle used as a ringbuffer? BeginFrameTouching, then intermediate ticks, then EndFrameTouching? EndFrameTouching would be 
		// BeginFrameTouching on next frame.
		DynamicArray<Entity *> m_BeginTouch;
		DynamicArray<Entity *> m_EndTouch;

		// Assuming that using fields, these will be small lists
		Set<Entity *> m_BeginFrameTouching;
		Set<Entity *> m_EndFrameTouching;
		Set<Entity *> m_EverTouchedThisFrame;
	};
}
