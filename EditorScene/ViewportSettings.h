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
				info.AddElement(GeometryMode::None,         TXT( "None" ) ); 
				info.AddElement(GeometryMode::Render,       TXT( "Render" ) ); 
				info.AddElement(GeometryMode::Collision,    TXT( "Collision" ) ); 
				info.AddElement(GeometryMode::Pathfinding,  TXT( "Pathfinding" ) ); 
				info.AddElement(GeometryMode::Count,        TXT( "Count" ) ); 
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
				info.AddElement( Layer,     TXT( "Layer" ) );
				info.AddElement( Type,      TXT( "Type" ) );
				info.AddElement( Scene,     TXT( "Scene" ) );
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
				comp.AddField( &ViewportSettings::m_CameraMode, TXT( "Camera Mode" ) );
				comp.AddField( &ViewportSettings::m_GeometryMode, TXT( "Geometry Mode" ) );
				comp.AddField( &ViewportSettings::m_CameraPrefs, TXT( "Camera Preferences" ) );
				comp.AddField( &ViewportSettings::m_ColorMode, TXT( "Coloring Mode" ) );

				comp.AddField( &ViewportSettings::m_Highlighting, TXT( "Highlighting" ) );
				comp.AddField( &ViewportSettings::m_AxesVisible, TXT( "Draw Axes" ) );
				comp.AddField( &ViewportSettings::m_GridVisible, TXT( "Draw Grid" ) );
				comp.AddField( &ViewportSettings::m_BoundsVisible, TXT( "Draw Bounding Boxes" ) );
			}
		}; 

		typedef Helium::SmartPtr<ViewportSettings> ViewportSettingsPtr; 
	}
}