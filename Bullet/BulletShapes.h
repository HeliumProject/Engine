
#include "Bullet/Bullet.h"
#include "Reflect/Structure.h"
#include "Math/Vector3.h"

namespace Helium
{
    class HELIUM_BULLET_API BulletShape
    {
        REFLECT_DECLARE_BASE_STRUCTURE(Helium::BulletShape);
        static void PopulateComposite( Reflect::Composite& comp );
        
        inline bool operator==( const BulletShape& _rhs ) const { return true; }
        inline bool operator!=( const BulletShape& _rhs ) const { return !( *this == _rhs ); }

        // Needed because a template function that instantiates different code based on traits of this type
        // looks broken and I don't want to deal with that tonight.
        float var;
    };
    
    class HELIUM_BULLET_API BulletShapeSphere
    {
        REFLECT_DECLARE_DERIVED_STRUCTURE(Helium::BulletShapeSphere, Helium::BulletShape);
        static void PopulateComposite( Reflect::Composite& comp );
        
        inline bool operator==( const BulletShapeSphere& _rhs ) const { return m_Radius == _rhs.m_Radius; }
        inline bool operator!=( const BulletShapeSphere& _rhs ) const { return !( *this == _rhs ); }

        float m_Radius;
    };

    class HELIUM_BULLET_API BulletShapeBox
    {
        REFLECT_DECLARE_DERIVED_STRUCTURE(Helium::BulletShapeBox, Helium::BulletShape);
        static void PopulateComposite( Reflect::Composite& comp );
        
        inline bool operator==( const BulletShapeBox& _rhs ) const { return m_Extents == _rhs.m_Extents; }
        inline bool operator!=( const BulletShapeBox& _rhs ) const { return !( *this == _rhs ); }

        Helium::Vector3 m_Extents;
    };
}