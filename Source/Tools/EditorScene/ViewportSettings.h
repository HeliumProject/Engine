#pragma once

#include "EditorScene/API.h"
#include "EditorScene/CameraSettings.h"
#include "EditorScene/SettingsManager.h"

namespace Helium
{
	namespace Editor
	{
		class GeometryMode
		{
		public:
			enum Enum
			{
				None,
				Render,
				Collision,
				Pathfinding,
				Count
			};

			HELIUM_DECLARE_ENUM( GeometryMode );

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(GeometryMode::None,         "None" ); 
				info.AddElement(GeometryMode::Render,       "Render" ); 
				info.AddElement(GeometryMode::Collision,    "Collision" ); 
				info.AddElement(GeometryMode::Pathfinding,  "Pathfinding" ); 
				info.AddElement(GeometryMode::Count,        "Count" ); 
			}
		};

		class ViewColorMode
		{
		public:
			enum Enum
			{
				Layer,
				Type,
				Scene,
			};

			HELIUM_DECLARE_ENUM( ViewColorMode );

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement( Layer,     "Layer" );
				info.AddElement( Type,      "Type" );
				info.AddElement( Scene,     "Scene" );
			}
		};

		class HELIUM_EDITOR_SCENE_API ViewportSettings : public Settings
		{
		public: 
			HELIUM_DECLARE_CLASS( ViewportSettings, Settings );

			ViewportSettings(); 

			virtual bool UserVisible() override
			{
				return false;
			}

			ViewColorMode GetColorMode() const;
			void SetColorMode( ViewColorMode mode );
			const Reflect::Field* ColorModeField() const;

			CameraMode           m_CameraMode; 
			GeometryMode         m_GeometryMode; 
			V_CameraSettings     m_CameraPrefs; // do not use m_CameraMode as an index!

			bool                 m_Highlighting; 
			bool                 m_AxesVisible; 
			bool                 m_GridVisible; 
			bool                 m_BoundsVisible; 

		private:
			ViewColorMode        m_ColorMode;

		public:
			static void PopulateMetaType( Reflect::MetaStruct& comp )
			{
				comp.AddField( &ViewportSettings::m_CameraMode, "Camera Mode" );
				comp.AddField( &ViewportSettings::m_GeometryMode, "Geometry Mode" );
				comp.AddField( &ViewportSettings::m_CameraPrefs, "Camera Preferences" );
				comp.AddField( &ViewportSettings::m_ColorMode, "Coloring Mode" );

				comp.AddField( &ViewportSettings::m_Highlighting, "Highlighting" );
				comp.AddField( &ViewportSettings::m_AxesVisible, "Draw Axes" );
				comp.AddField( &ViewportSettings::m_GridVisible, "Draw Grid" );
				comp.AddField( &ViewportSettings::m_BoundsVisible, "Draw Bounding Boxes" );
			}
		}; 

		typedef Helium::SmartPtr<ViewportSettings> ViewportSettingsPtr; 
	}
}