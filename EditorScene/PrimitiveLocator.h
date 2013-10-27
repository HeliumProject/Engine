#pragma once

#include "Primitive.h"

namespace Helium
{
	namespace SceneGraph
	{
		class PrimitiveLocator : public PrimitiveTemplate< Helium::SimpleVertex >
		{
		public:
			typedef PrimitiveTemplate< Helium::SimpleVertex > Base;

			float m_Length;

		public:
			PrimitiveLocator();

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				Helium::BufferedDrawer* drawInterface, DrawArgs* args, Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY, const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
		};
	}
}