#include "FoundationPch.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::Version );

using namespace Helium;
using namespace Helium::Reflect;

void Version::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Version::m_Source, TXT( "m_Source" ) );
    comp.AddField( &Version::m_SourceVersion, TXT( "m_SourceVersion" ) );
}

Reflect::Version::Version()
{

}

Reflect::Version::Version(const tchar_t* source, const tchar_t* sourceVersion)
: m_Source (source)
, m_SourceVersion (sourceVersion)
{

}

bool Reflect::Version::IsCurrent()
{
    return true;
}
