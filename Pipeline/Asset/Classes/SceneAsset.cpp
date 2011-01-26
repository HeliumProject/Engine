#include "SceneAsset.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Asset/Components/DependenciesComponent.h"
#include "Foundation/Component/ComponentHandle.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT( SceneAsset );

void SceneAsset::PopulateComposite( Reflect::Composite& comp )
{

}

bool SceneAsset::ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const
{
    if ( attr->IsClass( Reflect::GetClass<DependenciesComponent>() ) )
    {
        return true;
    }

    return Base::ValidateCompatible( attr, error );
}
