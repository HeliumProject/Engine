
#pragma once

#include "Framework/Framework.h"
#include "Framework/Components.h"
#include "Framework/Entity.h"

namespace Helium
{
	class HELIUM_FRAMEWORK_API EntityComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( EntityComponent, Component );

		Entity *GetEntity() { return static_cast<Entity *>(GetOwner()); }
	};
}

#include "Framework/EntityComponent.inl"