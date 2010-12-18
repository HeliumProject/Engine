#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Element.h"
#include "Pipeline/SceneGraph/CameraSettings.h"

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

            static void EnumerateEnum( Reflect::Enumeration& info )
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

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Layer,     TXT( "Layer" ) );
                info.AddElement( Type,      TXT( "Type" ) );
                info.AddElement( Scene,     TXT( "Scene" ) );
            }
        };

        class PIPELINE_API ViewportSettings : public Reflect::Element
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
            static void AcceptCompositeVisitor( Reflect::Composite& comp )
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