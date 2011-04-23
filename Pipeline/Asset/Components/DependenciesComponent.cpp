#include "DependenciesComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT( DependenciesComponent );

void DependenciesComponent::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &DependenciesComponent::m_Paths, TXT( "m_Paths" ) );
}
