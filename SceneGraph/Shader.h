#pragma once

#include "Math/Color4.h"

#include "SceneGraph/API.h"
#include "SceneGraph/SceneNode.h"

#if HELIUM_OS_WIN
#include <d3d9.h>
#endif

namespace Helium
{
	namespace SceneGraph
	{
		class Scene;

		class HELIUM_SCENE_GRAPH_API Shader : public SceneNode
		{
		public:
			HELIUM_DECLARE_CLASS( Shader, SceneNode );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

		public:
			Shader();
			~Shader();

			virtual void Create() HELIUM_OVERRIDE;
			virtual void Delete() HELIUM_OVERRIDE;

			bool GetAlpha() const;
#if HELIUM_OS_WIN
			IDirect3DTexture9* GetBaseTexture() const;
#else
			void* GetBaseTexture() const;
#endif

		public:
			// Reflected
			bool                m_WrapU;
			bool                m_WrapV;
			float               m_RepeatU;
			float               m_RepeatV;
			Color4              m_BaseColor;
			FilePath            m_AssetPath;

		protected:
			// Non-reflected
			bool                m_Alpha;
			uint32_t            m_BaseTextureSize;
#if HELIUM_OS_WIN
			IDirect3DTexture9*  m_BaseTexture;
#else
			void*               m_BaseTexture;
#endif
		};
	}
}