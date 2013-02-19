#include "FrameworkPch.h"
#include "Framework/World.h"

#include "Rendering/Renderer.h"
#include "Rendering/RSurface.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/RenderResourceManager.h"
#include "Framework/EntityDefinition.h"
#include "Framework/SliceDefinition.h"
#include "Framework/Slice.h"

namespace Helium
{
    class World;
    typedef Helium::StrongPtr< World > WorldPtr;
    typedef Helium::StrongPtr< const World > ConstWorldPtr;
}

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT( Helium::WorldDefinition, Framework, 0 );

//WorldDefinition::WorldDefinition()
//{
//}
//
//WorldDefinition::~WorldDefinition()
//{
//
//}
