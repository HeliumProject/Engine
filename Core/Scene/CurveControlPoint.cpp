/*#include "Precompile.h"*/
#include "CurveControlPoint.h"

#include "Core/Scene/Transform.h"
#include "Core/Scene/PropertiesGenerator.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT( CurveControlPoint );

void CurveControlPoint::EnumerateClass( Reflect::Compositor<CurveControlPoint>& comp )
{
    comp.AddField( &CurveControlPoint::m_Position, "m_Position" );
}

void CurveControlPoint::InitializeType()
{
    Reflect::RegisterClassType< CurveControlPoint >( TXT( "CurveControlPoint" ) );
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

i32 CurveControlPoint::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "point" ) );
}

tstring CurveControlPoint::GetApplicationTypeName() const
{
    return TXT( "CurveControlPoint" );
}

const Math::Vector3& CurveControlPoint::GetPosition() const
{
    return m_Position;
}

void CurveControlPoint::SetPosition( const Math::Vector3& value )
{
    m_Position = value;
    Dirty();
}

f32 CurveControlPoint::GetPositionX() const
{
    return GetPosition().x;
}

void CurveControlPoint::SetPositionX( f32 value )
{
    Math::Vector3 pos = GetPosition();
    pos.x = value;
    SetPosition( pos );
}

f32 CurveControlPoint::GetPositionY() const
{
    return GetPosition().y;
}

void CurveControlPoint::SetPositionY( f32 value )
{
    Math::Vector3 pos = GetPosition();
    pos.y = value;
    SetPosition( pos );
}

f32 CurveControlPoint::GetPositionZ() const
{
    return GetPosition().z;
}

void CurveControlPoint::SetPositionZ( f32 value )
{
    Math::Vector3 pos = GetPosition();
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
        static const tstring helpText = TXT( "Sets the position of this point in 3d space." );
        args.m_Generator->AddLabel( TXT( "Position" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, f32>( args.m_Selection, &CurveControlPoint::GetPositionX, &CurveControlPoint::SetPositionX )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, f32>( args.m_Selection, &CurveControlPoint::GetPositionY, &CurveControlPoint::SetPositionY )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<CurveControlPoint, f32>( args.m_Selection, &CurveControlPoint::GetPositionZ, &CurveControlPoint::SetPositionZ )->a_HelpText.Set( helpText );
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
            m_ObjectBounds.minimum = GetPosition() - Math::Vector3 (1.f,1.f,1.f);
            m_ObjectBounds.maximum = GetPosition() + Math::Vector3 (1.f,1.f,1.f);
            break;
        }
    }
}

Math::Matrix4 CurveControlPointTranslateManipulatorAdapter::GetFrame(ManipulatorSpace space)
{
    // base object manip frame
    Math::Matrix4 m = m_Point->GetTransform()->GetGlobalTransform();

    // if we are pivoting then just use the value
    m = Math::Matrix4 (GetValue()) * m;

    if (space == ManipulatorSpaces::Object)
    {
        return m;
    }
    else
    {
        Math::Matrix4 frame = Math::Matrix4::Identity;

        frame.t.x = m.t.x;
        frame.t.y = m.t.y;
        frame.t.z = m.t.z;

        return frame;
    }
}

Math::Matrix4 CurveControlPointTranslateManipulatorAdapter::GetObjectMatrix()
{
    return GetNode()->GetTransform()->GetGlobalTransform();
}

Math::Matrix4 CurveControlPointTranslateManipulatorAdapter::GetParentMatrix()
{
    return GetNode()->GetTransform()->GetGlobalTransform();
}
