#include "ShaderAsset.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/ArchiveXML.h"

#include "Foundation/Component/ComponentHandle.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ENUMERATION( AlphaType );
REFLECT_DEFINE_OBJECT( ShaderAsset );

void ShaderAsset::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::FileFilter, TXT( "*.shader.*" ) );

    comp.AddField( &ShaderAsset::m_ColorMapPath, TXT( "m_ColorMapPath" ) );
    comp.AddField( &ShaderAsset::m_NormalMapPath, TXT( "m_NormalMapPath" ) );
    comp.AddField( &ShaderAsset::m_GPIMapPath, TXT( "m_GPIMapPath" ) );

    comp.AddField( &ShaderAsset::m_EnableColorMapTint, TXT( "m_EnableColorMapTint" ) );
    comp.AddField( &ShaderAsset::m_ColorMapTint, TXT( "m_ColorMapTint" ) );

    comp.AddField( &ShaderAsset::m_NormalMapScaling, TXT( "m_NormalMapScaling" ) );

    comp.AddField( &ShaderAsset::m_GlossMapEnabled, TXT( "m_GlossMapEnabled" ) );
    comp.AddField( &ShaderAsset::m_GlossMapScaling, TXT( "m_GlossMapScaling" ) );
    comp.AddField( &ShaderAsset::m_GlossMapTint, TXT( "m_GlossMapTint" ) );
    comp.AddField( &ShaderAsset::m_GlossMapDirtiness, TXT( "m_GlossMapDirtiness" ) );
    comp.AddField( &ShaderAsset::m_SpecularPower, TXT( "m_SpecularPower" ) );

    comp.AddField( &ShaderAsset::m_ParallaxMapEnabled, TXT( "m_ParallaxMapEnabled" ) );
    comp.AddField( &ShaderAsset::m_ParallaxMapScaling, TXT( "m_ParallaxMapScaling" ) );

    comp.AddField( &ShaderAsset::m_IncandescentMapEnabled, TXT( "m_IncandescentMapEnabled" ) );
    comp.AddField( &ShaderAsset::m_IncandescentMapScaling, TXT( "m_IncandescentMapScaling" ) );

    comp.AddField( &ShaderAsset::m_DoubleSided, TXT( "m_DoubleSided" ) );

    comp.AddEnumerationField( &ShaderAsset::m_WrapModeU, TXT( "m_WrapModeU" ) );
    comp.AddEnumerationField( &ShaderAsset::m_WrapModeV, TXT( "m_WrapModeV" ) );
    comp.AddEnumerationField( &ShaderAsset::m_AlphaMode, TXT( "m_AlphaMode" ) );
}
