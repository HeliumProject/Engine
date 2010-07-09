#include "Entity.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"

using namespace Asset;

REFLECT_DEFINE_CLASS(Entity)

void Entity::EnumerateClass( Reflect::Compositor<Entity>& comp )
{
    comp.GetComposite().m_UIName = TXT( "Entity" );
    comp.GetComposite().SetProperty( AssetProperties::LongDescription, "Entities are objects that can be placed in the game engine. This includes pieces of the environment and characters. All entities are associated with a corresponding Maya art file. Instances of entities can be placed in a level with the Scene Editor." );
    comp.GetComposite().SetProperty( AssetProperties::FileFilter, "*.entity.*" );
}

void Entity::GatherIndexData( AssetIndexData& indexData )
{
}
