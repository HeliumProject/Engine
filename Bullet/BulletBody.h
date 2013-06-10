
#include "Bullet/Bullet.h"
#include "Engine/Asset.h"
#include "Math/Vector3.h"
#include "Foundation/DynamicArray.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btRigidBody;
struct btDefaultMotionState;

namespace Helium
{
	class BulletWorld;
	struct BulletBodyDefinition;
	struct BulletMotionState;

	// Intended as a lightweight wrapper around a bullet body. If you need a reference counted body, make your own.
	// By keeping it light, we can get this into components without bloating it.
	class HELIUM_BULLET_API BulletBody
	{
	public:
		BulletBody();
		~BulletBody();

		bool HasBody() { return m_Body != NULL; }
		btRigidBody *GetBody() { return m_Body; }

		void Initialize( 
			BulletWorld &rWorld,
			const BulletBodyDefinition &rBodyDefinition, 
			const Helium::Simd::Vector3 &rInitialPosition, 
			const Helium::Simd::Quat &rInitialRotation );

		void Destruct(BulletWorld &rWorld);

		void GetPosition(Helium::Simd::Vector3 &rPosition);
		void GetRotation(Helium::Simd::Quat &rRotation);

		void SetPosition(const Helium::Simd::Vector3 &rPosition);
		void SetRotation(const Helium::Simd::Quat &rRotation);
		
	private:
		DynamicArray<btCollisionShape *> m_Shapes;
		btRigidBody *m_Body;
		BulletMotionState *m_MotionState;
	};
}
