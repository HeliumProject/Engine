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
		REFLECT_DECLARE_OBJECT(Helium::BulletShape, Reflect::Object);
		//REFLECT_DECLARE_BASE_STRUCTURE(Helium::BulletShape);
		static void PopulateStructure( Reflect::Structure& comp );

		BulletShape();
		
		inline bool operator==( const BulletShape& _rhs ) const { return true; }
		inline bool operator!=( const BulletShape& _rhs ) const { return !( *this == _rhs ); }

		Simd::Vector3 m_Position;
		Simd::Quat m_Rotation;

		float m_Mass;

		//virtual btCollisionShape *CreateShape() const = 0;
		virtual btCollisionShape *CreateShape() const { HELIUM_ASSERT( 0 ); return NULL; } // Must implement because using REFLECT_DECLARE_OBJECT instead of REFLECT_DECLARE_ABSTRACT
	protected:
		void ConfigureShape(btCollisionShape *pShape);
	};
	typedef Helium::StrongPtr<BulletShape> BulletShapePtr;
	
	struct HELIUM_BULLET_API BulletShapeSphere : public Helium::BulletShape
	{
		REFLECT_DECLARE_OBJECT(Helium::BulletShapeSphere, BulletShape);
		//REFLECT_DECLARE_DERIVED_STRUCTURE(Helium::BulletShapeSphere, Helium::BulletShape);
		static void PopulateStructure( Reflect::Structure& comp );

		BulletShapeSphere();
		
		inline bool operator==( const BulletShapeSphere& _rhs ) const { return m_Radius == _rhs.m_Radius; }
		inline bool operator!=( const BulletShapeSphere& _rhs ) const { return !( *this == _rhs ); }
		
		virtual btCollisionShape *CreateShape() const;

		float m_Radius;
	};
	typedef Helium::StrongPtr< BulletShapeSphere > BulletShapeSpherePtr;

	struct HELIUM_BULLET_API BulletShapeBox : public Helium::BulletShape
	{
		REFLECT_DECLARE_OBJECT(Helium::BulletShapeBox, BulletShape);
		//REFLECT_DECLARE_DERIVED_STRUCTURE(Helium::BulletShapeBox, Helium::BulletShape);
		static void PopulateStructure( Reflect::Structure& comp );
		
		BulletShapeBox();

		inline bool operator==( const BulletShapeBox& _rhs ) const { return m_Extents == _rhs.m_Extents; }
		inline bool operator!=( const BulletShapeBox& _rhs ) const { return !( *this == _rhs ); }
		
		virtual btCollisionShape *CreateShape() const;

		Simd::Vector3 m_Extents;
	};
	typedef Helium::StrongPtr< BulletShapeBox > BulletShapeBoxPtr;
}