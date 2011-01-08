#include "SceneAsset.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Asset/Components/DependenciesComponent.h"
#include "Foundation/Component/ComponentHandle.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( SceneAsset );

void SceneAsset::AcceptCompositeVisitor( Reflect::Composite& comp )
{

}

bool SceneAsset::ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const
{
    if ( attr->HasType( Reflect::GetType<DependenciesComponent>() ) )
    {
        return true;
    }

    return __super::ValidateCompatible( attr, error );
}
