#pragma once

#include "Bullet/Bullet.h"

#include "Framework/SystemDefinition.h"
#include "Framework/FlagSet.h"

namespace Helium
{
	class HELIUM_BULLET_API BulletSystemComponent : public SystemComponent
	{
		HELIUM_DECLARE_ASSET( BulletSystemComponent, SystemComponent )
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		virtual void Initialize() override;
		virtual void Cleanup() override;
		virtual void Destroy() override;

		static const BulletSystemComponent *GetInstance()
		{
			return ms_Instance;
		}

	public:
		FlagSetDefinitionPtr m_BodyFlags;
		static BulletSystemComponent *ms_Instance;
	};
}
