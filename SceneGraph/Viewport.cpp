#include "SceneGraphPch.h"
#include "Viewport.h"

#include "Platform/Exception.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/CameraMovedCommand.h"
#include "SceneGraph/Color.h"
#include "SceneGraph/Pick.h"
#include "SceneGraph/PrimitiveAxes.h"
#include "SceneGraph/PrimitiveFrame.h"
#include "SceneGraph/PrimitiveGrid.h"
#include "SceneGraph/PrimitiveRings.h"
#include "SceneGraph/SceneSettings.h"
#include "SceneGraph/Statistics.h"
#include "SceneGraph/Orientation.h"
#include "SceneGraph/Tool.h"
#include "SceneGraph/GridSettings.h"

#include "Rendering/RRenderContext.h"

using namespace Helium;
using namespace Helium::SceneGraph;

const Helium::Color Viewport::s_LiveMaterial = SceneGraph::Color::MAGENTA;
const Helium::Color Viewport::s_SelectedMaterial = SceneGraph::Color::SPRINGGREEN;
const Helium::Color Viewport::s_ReactiveMaterial = SceneGraph::Color::WHITE;
const Helium::Color Viewport::s_HighlightedMaterial = SceneGraph::Color::CYAN;
const Helium::Color Viewport::s_UnselectableMaterial = SceneGraph::Color::GRAY;
const Helium::Color Viewport::s_ComponentMaterial = SceneGraph::Color::MAGENTA;
const Helium::Color Viewport::s_SelectedComponentMaterial = SceneGraph::Color::YELLOW;
const Helium::Color Viewport::s_RedMaterial = SceneGraph::Color::RED;
const Helium::Color Viewport::s_YellowMaterial = SceneGraph::Color::YELLOW;
const Helium::Color Viewport::s_GreenMaterial = SceneGraph::Color::GREEN;
const Helium::Color Viewport::s_BlueMaterial = SceneGraph::Color::BLUE;

Viewport::Viewport( HWND wnd, SettingsManager* settingsManager, World* world )
: m_Window( wnd )
, m_SettingsManager( settingsManager )
, m_World( world )
, m_SceneViewId( Invalid< uint32_t >() )
, m_Focused( false )
, m_Tool( NULL )
, m_CameraMode( CameraMode::Orbit )
, m_GeometryMode( GeometryMode::Render )
, m_DragMode( DragModes::None )
, m_Highlighting( true )
, m_AxesVisible( true )
, m_GridVisible( true )
, m_BoundsVisible( false )
, m_StatisticsVisible( false )
, m_Statistics( NULL )
, m_SelectionFrame( NULL )
{
    memset(m_GlobalPrimitives, NULL, sizeof(m_GlobalPrimitives));

    InitWidgets();
    InitCameras();

    Reset();

    OnResize();
}

Viewport::~Viewport()
{
    m_Cameras[ CameraMode::Orbit ].RemoveMovedListener( CameraMovedSignature::Delegate ( this, &Viewport::CameraMoved ) );

    for (uint32_t i=0; i<GlobalPrimitives::Count; i++)
        delete m_GlobalPrimitives[i];

    delete m_Statistics;
    delete m_SelectionFrame;
}

void Viewport::Reset()
{
#ifdef VIEWPORT_REFACTOR
    if ( !GetDevice() )
    {
        return;
    }

    for (uint32_t i=0; i<CameraMode::Count; i++)
    {
        m_Cameras[i].Reset();
    }

    static_cast<SceneGraph::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->m_Length = 0.05f;
    static_cast<SceneGraph::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->Update();

    SceneGraph::PrimitiveAxes* transformAxes = static_cast< SceneGraph::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::TransformAxes] );
    transformAxes->m_Length = 0.10f;
    transformAxes->Update();

    SceneGraph::PrimitiveAxes* transformAxesSelected = static_cast< SceneGraph::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] );
    transformAxesSelected->m_Length = 0.10f;
    transformAxesSelected->SetColor( s_SelectedMaterial );
    transformAxesSelected->Update();

    SceneGraph::PrimitiveAxes* jointAxes = static_cast< SceneGraph::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::JointAxes] );
    jointAxes->m_Length = 0.015f;
    jointAxes->Update();

    SceneGraph::PrimitiveRings* jointRings = static_cast< SceneGraph::PrimitiveRings* >( m_GlobalPrimitives[GlobalPrimitives::JointRings] );
    jointRings->m_Radius = 0.015f;
    jointRings->m_Steps = 18;
    jointRings->Update();

    m_AxesVisible = true;
    m_GridVisible = true;
    m_BoundsVisible = false;
    m_StatisticsVisible = false;

#ifdef _DEBUG
    m_StatisticsVisible = true;
#endif

#endif
}

void Viewport::LoadSettings(SceneGraph::ViewportSettings* prefs)
{
    // apply settings for all modes that we have... 
    for(size_t i = 0; i < prefs->m_CameraPrefs.size(); ++i)
    {
        CameraSettingsPtr cameraPrefs = prefs->m_CameraPrefs[i]; 
        CameraMode mode = cameraPrefs->m_CameraMode; 
        SceneGraph::Camera* camera = GetCameraForMode(mode); 
        camera->LoadSettings(cameraPrefs); 
    }

    SetCameraMode( prefs->m_CameraMode ); 
    SetGeometryMode( prefs->m_GeometryMode ); 
    SetHighlighting( prefs->m_Highlighting ); 
    SetAxesVisible( prefs->m_AxesVisible ); 
    SetGridVisible( prefs->m_GridVisible ); 
    SetBoundsVisible( prefs->m_BoundsVisible ); 
    SetStatisticsVisible( prefs->m_StatisticsVisible ); 
}

void Viewport::SaveSettings(SceneGraph::ViewportSettings* prefs)
{
    // just blow away the previous preferences
    prefs->m_CameraPrefs.clear(); 

    for(int i = 0; i < CameraMode::Count; ++i)
    {
        CameraMode mode = static_cast< CameraMode::Enum >( i ); 
        CameraSettingsPtr cameraPrefs = new CameraSettings(); 
        cameraPrefs->m_CameraMode = mode;
        SceneGraph::Camera* camera = GetCameraForMode( mode ); 
        camera->SaveSettings(cameraPrefs); 
        prefs->m_CameraPrefs.push_back( cameraPrefs ); 
    }

    prefs->m_CameraMode = GetCameraMode(); 
    prefs->m_GeometryMode = GetGeometryMode(); 
    prefs->m_Highlighting = IsHighlighting(); 
    prefs->m_AxesVisible = IsAxesVisible(); 
    prefs->m_GridVisible = IsGridVisible(); 
    prefs->m_BoundsVisible = IsBoundsVisible(); 
    prefs->m_StatisticsVisible = IsStatisticsVisible(); 
}

void Viewport::SetCameraMode(CameraMode mode)
{
    if ( mode != m_CameraMode )
    {
        CameraMode old = m_CameraMode;
        m_CameraMode = mode;

        m_CameraModeChanged.Raise( CameraModeChangeArgs( old, m_CameraMode ) );
    }
}

void Viewport::NextCameraMode()
{
    SetCameraMode( static_cast< CameraMode::Enum >( ((m_CameraMode + 1) % CameraMode::Count)) );
}

void Viewport::PreviousCameraMode()
{
    SetCameraMode( static_cast< CameraMode::Enum >( ((m_CameraMode + (CameraMode::Count-1)) % CameraMode::Count)) );
}

void Viewport::NextGeometryMode()
{
    SetGeometryMode( static_cast< GeometryMode::Enum >( ((m_GeometryMode + 1) % GeometryMode::Count)) );
}

void Viewport::SetTool(SceneGraph::Tool* tool)
{
    if ( m_Tool != tool )
    {
        m_Tool = tool;
        m_ToolChanged.Raise( ToolChangeArgs( m_Tool ) );
    }

    //make sure if we are in the highlight tool that we are in highlight drag mode
    if (!m_Tool)
    {
        m_DragMode = DragModes::None;
    }
}

bool Viewport::IsHighlighting() const
{
    return m_Highlighting;
}

void Viewport::SetHighlighting(bool highlight)
{
    m_Highlighting = highlight;

    m_DragMode = DragModes::None;

    if (!m_Highlighting)
    {
        m_ClearHighlight.Raise( ClearHighlightArgs (true) );
    }
}

SceneGraph::Primitive* Viewport::GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which )
{
    SceneGraph::Primitive* prim = NULL;
    if ( which >= 0 && which < GlobalPrimitives::Count )
    {
        prim = m_GlobalPrimitives[which];
    }
    return prim;
}

void Viewport::InitWidgets()
{
    // primitive API uses this, so init it first
    m_Statistics = new Statistics();

    m_GlobalPrimitives[GlobalPrimitives::ViewportAxes] = new SceneGraph::PrimitiveAxes;
    m_GlobalPrimitives[GlobalPrimitives::ViewportAxes]->Update();

    m_GlobalPrimitives[GlobalPrimitives::StandardAxes] = new SceneGraph::PrimitiveAxes;
    m_GlobalPrimitives[GlobalPrimitives::StandardAxes]->Update();

    m_GlobalPrimitives[GlobalPrimitives::StandardGrid] = new SceneGraph::PrimitiveGrid;

    m_SettingsManager->GetSettings< GridSettings >()->e_Changed.Add( Reflect::ObjectChangeSignature::Delegate( this, &Viewport::OnGridSettingsChanged ));

    OnGridSettingsChanged( Reflect::ObjectChangeArgs( NULL, NULL ) );

    m_GlobalPrimitives[GlobalPrimitives::StandardRings] = new SceneGraph::PrimitiveRings;
    m_GlobalPrimitives[GlobalPrimitives::StandardRings]->Update();

    m_GlobalPrimitives[GlobalPrimitives::TransformAxes] = new SceneGraph::PrimitiveAxes;
    m_GlobalPrimitives[GlobalPrimitives::TransformAxes]->Update();

    m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] = new SceneGraph::PrimitiveAxes;
    m_GlobalPrimitives[GlobalPrimitives::SelectedAxes]->Update();

    m_GlobalPrimitives[GlobalPrimitives::JointAxes] = new SceneGraph::PrimitiveAxes;
    m_GlobalPrimitives[GlobalPrimitives::JointAxes]->Update();

    m_GlobalPrimitives[GlobalPrimitives::JointRings] = new SceneGraph::PrimitiveRings;
    m_GlobalPrimitives[GlobalPrimitives::JointRings]->Update();

    m_SelectionFrame = new SceneGraph::PrimitiveFrame;
    m_SelectionFrame->Update();
}

void Viewport::InitCameras()
{
    // create the cameras
    m_Cameras[CameraMode::Orbit].Setup(ProjectionModes::Perspective, Vector3::Zero, Vector3::Zero);
    m_Cameras[CameraMode::Front].Setup(ProjectionModes::Orthographic, OutVector * -1.f, UpVector);
    m_Cameras[CameraMode::Side].Setup(ProjectionModes::Orthographic, SideVector * -1.f, UpVector);
    m_Cameras[CameraMode::Top].Setup(ProjectionModes::Orthographic, UpVector * -1.f, OutVector * -1.f);

    // Set the max size of the camera history
    m_CameraHistory[CameraMode::Orbit].SetMaxLength( 10 );
    m_CameraHistory[CameraMode::Front].SetMaxLength( 10 );
    m_CameraHistory[CameraMode::Side].SetMaxLength( 10 );
    m_CameraHistory[CameraMode::Top].SetMaxLength( 10 );

    m_Cameras[ CameraMode::Orbit ].AddMovedListener( CameraMovedSignature::Delegate ( this, &Viewport::CameraMoved ) );

    if (m_World)
    {
        GraphicsScene* pGraphicsScene = m_World->GetGraphicsScene();
        m_SceneViewId = pGraphicsScene->AllocateSceneView();
    }
}

void Viewport::OnResize()
{
    const uint32_t width = (m_Size.x > 0) ? m_Size.x : 64;
    const uint32_t height = (m_Size.y > 0) ? m_Size.y : 64;
    const float32_t aspectRatio =
        static_cast< float32_t >( width ) / static_cast< float32_t >( height );

    if (m_World)
    {
        Renderer* pRenderer = Renderer::GetStaticInstance();

        Renderer::ContextInitParameters ctxParams;
        ctxParams.pWindow = m_Window;
        ctxParams.displayWidth = width;
        ctxParams.displayHeight = height;
        ctxParams.bFullscreen = false;
        ctxParams.bVsync = false;

        RRenderContextPtr renderCtx = pRenderer->CreateSubContext( ctxParams );

        GraphicsScene* pGraphicsScene = m_World->GetGraphicsScene();
        GraphicsSceneView* pSceneView = pGraphicsScene->GetSceneView( m_SceneViewId );
        pSceneView->SetRenderContext( renderCtx );
        pSceneView->SetDepthStencilSurface( RenderResourceManager::GetStaticInstance().GetDepthStencilSurface() );
        pSceneView->SetAspectRatio( aspectRatio );
        pSceneView->SetViewport( 0, 0, width, height );
        pSceneView->SetClearColor( Helium::Color( 0x00505050 ) );
    }
}

void Viewport::SetSize(uint32_t x, uint32_t y)
{
    m_Size.x = x;
    m_Size.y = y;

    OnResize();
}

void Viewport::SetFocused(bool focused)
{
    m_Focused = focused;
}

void Viewport::KeyDown(const Helium::KeyboardInput& input)
{
    if ( m_Tool )
    {
        m_Tool->KeyDown( input );
    }
    else
    {
        input.Skip();
    }
}

void Viewport::KeyUp(const Helium::KeyboardInput& input)
{
    if ( m_Tool )
    {
        m_Tool->KeyUp( input );
    }
    else
    {
        input.Skip();
    }
}

void Viewport::KeyPress(const Helium::KeyboardInput& input)
{
    if ( m_Tool )
    {
        m_Tool->KeyPress( input );
    }
    else
    {
        input.Skip();
    }
}

void Viewport::MouseDown(const Helium::MouseButtonInput& input)
{
    // reset drag mode
    m_DragMode = DragModes::None;

    // are we going to allow entering select drag?
    bool allowSelection = true;

    // if the camera modifier key is down
    if (input.AltIsDown())
    {
        m_Cameras[m_CameraMode].MouseDown( input );

        // camera now owns the drag
        m_DragMode = DragModes::Camera;

        // do NOT allow selection while moving the camera
        allowSelection = false;

        // Save the previous view before it is updated
        UpdateCameraHistory();   
    }
    else if (m_Tool) // else if we have a tool object
    {
        // hit test the tool, and if we intersect or its a tool modifier mouse key
        if (m_Tool->MouseDown( input ) || input.MiddleDown() || input.RightDown())
        {
            // the tool now owns this drag
            m_DragMode = DragModes::Tool;
        }
        else
        {
            // we can still allow selection if it didn't hit he tool
            allowSelection = m_Tool->AllowSelection();
        }
    }

    // if its the left key if we are still looking for a selection
    if ( (input.LeftIsDown() || input.MiddleIsDown()) && m_DragMode == DragModes::None && allowSelection )
    {
        // we are selecting
        m_DragMode = DragModes::Select;

        // reset point trackers
        m_Start = Point (input.GetPosition().x, input.GetPosition().y);
        m_End = Point (input.GetPosition().x, input.GetPosition().y);

        // reset selection frame
        m_SelectionFrame->m_Start = m_Start;
        m_SelectionFrame->m_End = m_End;
        m_SelectionFrame->Update();

        // if we are highlighting
        if ( m_Highlighting )
        {
            // clear the previously highlighted set
            m_ClearHighlight.Raise( ClearHighlightArgs (false) );
        }
    }
}

void Viewport::MouseUp( const Helium::MouseButtonInput& input )
{
    // have we completed a drag?
    bool dragComplete = false;

    switch (m_DragMode)
    {
    case DragModes::None:
        {
            if (!m_Tool && input.RightUp())
            {
                m_End = Point (input.GetPosition().x, input.GetPosition().y);
                m_SelectionFrame->m_End = m_End;
                m_SelectionFrame->Update();

                PickVisitor* pick = NULL;

                SelectionTargetMode targetMode = SelectionTargetModes::Single;

                if (m_SelectionFrame->m_Start.x == m_SelectionFrame->m_End.x && m_SelectionFrame->m_Start.y == m_SelectionFrame->m_End.y)
                {
                    pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y);
                    targetMode = SelectionTargetModes::Single;
                }
                else 
                {
                    Frustum worldSpaceFrustum;
                    if ( m_Cameras[m_CameraMode].ViewportToFrustum( (float32_t)m_SelectionFrame->m_Start.x, (float32_t)m_SelectionFrame->m_Start.y, (float32_t)m_SelectionFrame->m_End.x, (float32_t)m_SelectionFrame->m_End.y, worldSpaceFrustum) )
                    {
                        Point center ( m_SelectionFrame->m_Start + m_SelectionFrame->m_End / 2 );

                        Line line;
                        m_Cameras[m_CameraMode].ViewportToLine( (float32_t)center.x, (float32_t)center.y, line);

                        pick = new FrustumLinePickVisitor(&m_Cameras[m_CameraMode], line, worldSpaceFrustum );
                        targetMode = SelectionTargetModes::Multiple;
                    }                   
                }

                if ( pick )
                {
                    m_Select.Raise( SelectArgs (pick, SelectionModes::Manifest, targetMode) );
                    delete pick;
                    pick = NULL;
                }

                // our drag is complete now
                dragComplete = true;
            }

            break;
        }

    case DragModes::Select:
        {
            if (input.LeftUp() || input.MiddleUp())
            {
                m_End = Point (input.GetPosition().x, input.GetPosition().y);
                m_SelectionFrame->m_End = m_End;
                m_SelectionFrame->Update();

                PickVisitor* pick = NULL;

                SelectionTargetMode targetMode = SelectionTargetModes::Single;

                if (m_SelectionFrame->m_Start.x == m_SelectionFrame->m_End.x && m_SelectionFrame->m_Start.y == m_SelectionFrame->m_End.y)
                {
                    pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], m_SelectionFrame->m_Start.x, m_SelectionFrame->m_Start.y);
                    targetMode = SelectionTargetModes::Single;
                }
                else 
                {
                    Frustum worldSpaceFrustum;

                    if ( m_Cameras[m_CameraMode].ViewportToFrustum( (float32_t)m_SelectionFrame->m_Start.x, (float32_t)m_SelectionFrame->m_Start.y, (float32_t)m_SelectionFrame->m_End.x, (float32_t)m_SelectionFrame->m_End.y, worldSpaceFrustum) )
                    {
                        Point center ( m_SelectionFrame->m_Start + m_SelectionFrame->m_End / 2 );

                        Line line;
                        m_Cameras[m_CameraMode].ViewportToLine( (float32_t)center.x, (float32_t)center.y, line);

                        pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], line, worldSpaceFrustum); 
                        targetMode = SelectionTargetModes::Multiple;
                    }
                }

                if ( pick )
                {
                    SelectionModes::SelectionMode selectMode;

                    if (input.MiddleUp())
                    {
                        selectMode = SelectionModes::Manifest;
                    }
                    else
                    {
                        if ( input.CtrlIsDown() )
                        {
                            if ( input.ShiftIsDown())
                            {
                                selectMode = SelectionModes::Add;
                            }
                            else
                            {
                                selectMode = SelectionModes::Remove;
                            }
                        }
                        else
                        {
                            if ( input.ShiftIsDown())
                            {
                                selectMode = SelectionModes::Toggle;
                            }
                            else
                            {
                                selectMode = SelectionModes::Replace;
                            }
                        }
                    }

                    m_Select.Raise( SelectArgs (pick, selectMode, targetMode) );
                    delete pick;
                    pick = NULL;
                }

                // our drag is complete now
                dragComplete = true;
            }

            break;
        }

    case DragModes::Camera:
        {
            if (input.AltIsDown())
            {
                m_Cameras[m_CameraMode].MouseUp( input );
            }

            break;
        }

    default:
        {
            if (m_Tool)
            {
                m_Tool->MouseUp( input );
            }

            break;
        }
    }

    if (dragComplete)
    {
        m_DragMode = DragModes::None;
    }
}

void Viewport::MouseMove( const Helium::MouseMoveInput& input )
{
    // this sucks, be we don't get a keyup for alt
    if (m_DragMode == DragModes::Camera && !input.AltIsDown())
    {
        m_DragMode = DragModes::None;
    }

    int highlightStartX = -1, highlightStartY = -1, highlightEndX = -1, highlightEndY = -1;

    switch (m_DragMode)
    {
    case DragModes::None:
        {
            // setup highlight
            if ( m_Highlighting && (!m_Tool || m_Tool->AllowSelection()) )
            {
                highlightStartX = highlightEndX = input.GetPosition().x;
                highlightStartY = highlightEndY = input.GetPosition().y;
            }

            if ( m_Tool )
            {
                m_Tool->MouseMove( input );
            }

            break;
        }

    case DragModes::Select:
        {
            m_End = Point (input.GetPosition().x, input.GetPosition().y);
            m_SelectionFrame->m_End = m_End;
            m_SelectionFrame->Update();

            if ( !m_Tool || m_Tool->AllowSelection() )
            {
                // setup highlight
                highlightStartX = m_SelectionFrame->m_Start.x;
                highlightStartY = m_SelectionFrame->m_Start.y;
                highlightEndX = input.GetPosition().x;
                highlightEndY = input.GetPosition().y;
            }

            break;
        }

    case DragModes::Camera:
        {
            if (input.AltIsDown())
            {
                m_Cameras[m_CameraMode].MouseMove( input );
            }

            break;
        }

    case DragModes::Tool:
        {
            if (m_Tool)
            {
                if ( m_Tool->AllowSelection() )
                { 
                    // setup highlight
                    highlightStartX = highlightEndX = input.GetPosition().x;
                    highlightStartY = highlightEndY = input.GetPosition().y;
                }

                m_Tool->MouseMove( input );
            }
            else
            {
                HELIUM_BREAK();
            }

            break;
        }
    }

    if (highlightStartX != -1 && highlightStartY != -1 && highlightEndX != -1 && highlightEndY != -1)
    {
        PickVisitor* pick = NULL;
        SelectionTargetMode targetMode = SelectionTargetModes::Single;

        if (highlightStartX == highlightEndX && highlightStartY == highlightEndY)
        {         
            pick = new FrustumLinePickVisitor (&m_Cameras[m_CameraMode], highlightStartX, highlightStartY);
            targetMode = SelectionTargetModes::Single;
        }
        else 
        {
            Frustum worldSpaceFrustum;

            if ( m_Cameras[m_CameraMode].ViewportToFrustum( (float32_t)highlightStartX, (float32_t)highlightStartY, (float32_t)highlightEndX, (float32_t)highlightEndY, worldSpaceFrustum) )
            {
                Point center ( Point (highlightStartX, highlightStartY) + Point (highlightEndX, highlightEndY) / 2 );

                Line line;
                m_Cameras[m_CameraMode].ViewportToLine( (float32_t)center.x, (float32_t)center.y, line);

                pick = new FrustumLinePickVisitor( &m_Cameras[m_CameraMode], line, worldSpaceFrustum );
                targetMode = SelectionTargetModes::Multiple;
            }
        }

        if (pick)
        {
            SetHighlightArgs args (pick, targetMode);
            m_SetHighlight.Raise( args );
            delete pick;
            pick = NULL;
        }
    }
}

void Viewport::MouseScroll( const Helium::MouseScrollInput& input )
{
    m_Cameras[m_CameraMode].MouseScroll( input );
}

void Viewport::Draw()
{
    SCENE_GRAPH_RENDER_SCOPE_TIMER( ("") );

    uint64_t start = Helium::TimerGetClock();

    if (!m_World)
    {
        return;
    }

    Camera& camera = m_Cameras[m_CameraMode];

    GraphicsScene* pGraphicsScene = m_World->GetGraphicsScene();
    GraphicsSceneView* pSceneView = pGraphicsScene->GetSceneView( m_SceneViewId );
    BufferedDrawer* pDrawer = pGraphicsScene->GetSceneViewBufferedDrawer( m_SceneViewId );

    DrawArgs args;

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Setup Viewport and Projection") );

        Vector3 pos;
        camera.GetPosition( pos );

        const Matrix4& invView = camera.GetInverseView();

        pSceneView->SetView(
            Simd::Vector3( &pos.x ),
            -Simd::Vector3( &invView.z.x ),
            Simd::Vector3( &invView.y.x ) );

        pSceneView->SetHorizontalFov( Camera::FieldOfView * static_cast< float32_t >(HELIUM_RAD_TO_DEG) );
    }

    if (m_GridVisible)
    {
        m_GlobalPrimitives[GlobalPrimitives::StandardGrid]->Draw( pDrawer, &args );
    }

#ifdef VIEWPORT_REFACTOR
    // this seems like a bad place to do this
    if (m_Tool)
    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Tool Evaluate") );
        m_Tool->Evaluate();
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Clear and Reset Scene") );

        device->BeginScene();
        device->SetRenderTarget( 0, m_DeviceManager.GetBackBuffer() );
        device->SetDepthStencilSurface( m_DeviceManager.GetDepthBuffer() );
        device->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 80, 80, 80), 1.0f, 0);
        device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);

        m_ResourceTracker->ResetState();
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Setup Viewport and Projection") );

        device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m_Cameras[m_CameraMode].SetProjection(m_Size.x, m_Size.y));
        device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_Cameras[m_CameraMode].GetViewport());
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Setup RenderState (culling, lighting, and fill") );

        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        if (m_Cameras[m_CameraMode].IsBackFaceCulling())
        {
            device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        }
        else
        {
            device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }

        device->SetRenderState(D3DRS_LIGHTING, TRUE);
        device->SetRenderState(D3DRS_COLORVERTEX, FALSE);
        device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
        device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
        device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

        device->SetRenderState(D3DRS_ZENABLE, TRUE);
        device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

        device->SetPixelShader( NULL );
        device->SetVertexShader( NULL );

        D3DLIGHT9 light;    
        ZeroMemory(&light, sizeof(light));

        D3DCOLORVALUE ambient;
        D3DCOLORVALUE diffuse;
        D3DCOLORVALUE specular;
        if ( m_Cameras[m_CameraMode].GetShadingMode() == ShadingMode::Wireframe )
        {
            ambient = SceneGraph::Color::DIMGRAY;
            diffuse = SceneGraph::Color::BLACK;
            specular = SceneGraph::Color::BLACK;
        }
        else
        {
            ambient = SceneGraph::Color::DIMGRAY;
            diffuse = SceneGraph::Color::SILVER;
            specular = SceneGraph::Color::SILVER;
        }

        Vector3 dir;
        m_Cameras[m_CameraMode].GetDirection(dir);

        // setup light
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Ambient = ambient;
        light.Diffuse = diffuse;
        light.Specular = specular;

        // set light into runtime
        light.Direction = *(D3DVECTOR*)&dir;
        device->SetLight(0, &light);
        device->LightEnable(0, true);

        // light from the back
        dir *= -1.0f;

        // set light into runtime
        light.Direction = *(D3DVECTOR*)&dir;
        device->SetLight(1, &light);
        device->LightEnable(1, true);
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("PreRender") );

        device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);

        if (m_GridVisible)
        {
            m_GlobalPrimitives[GlobalPrimitives::StandardGrid]->Draw( &args );
        }
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Render") );

        {
            SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Render Setup") );
            m_RenderVisitor.Reset( &args, this );
        }

        {
            SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Render Walk") );
            m_Render.Raise( &m_RenderVisitor );
        }

        if (m_Tool)
        {
            SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Render Tool") );
            m_Tool->Draw( &args );
        }

        {
            SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Render Draw") );
            m_RenderVisitor.Draw();
        }

        args.m_EntryCount = m_RenderVisitor.GetSize();
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("PostRender") );

        device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);
        device->Clear(NULL, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

        if (m_AxesVisible)
        {
            static_cast<SceneGraph::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->DrawViewport( &args, &m_Cameras[m_CameraMode] );
        }

        if (m_Tool)
        {
            m_Tool->Draw( &args );
        }

        if ( m_Focused )
        {
            unsigned w = 3;
            unsigned x = m_Size.x;
            unsigned y = m_Size.y;

            uint32_t color = D3DCOLOR_ARGB(255, 200, 200, 200);

            std::vector< TransformedColored > vertices;

            //   <--
            //  | \  ^
            //  v  \ |
            //   -->

            // top
            vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)w,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)w,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));

            // bottom
            vertices.push_back(TransformedColored ((float)0,    (float)y-w,   1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)y-w,   1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)y-w,   1.0f,   color));

            // left
            vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)w,    (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)w,    (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)0,    (float)0,     1.0f,   color));

            // right
            vertices.push_back(TransformedColored ((float)x-w,  (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x-w,  (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)y,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x,    (float)0,     1.0f,   color));
            vertices.push_back(TransformedColored ((float)x-w,  (float)0,     1.0f,   color));

            device->SetRenderState(D3DRS_ZENABLE, FALSE);
            device->SetFVF(ElementFormats[ElementTypes::TransformedColored]);
            device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &vertices.front(), sizeof(TransformedColored));
            device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[5]), sizeof(TransformedColored));
            device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[10]), sizeof(TransformedColored));
            device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &(vertices[15]), sizeof(TransformedColored));
            device->SetRenderState(D3DRS_ZENABLE, TRUE);

            m_ResourceTracker->ResetState();
        }

        if (m_DragMode == DragModes::Select)
        {
            m_SelectionFrame->Draw( &args );
        }
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Process Statistics") );

        m_Statistics->m_FrameNumber++;
        m_Statistics->m_FrameCount++;

        m_Statistics->m_RenderTime += Helium::CyclesToMillis( Helium::TimerGetClock() - start );
        m_Statistics->m_RenderWalkTime += args.m_WalkTime;
        m_Statistics->m_RenderSortTime += args.m_SortTime;
        m_Statistics->m_RenderCompareTime += args.m_CompareTime;
        m_Statistics->m_RenderDrawTime += args.m_DrawTime;

        m_Statistics->m_EntryCount += args.m_EntryCount;
        m_Statistics->m_TriangleCount += args.m_TriangleCount;
        m_Statistics->m_LineCount += args.m_LineCount;

        m_Statistics->Update();

        if (m_StatisticsVisible)
        {
            m_Statistics->Draw(&args);
        }
    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("End Scene") );

        device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        device->EndScene();

    }

    {
        SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Display") );

        if ( m_DeviceManager.Display( m_Window ) == D3DERR_DEVICELOST )
        {
            m_DeviceManager.SetDeviceLost();
        }
    }
#endif
}

void Viewport::UndoTransform()
{
    UndoTransform( m_CameraMode );
}

void Viewport::UndoTransform( CameraMode camMode )
{
    m_CameraHistory[camMode].Undo();
}

void Viewport::RedoTransform()
{
    RedoTransform( m_CameraMode );
}

void Viewport::RedoTransform( CameraMode camMode )
{
    m_CameraHistory[camMode].Redo();
}

void Viewport::UpdateCameraHistory()
{  
    // Update the camera history so we can undo/redo previous camera moves. 
    // This is implemented seperately from 'CameraMoved' since 'CameraMoved' reports all incremental spots during a transition.
    // We also need to be able to update this from other events in the scene editor, such as when we focus on an object
    // directly ( shortcut key - f )

    // We only work for the Orbit camera at the moment. SetTransform assumes a Perspective view
    // Not sure how to handle orthographic at the moment.
    if( m_CameraMode != CameraMode::Orbit )
    {
        return;
    }

    m_CameraHistory[m_CameraMode].Push( new CameraMovedCommand( this, &m_Cameras[m_CameraMode] ) );
}

void Viewport::CameraMoved( const SceneGraph::CameraMovedArgs& args )
{
    m_CameraMoved.Raise( args );  
}

void Viewport::OnGridSettingsChanged( const Reflect::ObjectChangeArgs& args )
{
    GridSettings* gridSettings = m_SettingsManager->GetSettings< GridSettings >();
    SceneGraph::PrimitiveGrid* grid = (SceneGraph::PrimitiveGrid*) m_GlobalPrimitives[GlobalPrimitives::StandardGrid];

    grid->m_Width = gridSettings->GetWidth();
    grid->m_Length = gridSettings->GetLength();
    grid->m_MajorStep = gridSettings->GetMajorStep();
    grid->m_MinorStep = gridSettings->GetMinorStep();
    grid->SetAxisColor( gridSettings->GetAxisColor().r, gridSettings->GetAxisColor().g, gridSettings->GetAxisColor().b, 0xFF );
    grid->SetMajorColor( gridSettings->GetMajorColor().r, gridSettings->GetMajorColor().g, gridSettings->GetMajorColor().b, 0xFF );
    grid->SetMinorColor( gridSettings->GetMinorColor().r, gridSettings->GetMinorColor().g, gridSettings->GetMinorColor().b, 0xFF );
    grid->Update();
}
