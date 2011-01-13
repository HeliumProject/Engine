#include "DependenciesComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT( DependenciesComponent );

void DependenciesComponent::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    Reflect::Field* fieldPaths = comp.AddField( &DependenciesComponent::m_Paths, TXT( "m_Paths" ) );
}
