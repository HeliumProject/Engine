#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectData.h"

using namespace Helium::Inspect;

Label::Label()
{
    a_ProportionalWidth.Set( 1.f/3.f );
}

bool Label::Process(const tstring& key, const tstring& value)
{
    bool handled = false;

    if (__super::Process(key, value))
    {
        return true;
    }

    if (key == LABEL_ATTR_TEXT)
    {
        SetText( value );
        return true;
    }

    return false;
}

void Label::Read()
{
    if ( IsBound() )
    {
        tstring str;
        ReadStringData( str );
        UpdateUI( str );

        __super::Read();
    }
}

void Label::SetText(const tstring& text)
{
    if ( !IsBound() )
    {
        Bind( new StringFormatter<tstring>( new tstring( text ), true ) );
    }
    else
    {
        WriteStringData( text );
    }

    UpdateUI( text );
}

tstring Label::GetText() const
{
    tstring text;
    if ( IsBound() )
    {
        ReadStringData( text );
    }
    return text;
}

void Label::UpdateUI( const tstring& text )
{
    if ( IsRealized() )
    {
        tstring trimmed = text;
        EllipsizeString( trimmed, m_Window->GetSize().GetWidth() );

        wxStaticText* staticText = Control::Cast< wxStaticText >( this );
        if ( trimmed != staticText->GetLabel().c_str() )
        {
            m_Window->Freeze();
            staticText->SetLabel( trimmed.c_str() );
            m_Window->Thaw();
        }
    }
}
