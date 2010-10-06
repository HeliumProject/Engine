#pragma once

#include "Core/API.h"

#include "Foundation/Reflect/Element.h"
#include "Core/SceneGraph/CameraSettings.h"

namespace Helium
{
    namespace SceneGraph
    {
        namespace GeometryModes
        {
            enum GeometryMode
            {
                None,
                Render,
                Collision,
                Pathfinding,
                Count
            };

            static void GeometryModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(GeometryModes::None, TXT( "GeometryModes::None" ) ); 
                info->AddElement(GeometryModes::Render, TXT( "GeometryModes::Render" ) ); 
                info->AddElement(GeometryModes::Collision, TXT( "GeometryModes::Collision" ) ); 
                info->AddElement(GeometryModes::Pathfinding, TXT( "GeometryModes::Pathfinding" ) ); 
                info->AddElement(GeometryModes::Count, TXT( "GeometryModes::Count" ) ); 
            }
        }

        typedef GeometryModes::GeometryMode GeometryMode;

        namespace ViewColorModes
        {
            enum ViewColorMode
            {
                Layer,
                Type,
                Scene,
            };
            static void ViewColorModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( Layer, TXT( "Layer" ) );
                info->AddElement( Type, TXT( "Type" ) );
                info->AddElement( Scene, TXT( "Scene" ) );
            }
        }
        typedef ViewColorModes::ViewColorMode ViewColorMode;

        class CORE_API ViewportSettings : public Reflect::Element
        {
        public: 
            REFLECT_DECLARE_CLASS( ViewportSettings, Reflect::Element );

            ViewportSettings(); 

            ViewColorMode GetColorMode() const;
            void SetColorMode( ViewColorMode mode );
            const Reflect::Field* ColorModeField() const;

            CameraMode           m_CameraMode; 
            GeometryMode         m_GeometryMode; 
            V_CameraSettings  m_CameraPrefs; // do not use m_CameraMode as an index!

            bool                 m_Highlighting; 
            bool                 m_AxesVisible; 
            bool                 m_GridVisible; 
            bool                 m_BoundsVisible; 
            bool                 m_StatisticsVisible; 

        private:
            ViewColorMode        m_ColorMode;

        public:
            static void EnumerateClass( Reflect::Compositor<ViewportSettings>& comp )
            {
                comp.AddEnumerationField( &ViewportSettings::m_CameraMode, "m_CameraMode" );
                comp.AddEnumerationField( &ViewportSettings::m_GeometryMode, "m_GeometryMode" );
                comp.AddField( &ViewportSettings::m_CameraPrefs, "m_CameraPrefs" );
                comp.AddEnumerationField( &ViewportSettings::m_ColorMode, "m_ColorMode" );

                comp.AddField( &ViewportSettings::m_Highlighting, "m_Highlighting" );
                comp.AddField( &ViewportSettings::m_AxesVisible, "m_AxesVisible" );
                comp.AddField( &ViewportSettings::m_GridVisible, "m_GridVisible" );
                comp.AddField( &ViewportSettings::m_BoundsVisible, "m_BoundsVisible" );
                comp.AddField( &ViewportSettings::m_StatisticsVisible, "m_StatisticsVisible" );
            }
        }; 

        typedef Helium::SmartPtr<ViewportSettings> ViewportSettingsPtr; 
    }
}