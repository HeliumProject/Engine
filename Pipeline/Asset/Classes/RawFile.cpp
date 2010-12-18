#include "RawFile.h"

#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( RawFile );

void RawFile::AcceptCompositeVisitor( Reflect::Composite& comp )
{
	comp.SetProperty( AssetProperties::LongDescription, "A raw file asset wraps a single raw file for inclusion for the project." );
	comp.SetProperty( AssetProperties::FileFilter, "*.rawfile.*" );
}
