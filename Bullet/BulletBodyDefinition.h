
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletShapes.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"

template <class T>
class btAlignedObjectArray;

namespace Helium
{
	struct BulletShape;
	typedef Helium::StrongPtr<BulletShape> BulletShapePtr;

	struct HELIUM_BULLET_API BulletBodyDefinition : public Reflect::Struct
	{
	public:
		HELIUM_DECLARE_BASE_STRUCT(Helium::BulletBodyDefinition);
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		BulletBodyDefinition();

		Helium::DynamicArray<BulletShapePtr> m_Shapes;
		float m_Restitution;
		float m_LinearDamping;
		float m_AngularDamping;
		bool m_LockPositionX;
		bool m_LockPositionY;
		bool m_LockPositionZ;
		bool m_LockRotationX;
		bool m_LockRotationY;
		bool m_LockRotationZ;
		bool m_IsKinematic;
		bool m_DisableCollisionResponse;
	};
}
