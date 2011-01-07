#include "ColorComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( ColorComponent );

void ColorComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &ColorComponent::m_ColorHDR, TXT( "m_ColorHDR" ) );
    comp.AddField( &ColorComponent::m_Alpha, TXT( "m_Alpha" ) );
    comp.AddField( &ColorComponent::m_HDR, TXT( "m_HDR" ) );
}

Component::ComponentUsage ColorComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}

void ColorComponent::GetColor( Color3& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( Color4& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( HDRColor3& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::GetColor( HDRColor4& color ) const
{
    color = m_ColorHDR;
}

void ColorComponent::SetColor( const HDRColor4& color )
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
