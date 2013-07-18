#pragma once

#include "SceneGraph/API.h"
#include "SceneGraph/CameraSettings.h"
#include "SceneGraph/SettingsManager.h"

namespace Helium
{
    namespace SceneGraph
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

            REFLECT_DECLARE_ENUMERATION( GeometryMode );

            static void PopulateEnumeration( Reflect::Enumeration& info )
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

            REFLECT_DECLARE_ENUMERATION( ViewColorMode );

            static void PopulateEnumeration( Reflect::Enumeration& info )
            {
                info.AddElement( Layer,     TXT( "Layer" ) );
                info.AddElement( Type,      TXT( "Type" ) );
                info.AddElement( Scene,     TXT( "Scene" ) );
            }
        };

        class HELIUM_SCENE_GRAPH_API ViewportSettings : public Settings
        {
        public: 
            REFLECT_DECLARE_OBJECT( ViewportSettings, Settings );

            ViewportSettings(); 

            virtual bool UserVisible() HELIUM_OVERRIDE
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
            bool                 m_StatisticsVisible; 

        private:
            ViewColorMode        m_ColorMode;

        public:
            static void PopulateStructure( Reflect::Structure& comp )
            {
                comp.AddField( &ViewportSettings::m_CameraMode, TXT( "Camera Mode" ) );
                comp.AddField( &ViewportSettings::m_GeometryMode, TXT( "Geometry Mode" ) );
                comp.AddField( &ViewportSettings::m_CameraPrefs, TXT( "Camera Preferences" ) );
                comp.AddField( &ViewportSettings::m_ColorMode, TXT( "Coloring Mode" ) );

                comp.AddField( &ViewportSettings::m_Highlighting, TXT( "Highlighting" ) );
                comp.AddField( &ViewportSettings::m_AxesVisible, TXT( "Draw Axes" ) );
                comp.AddField( &ViewportSettings::m_GridVisible, TXT( "Draw Grid" ) );
                comp.AddField( &ViewportSettings::m_BoundsVisible, TXT( "Draw Bounding Boxes" ) );
                comp.AddField( &ViewportSettings::m_StatisticsVisible, TXT( "Draw Statistics" ) );
            }
        }; 

        typedef Helium::SmartPtr<ViewportSettings> ViewportSettingsPtr; 
    }
}