/*#include "Precompile.h"*/
#include "CurveEditTool.h"

#include "Core/Scene/Point.h"
#include "Core/Scene/Pick.h"

#include "TranslateManipulator.h"
#include "Curve.h"
#include "Core/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

SCENE_DEFINE_TYPE(Core::CurveEditTool);

void CurveEditTool::InitializeType()
{
    Reflect::RegisterClassType< Core::CurveEditTool >( TXT( "Core::CurveEditTool" ) );
}

void CurveEditTool::CleanupType()
{
    Reflect::UnregisterClassType< Core::CurveEditTool >();
}

CurveEditMode CurveEditTool::s_EditMode = CurveEditModes::Modify;
bool CurveEditTool::s_CurrentSelection = false;

CurveEditTool::CurveEditTool( SettingsManager* settingsManager, Core::Scene *scene, Core::PropertiesGenerator *generator)
: Tool( scene, generator )
, m_SettingsManager( settingsManager )
, m_HotEditMode ( CurveEditModes::None )
{
    Initialize();

    m_ControlPointManipulator = new Core::TranslateManipulator( m_SettingsManager, ManipulatorModes::Translate, scene, generator );
}

CurveEditTool::~CurveEditTool()
{
    delete m_ControlPointManipulator;
}

CurveEditMode CurveEditTool::GetEditMode() const
{
    return m_HotEditMode != CurveEditModes::None ? m_HotEditMode : s_EditMode;
}

bool CurveEditTool::MouseDown( const MouseButtonInput& e )
{
    bool success = true;

    if ( GetEditMode() == CurveEditModes::Modify )
    {
        success = m_ControlPointManipulator->MouseDown( e );
    }
    else
    {
        Curve* curve = NULL;

        {
            FrustumLinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), e.GetPosition().x, e.GetPosition().y);

            m_Scene->Pick( &pick );

            V_PickHitSmartPtr sorted;
            PickHit::Sort(m_Scene->GetViewport()->GetCamera(), pick.GetHits(), sorted, PickSortTypes::Intersection);

            for ( V_PickHitSmartPtr::const_iterator itr = sorted.begin(), end = sorted.end(); itr != end; ++itr )
            {
                if ( curve = Reflect::ObjectCast<Curve>( (*itr)->GetHitObject() ) )
                {
                    break;
                }
            }
        }

        if ( !curve || !m_Scene->IsEditable() )
        {
            return false;
        }

        LinePickVisitor pick (m_Scene->GetViewport()->GetCamera(), e.GetPosition().x, e.GetPosition().y);

        switch ( GetEditMode() )
        {
        case CurveEditModes::Insert:
            {
                std::pair<u32, u32> points;
                if ( !curve->ClosestControlPoints( &pick, points ) )
                {
                    return false;
                }

                Core::Point* p0 = curve->GetControlPointByIndex( points.first );
                Core::Point* p1 = curve->GetControlPointByIndex( points.second );

                Vector3 a( p0->GetPosition() );
                Vector3 b( p1->GetPosition() );
                Vector3 p;

                if ( curve->GetCurveType() == Content::CurveTypes::Linear )
                {
                    float mu;

                    if ( !pick.GetPickSpaceLine().IntersectsSegment( a, b, -1.0f, &mu ) )
                    {
                        return false;
                    }

                    p = a * ( 1.0f - mu ) + b * mu;
                }
                else
                {
                    p = ( a + b ) * 0.5f;
                }

                u32 index = points.first > points.second ? points.first : points.second;

                PointPtr point = new Core::Point( m_Scene, new Content::Point( p ) );

                curve->GetOwner()->Push( curve->InsertControlPointAtIndex( index, point ) );
                break;
            }

        case CurveEditModes::Remove:
            {
                i32 index = curve->ClosestControlPoint( &pick );

                if ( index < 0 )
                {
                    return false;
                }

                curve->GetOwner()->Push( curve->RemoveControlPointAtIndex( index ) );
                break;
            }
        }

        curve->Dirty();

        m_Scene->Execute( false );
    }

    return success || __super::MouseDown( e );
}

void CurveEditTool::MouseUp( const MouseButtonInput& e )
{
    if ( GetEditMode() )
    {
        m_ControlPointManipulator->MouseUp( e );
    }

    m_AllowSelection = true;

    __super::MouseUp( e );
}

void CurveEditTool::MouseMove( const MouseMoveInput& e )
{
    if ( GetEditMode() )
    {
        m_ControlPointManipulator->MouseMove( e );

        if ( e.Dragging() )
        {
            // clear the current highlight
            m_Scene->ClearHighlight( ClearHighlightArgs(true) );

            // disallow selection when dragging
            m_AllowSelection = false;
        }
    }

    __super::MouseMove( e );
}

void CurveEditTool::KeyPress( const KeyboardInput& e )
{
    if ( !m_Scene->IsEditable() )
    {
        return;
    }

    i32 keyCode = e.GetKeyCode();

    if ( keyCode == KeyCodes::Left || keyCode == KeyCodes::Up || keyCode == KeyCodes::Right || keyCode == KeyCodes::Down )
    {
        OS_SelectableDumbPtr selection = m_Scene->GetSelection().GetItems();

        if ( selection.Empty() )
        {
            return;
        }

        Core::Point* point = Reflect::ObjectCast<Core::Point>( selection.Front() );

        if ( !point )
        {
            return;
        }

        Core::Curve* curve = Reflect::ObjectCast<Core::Curve>( point->GetParent() );

        if ( !curve )
        {
            return;
        }

        i32 index =  curve->GetIndexForControlPoint( point );

        if ( index == -1 )
        {
            return;
        }

        u32 countControlPoints = curve->GetNumberControlPoints();
        if ( keyCode == KeyCodes::Left || keyCode == KeyCodes::Down )
        {
            index--;
            index += countControlPoints;
            index %= countControlPoints;
        }
        else if ( keyCode == KeyCodes::Right || keyCode == KeyCodes::Up ) 
        {
            index++;
            index %= countControlPoints;
        }

        point = curve->GetControlPointByIndex( index );

        selection.Clear();
        selection.Append( point );
        m_Scene->GetSelection().SetItems( selection );
    }

    __super::KeyPress( e );
}

void CurveEditTool::KeyDown( const KeyboardInput& e )
{
    CurveEditMode mode = m_HotEditMode;

    switch (e.GetKeyCode())
    {
    case TXT('M'):
        m_HotEditMode = CurveEditModes::Modify;
        break;

    case TXT('I'):
        m_HotEditMode = CurveEditModes::Insert;
        break;

    case TXT('R'):
        m_HotEditMode = CurveEditModes::Remove;
        break;

    default:
        __super::KeyDown( e );
        break;
    }

    if ( mode != m_HotEditMode )
    {
        m_Generator->GetContainer()->Read();
    }
}

void CurveEditTool::KeyUp( const KeyboardInput& e )
{
    CurveEditMode mode = m_HotEditMode;

    switch (e.GetKeyCode())
    {
    case TXT('M'):
    case TXT('I'):
    case TXT('R'):
        m_HotEditMode = CurveEditModes::None;
        break;

    default:
        __super::KeyUp( e );
        break;
    }

    if ( mode != m_HotEditMode )
    {
        m_Generator->GetContainer()->Read();
    }
}

bool CurveEditTool::ValidateSelection( OS_SelectableDumbPtr& items )
{
    OS_SelectableDumbPtr result;

    OS_SelectableDumbPtr::Iterator itr = items.Begin();
    OS_SelectableDumbPtr::Iterator end = items.End();
    for( ; itr != end; ++itr )
    {
        Core::Point* p = Reflect::ObjectCast<Core::Point>( *itr );

        if ( !p )
        {
            continue;
        }

        bool appendPoint = true;
        if ( s_CurrentSelection )
        {
            appendPoint = false;
            OS_SelectableDumbPtr::Iterator curveItr = m_SelectedCurves.Begin();
            OS_SelectableDumbPtr::Iterator curveEnd = m_SelectedCurves.End();
            for ( ; curveItr != curveEnd; ++curveItr )
            {
                if ( p->GetParent() == *curveItr )
                {
                    appendPoint = true;
                    break;
                }
            }
        }

        if ( appendPoint )
        {
            result.Append( p );
        }
    }

    items = result;

    if ( items.Empty() )
    {
        OS_SelectableDumbPtr::Iterator itr = items.Begin();
        OS_SelectableDumbPtr::Iterator end = items.End();
        for( ; itr != end; ++itr )
        {
            Core::Curve* c = Reflect::ObjectCast<Core::Curve>( *itr );

            if ( !c )
            {
                continue;
            }

            result.Append( c );
            break;
        }
    }

    items = result;

    return !items.Empty();
}

void CurveEditTool::Evaluate()
{
    if ( GetEditMode() )
    {
        m_ControlPointManipulator->Evaluate();
    }

    __super::Evaluate();
}

void CurveEditTool::Draw( DrawArgs* args )
{
    if ( GetEditMode() )
    {
        m_ControlPointManipulator->Draw( args );
    }

    __super::Draw( args );
}

void CurveEditTool::CreateProperties()
{
    __super::CreateProperties();

    m_Generator->PushContainer( TXT( "Edit Curve" ) );
    {
        m_Generator->PushContainer();
        { 
            m_Generator->AddLabel( TXT( "Edit Control Points" ) );
            Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Core::CurveEditTool, int> (this, &CurveEditTool::GetCurveEditMode, &CurveEditTool::SetCurveEditMode ) );
            choice->a_IsDropDown.Set( true );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << CurveEditModes::Modify;
                items.push_back( Inspect::ChoiceItem( TXT( "Modify Points" ), str.str() ) );
            }

            {
                tostringstream str;
                str << CurveEditModes::Insert;
                items.push_back( Inspect::ChoiceItem( TXT( "Insert Points" ), str.str() ) );
            }

            {
                tostringstream str;
                str << CurveEditModes::Remove;
                items.push_back( Inspect::ChoiceItem( TXT( "Remove Points" ), str.str() ) );
            }
            choice->a_Items.Set( items );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        { 
            m_Generator->AddLabel( TXT( "Selected Curves Only" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Core::CurveEditTool, bool> (this, &CurveEditTool::GetSelectionMode, &CurveEditTool::SetSelectionMode ) );
        }
        m_Generator->Pop();
    }
    m_Generator->Pop();

    m_ControlPointManipulator->CreateProperties();
}

int CurveEditTool::GetCurveEditMode() const
{
    return s_EditMode;
}

void CurveEditTool::SetCurveEditMode( int mode )
{
    s_EditMode = (CurveEditMode)mode;

    m_Scene->GetSelection().Clear();
}

bool CurveEditTool::GetSelectionMode() const
{
    return s_CurrentSelection;
}

void CurveEditTool::SetSelectionMode( bool mode )
{
    s_CurrentSelection = mode;
}

void CurveEditTool::StoreSelectedCurves()
{
    m_SelectedCurves.Clear();
    OS_SelectableDumbPtr::Iterator selection_itr = m_Scene->GetSelection().GetItems().Begin();
    OS_SelectableDumbPtr::Iterator selection_end = m_Scene->GetSelection().GetItems().End();
    for ( ; selection_itr != selection_end; ++selection_itr )
    {
        Core::Curve* curve = Reflect::ObjectCast<Core::Curve>( *selection_itr );
        if ( curve )
        {
            m_SelectedCurves.Append( curve );
        }
    }
}
