/*#include "Precompile.h"*/
#include "Locator.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "Pipeline/SceneGraph/LocatorType.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"
#include "Pipeline/SceneGraph/PrimitiveLocator.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ENUMERATION( LocatorShape );
REFLECT_DEFINE_OBJECT( Locator );

void Locator::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    Reflect::Field* field = comp.AddEnumerationField( &Locator::m_Shape, TXT( "m_Shape" ) );
    field->SetProperty( TXT( "HelpText" ), TXT( "Determines the shape of the locator node." ) );
}

void Locator::InitializeType()
{
    Reflect::RegisterClassType< Locator >( TXT( "SceneGraph::Locator" ) );

    PropertiesGenerator::InitializePanel( TXT( "Locator" ), CreatePanelSignature::Delegate( &Locator::CreatePanel ) );
}

void Locator::CleanupType()
{
    Reflect::UnregisterClassType< Locator >();
}

Locator::Locator()
: m_Shape (LocatorShape::Cross)
{

}

Locator::~Locator()
{

}

int32_t Locator::GetImageIndex() const
{
#pragma TODO( "Fix this up somehow" )
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "locator" ) );
}

tstring Locator::GetApplicationTypeName() const
{
    return TXT( "Locator" );
}

SceneNodeTypePtr Locator::CreateNodeType( Scene* scene ) const
{
    // Overridden to create an locator-specific type
    LocatorType* nodeType = new LocatorType( scene, GetClass() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

int Locator::GetShape() const
{
    return (int)m_Shape;
}

void Locator::SetShape( int shape )
{
    m_Shape = static_cast< LocatorShape::Enum >( shape );
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
            if (m_NodeType)
            {
                LocatorType* type = Reflect::AssertCast<LocatorType>(m_NodeType);

                const Primitive* prim = type->GetShape( m_Shape );
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
    // shape is drawn non-normalized
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Locator::DrawShape;

        if ( m_TransparentOverride ? m_Transparent : Reflect::AssertCast<InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    // don't call Base here, it will draw big ass axes
    HierarchyNode::Render( render );
}

void Locator::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Locator* locator = Reflect::AssertCast<Locator>( object );

    const LocatorType* type = Reflect::AssertCast<LocatorType>( locator->GetNodeType() );

    locator->SetMaterial( type->GetMaterial() );

    const Primitive* prim = type->GetShape( locator->m_Shape );
    if (prim)
    {
        prim->Draw( args, locator->m_SolidOverride ? &locator->m_Solid : NULL, locator->m_TransparentOverride ? &locator->m_Transparent : NULL );
    }
}

bool Locator::Pick( PickVisitor* pick )
{
    bool result = false;

    const LocatorType* type = Reflect::AssertCast<LocatorType>(m_NodeType);

    pick->SetCurrentObject(this, pick->State().m_Matrix);

    const Primitive* prim = type->GetShape( m_Shape );
    if (prim)
    {
        result |= prim->Pick(pick, m_SolidOverride ? &m_Solid : NULL);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Locator.
// 
bool Locator::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Locator" ) )
    {
        return true;
    }

    return Base::ValidatePanel( name );
}

void Locator::CreatePanel( CreatePanelArgs& args )
{
    Inspect::Container* container = args.m_Generator->PushContainer( TXT( "Locator" ) );
    container->a_HelpText.Set( TXT( "This area contains settings specific to the locator node type." ) );
    {
        args.m_Generator->PushContainer( TXT( "Shape Control" ) );
        {
            const tstring helpText = TXT( "Choose the shape of the locator." );
            args.m_Generator->AddLabel( TXT( "Shape" ) )->a_HelpText.Set( helpText );

            Inspect::Choice* choice = args.m_Generator->AddChoice<Locator, int>(args.m_Selection, &Locator::GetShape, &Locator::SetShape);
            choice->a_IsDropDown.Set( true );
            choice->a_HelpText.Set( helpText );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << LocatorShape::Cross;
                items.push_back( Inspect::ChoiceItem( TXT( "Cross" ), str.str() ) );
            }

            {
                tostringstream str;
                str << LocatorShape::Cube;
                items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
            }

            choice->a_Items.Set( items );
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}