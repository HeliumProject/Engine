#pragma once

#include "Pipeline/API.h"

#include "Foundation/Math/Color3.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace SceneGraph
    {
        class GridUnit
        {
        public:
            enum Enum
            {
                Meters,
                Centimeters,
            };

            REFLECT_DECLARE_ENUMERATION( GridUnit );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(Meters,         TXT( "Meters" ) );
                info.AddElement(Centimeters,    TXT( "Centimeters" ) );
            }
        };

        class PIPELINE_API GridSettings : public Reflect::Element
        {
        public:
            REFLECT_DECLARE_CLASS( GridSettings, Reflect::Element );

            GridSettings( const tstring& version = TXT( "" ),
                GridUnit units = GridUnit::Meters,
                uint32_t width = 12,
                uint32_t length = 12,
                float32_t majorStep = 5.0f,
                float32_t minorStep = 1.0f,
                Color3 axisColor = Color3( Vector3( 0.0f, 0.0f, 0.0f ) ),
                Color3 majorColor = Color3( Vector3( 0.5f, 0.5f, 0.5f ) ),
                Color3 minorColor = Color3( Vector3( 0.5f, 0.5f, 0.5f ) ) );
            ~GridSettings();

            virtual void PostDeserialize();

            uint32_t GetWidth();
            uint32_t GetLength();
            float32_t GetMajorStep();
            float32_t GetMinorStep();
            const Color3& GetAxisColor();
            const Color3& GetMajorColor();
            const Color3& GetMinorColor();

        private:
            void OnChanged( const Reflect::ElementChangeArgs& args );

            static float32_t GetConversionFactor( GridUnit units );
            static float32_t ConvertUnits( float32_t sourceValue, GridUnit sourceUnits, GridUnit destinationUnits );

        private:
            GridUnit m_Units;
            GridUnit m_PreviousUnits;
            uint32_t m_Width;
            uint32_t m_Length;
            float32_t m_MajorStep;
            float32_t m_MinorStep;
            Color3 m_AxisColor;
            Color3 m_MajorColor;
            Color3 m_MinorColor;

        public:
            static void AcceptCompositeVisitor( Reflect::Composite& comp )
            {
                comp.AddEnumerationField( &GridSettings::m_Units, "Units" );
                comp.AddField( &GridSettings::m_Width, "Width" );
                comp.AddField( &GridSettings::m_Length, "Length" );
                comp.AddField( &GridSettings::m_MajorStep, "Major Step" );
                comp.AddField( &GridSettings::m_MinorStep, "Minor Step" );
                comp.AddField( &GridSettings::m_AxisColor, "Axis Color" );
                comp.AddField( &GridSettings::m_MajorColor, "Major Color" );
                comp.AddField( &GridSettings::m_MinorColor, "Minor Color" );
            }
        };

        typedef Helium::SmartPtr<GridSettings> GridSettingsPtr;
    }
}