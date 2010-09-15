#include "ColorComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( ColorComponent );

void ColorComponent::EnumerateClass( Reflect::Compositor< ColorComponent >& comp )
{
    comp.GetComposite().m_UIName = TXT( "Color" );

    comp.AddField( &ColorComponent::m_ColorHDR, "m_ColorHDR" );

    comp.AddField( &ColorComponent::m_Alpha, "m_Alpha" );
    comp.AddField( &ColorComponent::m_HDR, "m_HDR" );
}

Component::ComponentUsage ColorComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}

void ColorComponent::GetColor( Math::Color3& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( Math::Color4& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( Math::HDRColor3& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( Math::HDRColor4& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::SetColor( const Math::HDRColor4& color )
{
    m_ColorHDR = color;
}

bool ColorComponent::Alpha() const
{
    return m_Alpha;
}

void ColorComponent::SetAlpha( bool alpha )
{
    m_Alpha = alpha;
}

bool ColorComponent::HDR() const
{
    return m_HDR;
}

void ColorComponent::SetHDR( bool hdr )
{
    m_HDR = hdr;
}
