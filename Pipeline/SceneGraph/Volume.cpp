/*#include "Precompile.h"*/
#include "Volume.h"

#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "Pipeline/SceneGraph/VolumeType.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"
#include "Pipeline/SceneGraph/PrimitiveCylinder.h"
#include "Pipeline/SceneGraph/PrimitiveSphere.h"
#include "Pipeline/SceneGraph/PrimitiveCapsule.h"
#include "Pipeline/SceneGraph/PrimitivePointer.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ENUMERATION( VolumeShape );
REFLECT_DEFINE_CLASS( Volume );

void Volume::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddEnumerationField( &Volume::m_Shape, TXT( "m_Shape" ) );
    comp.AddField( &Volume::m_ShowPointer,      TXT( "m_ShowPointer" ) );
}

void Volume::InitializeType()
{
    Reflect::RegisterClassType< Volume >( TXT( "SceneGraph::Volume" ) );

    PropertiesGenerator::InitializePanel( TXT( "Volume" ), CreatePanelSignature::Delegate( &Volume::CreatePanel ) );
}

void Volume::CleanupType()
{
    Reflect::UnregisterClassType< Volume >();
}

Volume::Volume()
: m_Shape (VolumeShape::Cube)
           
{

}

Volume::~Volume()
{

}

int32_t Volume::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "volume" ) );
}

tstring Volume::GetApplicationTypeName() const
{
    return TXT( "Volume" );
}

SceneNodeTypePtr Volume::CreateNodeType( Scene* scene ) const
{
    // Overridden to create an volume-specific type
    VolumeType* nodeType = new VolumeType( scene, GetClass() );

    // Set the image index (usually this is handled by the base class, but we aren't calling the base)
    nodeType->SetImageIndex( GetImageIndex() );

    return nodeType;
}

int Volume::GetShape() const
{
    return (int)m_Shape;
}

void Volume::SetShape( int shape )
{
    m_Shape = static_cast< VolumeShape::Enum >( shape );
}

void Volume::Evaluate(GraphDirection direction)
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
                VolumeType* type = Reflect::AssertCast<VolumeType>(m_NodeType);

                if ( IsPointerVisible() )
                {
                    AlignedBox box (type->GetPointer()->GetBounds());

                    Scale scale;
                    Matrix3 rotate;
                    Vector3 translate;
                    m_InverseGlobalTransform.Decompose (scale, rotate, translate);

                    //  this will compensate for the normalized render of the pointer
                    box.Transform (Matrix4 (scale));

                    m_ObjectBounds.Merge( box );
                }

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

void Volume::Render( RenderVisitor* render )
{
    // pointer is drawn normalized
    if ( IsPointerVisible() )
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix.Normalized();
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Volume::DrawPointer;
    }

    // shape is drawn non-normalized
    {
        RenderEntry* entry = render->Allocate(this);
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();
        entry->m_Draw = &Volume::DrawShape;

        if ( m_TransparentOverride ? m_Transparent : Reflect::AssertCast<InstanceType>( m_NodeType )->IsTransparent() )
        {
            entry->m_Flags |= RenderFlags::DistanceSort;
        }
    }

    // don't call Base here, it will draw big ass axes
    HierarchyNode::Render( render );
}

void Volume::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Volume* volume = Reflect::AssertCast<Volume>( object );

    const VolumeType* type = Reflect::AssertCast<VolumeType>( volume->GetNodeType() );

    volume->SetMaterial( type->GetMaterial() );

    // draw type pointer
    type->GetPointer()->Draw( args );
}

void Volume::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const Volume* volume = Reflect::AssertCast<Volume>( object );

    const VolumeType* type = Reflect::AssertCast<VolumeType>( volume->GetNodeType() );

    volume->SetMaterial( type->GetMaterial() );

    const Primitive* prim = type->GetShape( volume->m_Shape );
    if (prim)
    {
        prim->Draw( args, volume->m_SolidOverride ? &volume->m_Solid : NULL, volume->m_TransparentOverride ? &volume->m_Transparent : NULL );
    }
}

bool Volume::Pick( PickVisitor* pick )
{
    bool result = false;

    VolumeType* type = Reflect::AssertCast<VolumeType>(m_NodeType);

    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
    result |= type->GetPointer()->Pick (pick);

    pick->SetCurrentObject (this, pick->State().m_Matrix);

    const Primitive* prim = type->GetShape( m_Shape );
    if (prim)
    {
        result |= prim->Pick(pick, m_SolidOverride ? &m_Solid : NULL);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Volume.
// 
bool Volume::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Volume" ) )
    {
        return true;
    }

    return Base::ValidatePanel( name );
}

void Volume::CreatePanel( CreatePanelArgs& args )
{
    args.m_Generator->PushContainer( TXT( "Volume" ) );
    {
        args.m_Generator->PushContainer();
        {
            const tstring helpText = TXT( "Select the shape of this volume." );
            args.m_Generator->AddLabel( TXT( "Shape" ) )->a_HelpText.Set( helpText );

            Inspect::Choice* choice = args.m_Generator->AddChoice<Volume, int>(args.m_Selection, &Volume::GetShape, &Volume::SetShape);
            choice->a_IsDropDown.Set( true );
            choice->a_HelpText.Set( helpText );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << VolumeShape::Cube;
                items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Cylinder;
                items.push_back( Inspect::ChoiceItem( TXT( "Cylinder" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Sphere;
                items.push_back( Inspect::ChoiceItem( TXT( "Sphere" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Capsule;
                items.push_back( Inspect::ChoiceItem( TXT( "Capsule" ), str.str() ) );
            }

            choice->a_Items.Set( items );
        }
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}

bool Volume::IsPointerVisible() const
{
    return m_ShowPointer; 
}

void Volume::SetPointerVisible(bool visible)
{
    m_ShowPointer = m_ShowPointer;

    // we need to dirty to cause our bounds needs to be re-computed
    Dirty();
}