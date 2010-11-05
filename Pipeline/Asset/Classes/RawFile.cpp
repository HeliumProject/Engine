#include "RawFile.h"

#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( RawFile );

void RawFile::EnumerateClass( Reflect::Compositor<RawFile>& comp )
{
	comp.GetComposite().m_UIName = TXT( "Raw File" );
	comp.GetComposite().SetProperty( AssetProperties::LongDescription, "A raw file asset wraps a single raw file for inclusion for the project." );
	comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.rawfile.*" );

}


