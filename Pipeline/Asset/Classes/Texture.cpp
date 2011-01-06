#include "Texture.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( Asset::TextureFormat );
REFLECT_DEFINE_ENUMERATION( Asset::TextureFilter );
REFLECT_DEFINE_ENUMERATION( Asset::TextureCoordinateWrapMode );
REFLECT_DEFINE_CLASS( Texture );

void Texture::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::FileFilter, "*.tga;*.png;*.jpg;*.bmp" );

    comp.AddEnumerationField( &Texture::m_WrapModeU, "m_WrapModeU" );
    comp.AddEnumerationField( &Texture::m_WrapModeV, "m_WrapModeV" );
}
