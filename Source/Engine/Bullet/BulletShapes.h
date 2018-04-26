#pragma once

#include "Bullet/Bullet.h"
#include "Reflect/Object.h"
#include "Math/Vector3.h"


class btCollisionShape;

// TODO: I would prefer that these all be structures. There is no reason to reference count these.
// But while reflect doens't support dynamic arrays of pointers to structs, these will be objects.
namespace Helium
{
	struct HELIUM_BULLET_API BulletShape : public Reflect::Object
	{
		//REFLECT_DECLARE_ABSTRACT(Helium::BulletShape, Reflect::Object); // TODO: Serialization can't read if value is default because abstract makes no default object to compare with
		HELIUM_DECLARE_CLASS(Helium::BulletShape, Reflect::Object);
		//REFLECT_DECLARE_BASE_STRUCT(Helium::BulletShape);
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		BulletShape();
		
		inline bool operator==( const BulletShape& _rhs ) const { return true; }
		inline bool operator!=( const BulletShape& _rhs ) const { return !( *this == _rhs ); }

		Simd::Vector3 m_Position;
		Simd::Quat m_Rotation;

		float m_Mass;

		//virtual btCollisionShape *CreateShape() const = 0;
		virtual btCollisionShape *CreateShape() const { HELIUM_ASSERT( 0 ); return NULL; } // Must implement because using HELIUM_DECLARE_CLASS instead of HELIUM_DECLARE_ABSTRACT
	protected:
		void ConfigureShape(btCollisionShape *pShape);
	};
	typedef Helium::StrongPtr<BulletShape> BulletShapePtr;
	
	struct HELIUM_BULLET_API BulletShapeSphere : public Helium::BulletShape
	{
		HELIUM_DECLARE_CLASS(Helium::BulletShapeSphere, BulletShape);
		//REFLECT_DECLARE_DERIVED_STRUCT(Helium::BulletShapeSphere, Helium::BulletShape);
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		BulletShapeSphere();
		
		inline bool operator==( const BulletShapeSphere& _rhs ) const { return m_Radius == _rhs.m_Radius; }
		inline bool operator!=( const BulletShapeSphere& _rhs ) const { return !( *this == _rhs ); }
		
		virtual btCollisionShape *CreateShape() const override;

		float m_Radius;
	};
	typedef Helium::StrongPtr< BulletShapeSphere > BulletShapeSpherePtr;

	struct HELIUM_BULLET_API BulletShapeBox : public Helium::BulletShape
	{
		HELIUM_DECLARE_CLASS(Helium::BulletShapeBox, BulletShape);
		//REFLECT_DECLARE_DERIVED_STRUCT(Helium::BulletShapeBox, Helium::BulletShape);
		static void PopulateMetaType( Reflect::MetaStruct& comp );
		
		BulletShapeBox();

		inline bool operator==( const BulletShapeBox& _rhs ) const { return m_Extents == _rhs.m_Extents; }
		inline bool operator!=( const BulletShapeBox& _rhs ) const { return !( *this == _rhs ); }
		
		virtual btCollisionShape *CreateShape() const override;

		Simd::Vector3 m_Extents;
	};
	typedef Helium::StrongPtr< BulletShapeBox > BulletShapeBoxPtr;
}