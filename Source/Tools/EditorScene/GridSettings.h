#pragma once

#include "EditorScene/API.h"
#include "EditorScene/SettingsManager.h"
#include "Math/Color.h"
#include "Reflect/TranslatorDeduction.h"

namespace Helium
{
	namespace Editor
	{
		class GridUnit
		{
		public:
			enum Enum
			{
				Meters,
				Centimeters,
			};

			HELIUM_DECLARE_ENUM( GridUnit );

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(Meters,         "Meters" );
				info.AddElement(Centimeters,    "Centimeters" );
			}
		};

		class HELIUM_EDITOR_SCENE_API GridSettings : public Settings
		{
		public:
			HELIUM_DECLARE_CLASS( GridSettings, Settings );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			GridSettings( const std::string& version = "",
				GridUnit units = GridUnit::Meters,
				uint32_t width = 12,
				uint32_t length = 12,
				float32_t majorStep = 5.0f,
				float32_t minorStep = 1.0f,
				Color axisColor = Color( 0.0f, 0.0f, 0.0f, 1.f ),
				Color majorColor = Color( 0.5f, 0.5f, 0.5f, 1.f ),
				Color minorColor = Color( 0.5f, 0.5f, 0.5f, 1.f ) );
			~GridSettings();

			virtual void PostDeserialize();

			uint32_t GetWidth();
			uint32_t GetLength();
			float32_t GetMajorStep();
			float32_t GetMinorStep();
			const Color& GetAxisColor();
			const Color& GetMajorColor();
			const Color& GetMinorColor();

		private:
			void OnChanged( const Reflect::ObjectChangeArgs& args );

			static float32_t GetConversionFactor( GridUnit units );
			static float32_t ConvertUnits( float32_t sourceValue, GridUnit sourceUnits, GridUnit destinationUnits );

		private:
			GridUnit m_Units;
			GridUnit m_PreviousUnits;
			uint32_t m_Width;
			uint32_t m_Length;
			float32_t m_MajorStep;
			float32_t m_MinorStep;
			Color m_AxisColor;
			Color m_MajorColor;
			Color m_MinorColor;
		};

		typedef Helium::SmartPtr<GridSettings> GridSettingsPtr;
	}
}