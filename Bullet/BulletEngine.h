
#include "Bullet/Bullet.h"

#include "Framework/SystemDefinition.h"
#include "Framework/FlagSet.h"

namespace Helium
{
	namespace Bullet
	{
		HELIUM_BULLET_API void Initialize();
		HELIUM_BULLET_API void Cleanup();
	}

	class HELIUM_BULLET_API BulletSystemComponent : public SystemComponent
	{
		HELIUM_DECLARE_ASSET( BulletSystemComponent, SystemComponent )
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		virtual void Initialize();
		virtual void FinalizeInit();
		virtual void Cleanup();
		virtual void Destroy();

		static const BulletSystemComponent *GetStaticInstance()
		{
			return ms_Instance;
		}

	public:
		FlagSetDefinitionPtr m_BodyFlags;
		static BulletSystemComponent *ms_Instance;
	};
}
