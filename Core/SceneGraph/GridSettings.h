#pragma once

#include "Core/API.h"

#include "Foundation/Math/Color3.h"
#include "Foundation/Reflect/Element.h"

namespace Helium
{
    namespace SceneGraph
    {
        namespace GridUnits
        {
            enum GridUnit
            {
                Meters,
                Centimeters,
            };

            static void GridUnitEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Meters, TXT( "Meters" ) );
                info->AddElement(Centimeters, TXT( "Centimeters" ) );
            }
        }
        typedef GridUnits::GridUnit GridUnit;

        class CORE_API GridSettings : public Reflect::Element
        {
        public:
            REFLECT_DECLARE_CLASS( GridSettings, Reflect::Element );

            GridSettings( const tstring& version = TXT( "" ),
                GridUnit units = GridUnits::Meters,
                u32 width = 12,
                u32 length = 12,
                f32 majorStep = 5.0f,
                f32 minorStep = 1.0f,
                Color3 axisColor = Color3( Vector3( 0.0f, 0.0f, 0.0f ) ),
                Color3 majorColor = Color3( Vector3( 0.5f, 0.5f, 0.5f ) ),
                Color3 minorColor = Color3( Vector3( 0.5f, 0.5f, 0.5f ) ) );
            ~GridSettings();

            virtual void PostDeserialize();

            u32 GetWidth();
            u32 GetLength();
            f32 GetMajorStep();
            f32 GetMinorStep();
            const Color3& GetAxisColor();
            const Color3& GetMajorColor();
            const Color3& GetMinorColor();

        private:
            void OnChanged( const Reflect::ElementChangeArgs& args );

            static f32 GetConversionFactor( GridUnit units );
            static f32 ConvertUnits( f32 sourceValue, GridUnit sourceUnits, GridUnit destinationUnits );

        private:
            GridUnit m_Units;
            GridUnit m_PreviousUnits;
            u32 m_Width;
            u32 m_Length;
            f32 m_MajorStep;
            f32 m_MinorStep;
            Color3 m_AxisColor;
            Color3 m_MajorColor;
            Color3 m_MinorColor;

        public:
            static void EnumerateClass( Reflect::Compositor< GridSettings >& comp )
            {
                comp.AddEnumerationField( &GridSettings::m_Units, "m_Units" );
                comp.AddField( &GridSettings::m_Width, "m_Width" );
                comp.AddField( &GridSettings::m_Length, "m_Length" );
                comp.AddField( &GridSettings::m_MajorStep, "m_MajorStep" );
                comp.AddField( &GridSettings::m_MinorStep, "m_MinorStep" );
                comp.AddField( &GridSettings::m_AxisColor, "m_AxisColor" );
                comp.AddField( &GridSettings::m_MajorColor, "m_MajorColor" );
                comp.AddField( &GridSettings::m_MinorColor, "m_MinorColor" );
            }
        };

        typedef Helium::SmartPtr<GridSettings> GridSettingsPtr;
    }
}