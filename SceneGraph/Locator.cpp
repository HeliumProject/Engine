#include "SceneGraphPch.h"
#include "Locator.h"

#include "SceneGraph/Scene.h"
#include "SceneGraph/PropertiesGenerator.h"
#include "SceneGraph/PrimitiveLocator.h"
#include "SceneGraph/PrimitiveCube.h"

HELIUM_DEFINE_ENUM( Helium::SceneGraph::LocatorShape );
HELIUM_DEFINE_CLASS( Helium::SceneGraph::Locator );

using namespace Helium;
using namespace Helium::SceneGraph;

void Locator::PopulateMetaType( Reflect::MetaStruct& comp )
{
    Reflect::Field* field = comp.AddField( &Locator::m_Shape, TXT( "m_Shape" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "Determines the shape of the locator node." ) );
}

void Locator::InitializeType()
{

}

void Locator::CleanupType()
{

}

Locator::Locator()
: m_Shape (LocatorShape::Cross)
{

}

Locator::~Locator()
{

}

LocatorShape Locator::GetShape() const
{
    return m_Shape;
}

void Locator::SetShape( LocatorShape shape )
{
    m_Shape = shape;
	Dirty();
}

void Locator::Evaluate(GraphDirection direction)
{
    Base::Evaluate(direction);

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            // start the box from scratch
            m_ObjectBounds.Reset();

            // merge type pointer into our bounding box
            const Primitive* prim = NULL;
            if (prim)
            {
                m_ObjectBounds.Merge(prim->GetBounds());
            }

            break;
        }

    case GraphDirections::Upstream:
        break;
    }
}

void Locator::Render( RenderVisitor* render )
{
#ifdef VIEWPORT_REFACTOR
    // shape is drawn non-normalized
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Locator::DrawShape;

        if ( m_Transparent )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }
#endif

    // don't call Base here, it will draw big ass axes
    HierarchyNode::Render( render );
}

#ifdef VIEWPORT_REFACTOR

void Locator::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Locator* locator = Reflect::AssertCast<Locator>( object );

    locator->SetMaterial( type->GetMaterial() );

    const Primitive* prim = NULL;
    if (prim)
    {
        prim->Draw( args, locator->m_SolidOverride ? &locator->m_Solid : NULL, locator->m_TransparentOverride ? &locator->m_Transparent : NULL );
    }
}

#endif

bool Locator::Pick( PickVisitor* pick )
{
    bool result = false;

    pick->SetCurrentObject(this, pick->State().m_Matrix);

    const Primitive* prim = NULL;
    if (prim)
    {
        result |= prim->Pick(pick, m_SolidOverride ? &m_Solid : NULL);
    }

    return result;
}