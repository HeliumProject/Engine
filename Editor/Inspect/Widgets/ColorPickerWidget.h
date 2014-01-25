#pragma once

#include "Editor/Inspect/wxWidget.h"

#include "Editor/Controls/ColorPicker.h"
#include "Editor/SimpleConfig.h"

namespace Helium
{
	namespace Editor
	{
		class ColorPickerWidget;

		class ColorPickerWindow : public wxPanel
		{
		public:
			ColorPickerWindow( wxWindow* parent, ColorPickerWidget* colorPickerWidget );

			void SetOverride( bool override )
			{
				m_Override = override;
			}

			void SetColor( const Color& color )
			{
				HELIUM_ASSERT( m_ColorPicker );
				m_ColorPicker->SetColour( wxColor( color.GetR(), color.GetG(), color.GetB(), color.GetA() ) );
			}

			void GetColor( Color& color )
			{
				HELIUM_ASSERT( m_ColorPicker );
				wxColour value = m_ColorPicker->GetColour();
				color.SetR( value.Red() );
				color.SetG( value.Green() );
				color.SetB( value.Blue() );
				color.SetA( value.Alpha() );
			}

			// Callback when the color is changed
			void OnChanged( wxCommandEvent& );

		private:
			ColorPicker*            m_ColorPicker;
			ColorPickerWidget*      m_ColorPickerWidget;
			bool                    m_Override;
		};

		class ColorPickerWidget : public Widget
		{
		public:
			HELIUM_DECLARE_CLASS( ColorPickerWidget, Widget );

			ColorPickerWidget()
				: m_ColorPickerControl( NULL )
				, m_ColorPickerWindow( NULL )
			{

			}

			ColorPickerWidget( Inspect::ColorPicker* colorPicker );

			virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
			virtual void DestroyWindow() HELIUM_OVERRIDE;

			// Inspect::Widget
			virtual void Read() HELIUM_OVERRIDE;
			virtual bool Write() HELIUM_OVERRIDE;

			// Editor::Widget
			virtual void IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args ) HELIUM_OVERRIDE;

			// Listeners
			void HighlightChanged( const Attribute< bool >::ChangeArgs& args );

		protected:
			Inspect::ColorPicker* m_ColorPickerControl;
			ColorPickerWindow*    m_ColorPickerWindow;
		};

	}
}