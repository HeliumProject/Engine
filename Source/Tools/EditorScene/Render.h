#pragma once

#include "Platform/Types.h"

#include "Foundation/Event.h" 
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

#include "Rendering/Renderer.h"

#include "GraphicsTypes/VertexTypes.h"

#include "EditorScene/Pick.h"
#include "EditorScene/Visitor.h"

namespace Helium
{
	class BufferedDrawer;
}

namespace Helium
{
	namespace Editor
	{
		class Camera;
		class Viewport;

		namespace IndexElementTypes
		{
			enum IndexElementType
			{
				Unknown,
				Unsigned16,
				Unsigned32,
				Count,
			};
		}

		typedef IndexElementTypes::IndexElementType IndexElementType;

		namespace VertexElementTypes
		{
			enum VertexElementType
			{
				Unknown,
				SimpleVertex,
				ScreenVertex,
				StaticMeshVertex,
				SimpleTexturedVertex,
				Count,
			};
		}

		typedef VertexElementTypes::VertexElementType VertexElementType;

		static uint32_t IndexElementSizes[] =
		{
			0x0,                // Unknown
			sizeof( uint16_t ), // Unsigned16
			sizeof( uint32_t ), // Unsigned32
		};

		HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( IndexElementSizes ) == IndexElementTypes::Count );

		static uint32_t VertexElementSizes[] =
		{
			0x0,                                    // Unknown
			sizeof( Helium::SimpleVertex ),          // SimpleVertex
			sizeof( Helium::ScreenVertex ),          // ScreenVertex
			sizeof( Helium::StaticMeshVertex< 1 > ), // StaticMeshVertex
			sizeof( Helium::SimpleTexturedVertex ),  // SimpleTexturedVertex
		};

		HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( VertexElementSizes ) == VertexElementTypes::Count );

		static Helium::ERendererIndexFormat IndexElementFormats[] =
		{
			Helium::RENDERER_INDEX_FORMAT_INVALID,   // Unknown
			Helium::RENDERER_INDEX_FORMAT_UINT16,    // Unsigned16
			Helium::RENDERER_INDEX_FORMAT_UINT32,    // Unsigned32
		};

		HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( IndexElementFormats ) == IndexElementTypes::Count );

		class RenderVisitor : public Visitor
		{
		private:
			friend class RenderEntry;

			// view we are rendering for
			const Editor::Viewport* m_View;

			// buffered rendering interface
			BufferedDrawer* m_DrawInterface;

		public:
			RenderVisitor();

			const Editor::Viewport* GetViewport()
			{
				return m_View;
			}

			BufferedDrawer* GetDrawInterface() const
			{
				return m_DrawInterface;
			}

			void Reset( const Editor::Viewport* view, BufferedDrawer* drawInterface );
		};
	}
}