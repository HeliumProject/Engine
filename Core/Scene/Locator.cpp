/*#include "Precompile.h"*/
#include "Locator.h"

#include "Core/Content/Nodes/ContentLocator.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/LocatorType.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/PrimitiveLocator.h"
#include "Core/Scene/PrimitiveCube.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::Locator);

void Locator::InitializeType()
{
    Reflect::RegisterClassType< Core::Locator >( TXT( "Core::Locator" ) );

    PropertiesGenerator::InitializePanel( TXT( "Locator" ), CreatePanelSignature::Delegate( &Locator::CreatePanel ) );
}

void Locator::CleanupType()
{
    Reflect::UnregisterClassType< Core::Locator >();
}

Locator::Locator(Core::Scene* scene)
: Core::Instance (scene, new Content::Locator ())
{

}

Locator::Locator(Core::Scene* scene, Content::Locator* locator)
: Core::Instance ( scene, locator )
{

}

Locator::~Locator()
{

}

i32 Locator::GetImageIndex() const
{
#pragma TODO( "Fix this up somehow" )
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "locator" ) );
}

tstring Locator::GetApplicationTypeName() const
{
    return TXT( "Locator" );
}

SceneNodeTypePtr Locator::CreateNodeType( Core::Scene* scene ) const
{
    // Overridden to create an locator-specific type
    Core::LocatorType* nodeType = new Core::LocatorType( scene, GetType() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

int Locator::GetShape() const
{
    return GetPackage< Content::Locator >()->m_Shape;
}

void Locator::SetShape( int shape )
{
    GetPackage< Content::Locator >()->m_Shape = static_cast< Content::LocatorShape > (shape);
}

void Locator::Evaluate(GraphDirection direction)
{
    __super::Evaluate(direction);

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            // start the box from scratch
            m_ObjectBounds.Reset();

            // merge type pointer into our bounding box
            if (m_NodeType)
            {
                Core::LocatorType* type = Reflect::AssertCast<Core::LocatorType>(m_NodeType);

                const Core::Primitive* prim = type->GetShape( GetPackage< Content::Locator >()->m_Shape );
                if (prim)
                {
                    m_ObjectBounds.Merge(prim->GetBounds());
                }
            }

            break;
        }
    }
}

void Locator::Render( RenderVisitor* render )
{
    const Content::Locator* package = GetPackage< Content::Locator >();

    // shape is drawn non-normalized
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Locator::DrawShape;

        if ( package->m_TransparentOverride ? package->m_Transparent : Reflect::AssertCast<Core::InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    // don't call __super here, it will draw big ass axes
    Core::HierarchyNode::Render( render );
}

void Locator::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Core::Locator* locator = Reflect::ConstAssertCast<Core::Locator>( object );

    const Core::LocatorType* type = Reflect::ConstAssertCast<Core::LocatorType>( locator->GetNodeType() );

    const Content::Locator* package = locator->GetPackage< Content::Locator >();

    locator->SetMaterial( type->GetMaterial() );

    const Core::Primitive* prim = type->GetShape( package->m_Shape );
    if (prim)
    {
        prim->Draw( args, package->m_SolidOverride ? &package->m_Solid : NULL, package->m_TransparentOverride ? &package->m_Transparent : NULL );
    }
}

bool Locator::Pick( PickVisitor* pick )
{
    bool result = false;

    const Core::LocatorType* type = Reflect::AssertCast<Core::LocatorType>(m_NodeType);

    const Content::Locator* package = GetPackage< Content::Locator >();

    pick->SetCurrentObject (this, pick->State().m_Matrix);

    const Core::Primitive* prim = type->GetShape( package->m_Shape );
    if (prim)
    {
        result |= prim->Pick(pick, package->m_SolidOverride ? &package->m_Solid : NULL);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Core::Locator.
// 
bool Locator::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Locator" ) )
    {
        return true;
    }

    return __super::ValidatePanel( name );
}

void Locator::CreatePanel( CreatePanelArgs& args )
{
    Inspect::Container* container = args.m_Generator->PushContainer( TXT( "Locator" ) );
    container->a_HelpText.Set( TXT( "This area contains settings specific to the locator node type." ) );
    {
        args.m_Generator->PushContainer();
        {
            static const tstring helpText = TXT( "Choose the shape of the locator." );
            args.m_Generator->AddLabel( TXT( "Shape" ) )->a_HelpText.Set( helpText );

            Inspect::Choice* choice = args.m_Generator->AddChoice<Core::Locator, int>(args.m_Selection, &Locator::GetShape, &Locator::SetShape);
            choice->a_IsDropDown.Set( true );
            choice->a_HelpText.Set( helpText );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << Content::LocatorShapes::Cross;
                items.push_back( Inspect::ChoiceItem( TXT( "Cross" ), str.str() ) );
            }

            {
                tostringstream str;
                str << Content::LocatorShapes::Cube;
                items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
            }

            choice->a_Items.Set( items );
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}