#pragma once

#include "EditorScene/API.h"

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

namespace Helium
{
	namespace Editor
	{
		class CameraMode
		{
		public:
			enum Enum
			{
				Orbit,
				Front,
				Side,
				Top,

				Count
			};

			HELIUM_DECLARE_ENUM(CameraMode);

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(CameraMode::Orbit,  "Orbit" ); 
				info.AddElement(CameraMode::Front,  "Front" ); 
				info.AddElement(CameraMode::Side,   "Side" ); 
				info.AddElement(CameraMode::Top,    "Top" ); 
			}
		};

		class ShadingMode
		{
		public:
			enum Enum
			{
				Wireframe,
				Material,
				Texture,
			};

			HELIUM_DECLARE_ENUM(ShadingMode);

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(ShadingMode::Wireframe, "Wireframe" );
				info.AddElement(ShadingMode::Material,  "Material" );
				info.AddElement(ShadingMode::Texture,   "Texture" );
			}
		};

		class HELIUM_EDITOR_SCENE_API CameraSettings : public Reflect::Object
		{
		public:
			HELIUM_DECLARE_CLASS( CameraSettings, Reflect::Object );

			CameraSettings(); 

		public: 
			CameraMode  m_CameraMode; // we save what mode we correspond to
			ShadingMode m_ShadingMode; 

			bool m_WireframeOnMesh; 
			bool m_WireframeOnShaded;

			bool m_ViewFrustumCulling; 
			bool m_BackFaceCulling; 

		public:
			static void PopulateMetaType( Reflect::MetaStruct& comp )
			{
				comp.AddField( &CameraSettings::m_CameraMode, "Camera Mode" );
				comp.AddField( &CameraSettings::m_ShadingMode, "Shading Mode" );
				comp.AddField( &CameraSettings::m_WireframeOnMesh, "Wireframe On Mesh" );
				comp.AddField( &CameraSettings::m_WireframeOnShaded, "Wireframe On Shaded" );
				comp.AddField( &CameraSettings::m_ViewFrustumCulling, "View Frustum Culling" );
				comp.AddField( &CameraSettings::m_BackFaceCulling, "Backface Culling" );
			}
		};

		typedef Helium::StrongPtr<CameraSettings> CameraSettingsPtr; 
		typedef std::vector< CameraSettingsPtr> V_CameraSettings; 
	}
}