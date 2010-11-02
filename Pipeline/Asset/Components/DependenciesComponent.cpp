#include "DependenciesComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( DependenciesComponent );

void DependenciesComponent::EnumerateClass( Reflect::Compositor<DependenciesComponent>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Dependencies" );

  Reflect::Field* fieldPaths = comp.AddField( &DependenciesComponent::m_Paths, "m_Paths" );
}
