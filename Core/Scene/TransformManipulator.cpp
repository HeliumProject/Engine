/*#include "Precompile.h"*/
#include "TransformManipulator.h"
#include "Core/Scene/Transform.h"

#include "Core/Scene/Viewport.h"
#include "Core/Scene/Camera.h"
#include "Color.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::TransformManipulator);

void TransformManipulator::InitializeType()
{
    Reflect::RegisterClassType< Core::TransformManipulator >( TXT( "Core::TransformManipulator" ) );
}

void TransformManipulator::CleanupType()
{
    Reflect::UnregisterClassType< Core::TransformManipulator >();
}

TransformManipulator::TransformManipulator(const ManipulatorMode mode, Core::Scene* scene, PropertiesGenerator* generator)
: Tool(scene, generator)
, m_Mode (mode)
, m_SelectedAxes (MultipleAxes::None)
, m_Left (false)
, m_Middle (false)
, m_Right (false)
, m_StartX (0)
, m_StartY (0)
, m_Manipulating (false)
, m_Manipulated (false)
{
    ZeroMemory(&m_AxisMaterial, sizeof(m_AxisMaterial));
    m_AxisMaterial.Ambient = Core::Color::BLACK;
    m_AxisMaterial.Diffuse = Core::Color::BLACK;
    m_AxisMaterial.Specular = Core::Color::BLACK;

    ZeroMemory(&m_SelectedAxisMaterial, sizeof(m_SelectedAxisMaterial));
    m_SelectedAxisMaterial.Ambient = Core::Color::YELLOW;
    m_SelectedAxisMaterial.Diffuse = Core::Color::BLACK;
    m_SelectedAxisMaterial.Specular = Core::Color::BLACK;

    m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate (this, &TransformManipulator::SelectionChanged) );

    SelectionChanged(m_Scene->GetSelection().GetItems());
}

TransformManipulator::~TransformManipulator()
{
    m_Scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate (this, &TransformManipulator::SelectionChanged) );
}

void TransformManipulator::Cleanup()
{
    SetResult();
}

void TransformManipulator::SelectionChanged(const SelectionChangeArgs& args)
{
    m_ManipulatorAdapters.clear();

    OS_PersistentDumbPtr::Iterator itr = args.m_Selection.Begin();
    OS_PersistentDumbPtr::Iterator end = args.m_Selection.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* h = Reflect::ObjectCast< Core::HierarchyNode >( *itr );
        if ( h )
        {
            h->ConnectManipulator(this);
        }
    }
}

bool TransformManipulator::SetAxisMaterial(AxesFlags axes)
{
    if ((m_SelectedAxes & axes) != MultipleAxes::None)
    {
        m_View->GetDevice()->SetMaterial(&m_SelectedAxisMaterial);
        return m_SelectedAxes != MultipleAxes::All;
    }
    else
    {
        switch (axes)
        {
        case MultipleAxes::X:
            m_AxisMaterial.Ambient = Core::Color::RED;
            break;

        case MultipleAxes::Y:
            m_AxisMaterial.Ambient = Core::Color::GREEN;
            break;

        case MultipleAxes::Z:
            m_AxisMaterial.Ambient = Core::Color::BLUE;
            break;
        }

        m_View->GetDevice()->SetMaterial(&m_AxisMaterial);
        return false;
    }
}

Vector3 TransformManipulator::GetAxesNormal(AxesFlags axes)
{
    Vector3 result;

    if (axes == MultipleAxes::All)
    {
        m_View->GetCamera()->GetDirection(result);

        return result;
    }

    if ((axes & MultipleAxes::X) != MultipleAxes::None)
    {
        if ((axes & MultipleAxes::Y) != MultipleAxes::None)
            return Vector3::BasisZ;

        if ((axes & MultipleAxes::Z) != MultipleAxes::None)
            return Vector3::BasisY;

        return Vector3::BasisX;
    }

    if ((axes & MultipleAxes::Y) != MultipleAxes::None)
    {
        if ((axes & MultipleAxes::X) != MultipleAxes::None)
            return Vector3::BasisZ;

        if ((axes & MultipleAxes::Z) != MultipleAxes::None)
            return Vector3::BasisX;

        return Vector3::BasisY;
    }

    if ((axes & MultipleAxes::Z) != MultipleAxes::None)
    {
        if ((axes & MultipleAxes::X) != MultipleAxes::None)
            return Vector3::BasisY;

        if ((axes & MultipleAxes::Y) != MultipleAxes::None)
            return Vector3::BasisX;

        return Vector3::BasisZ;
    }

    return result;
}

bool TransformManipulator::MouseDown( const MouseButtonInput& e )
{
    if (e.LeftDown())
        m_Left = true;

    if (e.MiddleDown())
        m_Middle = true;

    if (e.RightDown())
        m_Right = true;

    m_StartX = e.GetPosition().x;
    m_StartY = e.GetPosition().y;

    m_Manipulated = false;
    m_Manipulating = true;

    return true;
}

void TransformManipulator::MouseUp( const MouseButtonInput& e )
{
    m_Manipulating = false;

    if (m_Manipulated)
    {
        SetResult();
    }

    if (e.LeftUp())
        m_Left = false;

    if (e.MiddleUp())
        m_Middle = false;

    if (e.RightUp())
        m_Right = false;

    m_AllowSelection = true;
}

void TransformManipulator::MouseMove( const MouseMoveInput& e )
{
    if ( e.Dragging() )
    {
        // clear the current highlight
        m_Scene->ClearHighlight( ClearHighlightArgs (true) );

        // disallow selection when dragging
        m_AllowSelection = false;
    }
    else
    {
        // reset cached button state
        m_Left = m_Middle = m_Right = false;

        // allow selection when not dragging
        m_AllowSelection = true;
    }
}
