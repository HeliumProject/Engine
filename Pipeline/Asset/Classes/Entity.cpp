#include "Entity.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Version.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT(Entity)

void Entity::PopulateComposite( Reflect::Composite& comp )
{
    comp.SetProperty( AssetProperties::LongDescription, "Entities are objects that can be placed in the game engine. This includes pieces of the environment and characters. All entities are associated with a corresponding Maya art file. Instances of entities can be placed in a level with the Scene Editor." );
    comp.SetProperty( AssetProperties::FileFilter, "*.HeliumEntity" );
}

void Entity::GatherSearchableProperties( Helium::SearchableProperties* properties ) const
{
    Base::GatherSearchableProperties( properties );
}
