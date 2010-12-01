/*#include "Precompile.h"*/
#include "CurveControlPoint.h"

#include "Pipeline/SceneGraph/Transform.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_CLASS( CurveControlPoint );

void CurveControlPoint::EnumerateClass( Reflect::Compositor<CurveControlPoint>& comp )
{
    comp.AddField( &CurveControlPoint::m_Position, "m_Position" );
}

void CurveControlPoint::InitializeType()
{
    Reflect::RegisterClassType< CurveControlPoint >( TXT( "SceneGraph::CurveControlPoint" ) );
    PropertiesGenerator::InitializePanel( TXT( "CurveControlPoint" ), CreatePanelSignature::Delegate( &CurveControlPoint::CreatePanel ) );
}

void CurveControlPoint::CleanupType()
{
    Reflect::UnregisterClassType< CurveControlPoint >();
}

CurveControlPoint::CurveControlPoint() 
{
}

CurveControlPoint::~CurveControlPoint()
{
}

int32_t CurveControlPoint::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "point" ) );
}

tstring CurveControlPoint::GetApplicationTypeName() const
{
    return TXT( "CurveControlPoint" );
}

const Vector3& CurveControlPoint::GetPosition() const
{
    return m_Position;
}

void CurveControlPoint::SetPosition( const Vector3& value )
{
    m_Position = value;
    Dirty();
}

float32_t CurveControlPoint::GetPositionX() const
{
    return GetPosition().x;
}

void CurveControlPoint::SetPositionX( float32_t value )
{
    Vector3 pos = GetPosition();
    pos.x = value;
    SetPosition( pos );
}

float32_t CurveControlPoint::GetPositionY() const
{
    return GetPosition().y;
}

void CurveControlPoint::SetPositionY( float32_t value )
{
    Vector3 pos = GetPosition();
    pos.y = value;
    SetPosition( pos );
}

float32_t CurveControlPoint::GetPositionZ() const
{
    return GetPosition().z;
}

void CurveControlPoint::SetPositionZ( float32_t value )
{
    Vector3 pos = GetPosition();
    pos.z = value;
    SetPosition( pos );
}

void CurveControlPoint::ConnectManipulator(ManiuplatorAdapterCollection *collection)
{
    switch( collection->GetMode() )
    {
    case ManipulatorModes::Translate:
        {
            collection->AddManipulatorAdapter( new CurveControlPointTranslateManipulatorAdapter( this ) );
            break;
        }

    default:
        {
            break;
        }
    }
}

bool CurveControlPoint::ValidatePanel( const tstring& name )
{
    if ( name == TXT( "CurveControlPoint" ) )
    {
        return true;
    }

    return __super::ValidatePanel( name );
}

void CurveControlPoint::CreatePanel( CreatePanelArgs& args )
{
    args.m_Generator->PushContainer( TXT( "CurveControlPoint" ) );
    {
        args.m_Generator->PushContainer();
        const tstring helpText = TXT( "Sets the position of this point in 3d space." );
        args.m_Generator->AddLabel( TXT( "Position" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, float32_t>( args.m_Selection, &CurveControlPoint::GetPositionX, &CurveControlPoint::SetPositionX )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, float32_t>( args.m_Selection, &CurveControlPoint::GetPositionY, &CurveControlPoint::SetPositionY )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, float32_t>( args.m_Selection, &CurveControlPoint::GetPositionZ, &CurveControlPoint::SetPositionZ )->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }
    args.m_Generator->Pop();
}

bool CurveControlPoint::Pick( PickVisitor* pick )
{
    return pick->PickPoint( GetPosition() );
}

void CurveControlPoint::Evaluate( GraphDirection direction )
{
    __super::Evaluate(direction);

    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            m_ObjectBounds.minimum = GetPosition() - Vector3 (1.f,1.f,1.f);
            m_ObjectBounds.maximum = GetPosition() + Vector3 (1.f,1.f,1.f);
            break;
        }
    }
}

Matrix4 CurveControlPointTranslateManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    // base object manip frame
    Matrix4 m = m_Point->GetTransform()->GetGlobalTransform();

    // if we are pivoting then just use the value
    m = Matrix4 (GetValue()) * m;

    if (space == ManipulatorSpace::Object)
    {
        return m;
    }
    else
    {
        Matrix4 frame = Matrix4::Identity;

        frame.t.x = m.t.x;
        frame.t.y = m.t.y;
        frame.t.z = m.t.z;

        return frame;
    }
}

Matrix4 CurveControlPointTranslateManipulatorAdapter::GetObjectMatrix()
{
    return GetNode()->GetTransform()->GetGlobalTransform();
}

Matrix4 CurveControlPointTranslateManipulatorAdapter::GetParentMatrix()
{
    return GetNode()->GetTransform()->GetGlobalTransform();
}
