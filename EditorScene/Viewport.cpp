#include "EditorScenePch.h"
#include "Viewport.h"

#include "Platform/Exception.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "EditorScene/Camera.h"
#include "EditorScene/CameraMovedCommand.h"
#include "EditorScene/Color.h"
#include "EditorScene/DeviceManager.h"
#include "EditorScene/Pick.h"
#include "EditorScene/PrimitiveAxes.h"
#include "EditorScene/PrimitiveFrame.h"
#include "EditorScene/PrimitiveGrid.h"
#include "EditorScene/PrimitiveRings.h"
#include "EditorScene/SceneSettings.h"
#include "EditorScene/Orientation.h"
#include "EditorScene/Tool.h"
#include "EditorScene/GridSettings.h"

#include "Rendering/RRenderContext.h"

using namespace Helium;
using namespace Helium::Editor;

const Helium::Color Viewport::s_LiveMaterial = Editor::Color::MAGENTA;
const Helium::Color Viewport::s_SelectedMaterial = Editor::Color::SPRINGGREEN;
const Helium::Color Viewport::s_ReactiveMaterial = Editor::Color::WHITE;
const Helium::Color Viewport::s_HighlightedMaterial = Editor::Color::CYAN;
const Helium::Color Viewport::s_UnselectableMaterial = Editor::Color::GRAY;
const Helium::Color Viewport::s_ComponentMaterial = Editor::Color::MAGENTA;
const Helium::Color Viewport::s_SelectedComponentMaterial = Editor::Color::YELLOW;
const Helium::Color Viewport::s_RedMaterial = Editor::Color::RED;
const Helium::Color Viewport::s_YellowMaterial = Editor::Color::YELLOW;
const Helium::Color Viewport::s_GreenMaterial = Editor::Color::GREEN;
const Helium::Color Viewport::s_BlueMaterial = Editor::Color::BLUE;

#if HELIUM_OS_WIN
Viewport::Viewport( HWND wnd, SettingsManager* settingsManager)
#else
Viewport::Viewport( void* wnd, SettingsManager* settingsManager)
#endif
	: m_Window( wnd )
	, m_SettingsManager( settingsManager )
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
	, m_SelectionFrame( NULL )
{
	memset(m_GlobalPrimitives, 0x0, sizeof(m_GlobalPrimitives));

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

	static_cast<Editor::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->m_Length = 0.05f;
	static_cast<Editor::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->Update();

	Editor::PrimitiveAxes* transformAxes = static_cast< Editor::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::TransformAxes] );
	transformAxes->m_Length = 0.10f;
	transformAxes->Update();

	Editor::PrimitiveAxes* transformAxesSelected = static_cast< Editor::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] );
	transformAxesSelected->m_Length = 0.10f;
	transformAxesSelected->SetColor( s_SelectedMaterial );
	transformAxesSelected->Update();

	Editor::PrimitiveAxes* jointAxes = static_cast< Editor::PrimitiveAxes* >( m_GlobalPrimitives[GlobalPrimitives::JointAxes] );
	jointAxes->m_Length = 0.015f;
	jointAxes->Update();

	Editor::PrimitiveRings* jointRings = static_cast< Editor::PrimitiveRings* >( m_GlobalPrimitives[GlobalPrimitives::JointRings] );
	jointRings->m_Radius = 0.015f;
	jointRings->m_Steps = 18;
	jointRings->Update();

	m_AxesVisible = true;
	m_GridVisible = true;
	m_BoundsVisible = false;
#endif
}

void Viewport::LoadSettings(Editor::ViewportSettings* prefs)
{
	// apply settings for all modes that we have... 
	for(size_t i = 0; i < prefs->m_CameraPrefs.size(); ++i)
	{
		CameraSettingsPtr cameraPrefs = prefs->m_CameraPrefs[i]; 
		CameraMode mode = cameraPrefs->m_CameraMode; 
		Editor::Camera* camera = GetCameraForMode(mode); 
		camera->LoadSettings(cameraPrefs); 
	}

	SetCameraMode( prefs->m_CameraMode ); 
	SetGeometryMode( prefs->m_GeometryMode ); 
	SetHighlighting( prefs->m_Highlighting ); 
	SetAxesVisible( prefs->m_AxesVisible ); 
	SetGridVisible( prefs->m_GridVisible ); 
	SetBoundsVisible( prefs->m_BoundsVisible ); 
}

void Viewport::SaveSettings(Editor::ViewportSettings* prefs)
{
	// just blow away the previous preferences
	prefs->m_CameraPrefs.clear(); 

	for(int i = 0; i < CameraMode::Count; ++i)
	{
		CameraMode mode = static_cast< CameraMode::Enum >( i ); 
		CameraSettingsPtr cameraPrefs = new CameraSettings(); 
		cameraPrefs->m_CameraMode = mode;
		Editor::Camera* camera = GetCameraForMode( mode ); 
		camera->SaveSettings(cameraPrefs); 
		prefs->m_CameraPrefs.push_back( cameraPrefs ); 
	}

	prefs->m_CameraMode = GetCameraMode(); 
	prefs->m_GeometryMode = GetGeometryMode(); 
	prefs->m_Highlighting = IsHighlighting(); 
	prefs->m_AxesVisible = IsAxesVisible(); 
	prefs->m_GridVisible = IsGridVisible(); 
	prefs->m_BoundsVisible = IsBoundsVisible(); 
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

void Viewport::SetTool(Editor::Tool* tool)
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

Editor::Primitive* Viewport::GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which )
{
	Editor::Primitive* prim = NULL;
	if ( which >= 0 && which < GlobalPrimitives::Count )
	{
		prim = m_GlobalPrimitives[which];
	}
	return prim;
}

void Viewport::InitWidgets()
{
	m_GlobalPrimitives[GlobalPrimitives::ViewportAxes] = new Editor::PrimitiveAxes;
	m_GlobalPrimitives[GlobalPrimitives::ViewportAxes]->Update();

	m_GlobalPrimitives[GlobalPrimitives::StandardAxes] = new Editor::PrimitiveAxes;
	m_GlobalPrimitives[GlobalPrimitives::StandardAxes]->Update();

	m_GlobalPrimitives[GlobalPrimitives::StandardGrid] = new Editor::PrimitiveGrid;

	m_SettingsManager->GetSettings< GridSettings >()->e_Changed.Add( Reflect::ObjectChangeSignature::Delegate( this, &Viewport::OnGridSettingsChanged ));

	OnGridSettingsChanged( Reflect::ObjectChangeArgs( NULL, NULL ) );

	m_GlobalPrimitives[GlobalPrimitives::StandardRings] = new Editor::PrimitiveRings;
	m_GlobalPrimitives[GlobalPrimitives::StandardRings]->Update();

	m_GlobalPrimitives[GlobalPrimitives::TransformAxes] = new Editor::PrimitiveAxes;
	m_GlobalPrimitives[GlobalPrimitives::TransformAxes]->Update();

	m_GlobalPrimitives[GlobalPrimitives::SelectedAxes] = new Editor::PrimitiveAxes;
	m_GlobalPrimitives[GlobalPrimitives::SelectedAxes]->Update();

	m_GlobalPrimitives[GlobalPrimitives::JointAxes] = new Editor::PrimitiveAxes;
	m_GlobalPrimitives[GlobalPrimitives::JointAxes]->Update();

	m_GlobalPrimitives[GlobalPrimitives::JointRings] = new Editor::PrimitiveRings;
	m_GlobalPrimitives[GlobalPrimitives::JointRings]->Update();

	m_SelectionFrame = new Editor::PrimitiveFrame;
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

		GraphicsScene* pGraphicsScene = GetGraphicsScene();
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
	HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("") );

	uint64_t start = Timer::GetTickCount();

	if (!m_World)
	{
		return;
	}

	Camera& camera = m_Cameras[m_CameraMode];

	GraphicsScene* pGraphicsScene = GetGraphicsScene();
	GraphicsSceneView* pSceneView = pGraphicsScene->GetSceneView( m_SceneViewId );
	BufferedDrawer* pDrawer = pGraphicsScene->GetSceneViewBufferedDrawer( m_SceneViewId );

	{
		HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Setup Viewport and Projection") );

		Vector3 pos;
		camera.GetPosition( pos );

		const Matrix4& invView = camera.GetInverseView();

		pSceneView->SetView(
			Simd::Vector3( &pos.x ),
			-Simd::Vector3( &invView.z.x ),
			Simd::Vector3( &invView.y.x ) );

		pSceneView->SetHorizontalFov( Camera::FieldOfView * static_cast< float32_t >(HELIUM_RAD_TO_DEG) );
	}

	pGraphicsScene->SetActiveSceneView( m_SceneViewId );
	pGraphicsScene->Update( m_World.Get() );
	pGraphicsScene->SetActiveSceneView( Invalid< uint32_t >() );

	if (m_GridVisible)
	{
		m_GlobalPrimitives[GlobalPrimitives::StandardGrid]->Draw( pDrawer );
	}

	// this seems like a bad place to do this
	if (m_Tool)
	{
		HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Tool Evaluate") );
		m_Tool->Evaluate();
	}

	{
		HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("PreRender") );

		if (m_GridVisible)
		{
			m_GlobalPrimitives[GlobalPrimitives::StandardGrid]->Draw( pDrawer );
		}
	}

	{
		HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Render") );

		{
			HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Render Setup") );
			m_RenderVisitor.Reset( this, pDrawer );
		}

		{
			HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Render Walk") );
			m_Render.Raise( &m_RenderVisitor );
		}

		if (m_Tool)
		{
			HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("Render Tool") );
			m_Tool->Draw( pDrawer );
		}
	}

	{
		HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER( ("PostRender") );

		//device->Clear(NULL, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

		if (m_AxesVisible)
		{
			static_cast<Editor::PrimitiveAxes*>(m_GlobalPrimitives[GlobalPrimitives::ViewportAxes])->DrawViewport( pDrawer, &m_Cameras[m_CameraMode] );
		}

		if (m_Tool)
		{
			m_Tool->Draw( pDrawer );
		}

#if  VIEWPORT_REFACTOR
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
		}

		if (m_DragMode == DragModes::Select)
		{
			m_SelectionFrame->Draw( &args );
		}
#endif
	}
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

void Viewport::CameraMoved( const Editor::CameraMovedArgs& args )
{
	m_CameraMoved.Raise( args );  
}

void Viewport::OnGridSettingsChanged( const Reflect::ObjectChangeArgs& args )
{
	GridSettings* gridSettings = m_SettingsManager->GetSettings< GridSettings >();
	Editor::PrimitiveGrid* grid = (Editor::PrimitiveGrid*) m_GlobalPrimitives[GlobalPrimitives::StandardGrid];

	grid->m_Width = gridSettings->GetWidth();
	grid->m_Length = gridSettings->GetLength();
	grid->m_MajorStep = gridSettings->GetMajorStep();
	grid->m_MinorStep = gridSettings->GetMinorStep();
	grid->SetAxisColor( gridSettings->GetAxisColor().r, gridSettings->GetAxisColor().g, gridSettings->GetAxisColor().b, 0xFF );
	grid->SetMajorColor( gridSettings->GetMajorColor().r, gridSettings->GetMajorColor().g, gridSettings->GetMajorColor().b, 0xFF );
	grid->SetMinorColor( gridSettings->GetMinorColor().r, gridSettings->GetMinorColor().g, gridSettings->GetMinorColor().b, 0xFF );
	grid->Update();
}

void Viewport::BindToWorld( World* newWorld )
{
	if (newWorld && newWorld != m_World)
	{
		GraphicsScene* pGraphicsScene;

		// Release the old scene view if we have one.
		if ( m_World && IsValid(m_SceneViewId) )
		{
			pGraphicsScene = GetGraphicsScene();
			pGraphicsScene->ReleaseSceneView( m_SceneViewId );
		}

		// Set up the new scene view.
		m_World = newWorld;
		pGraphicsScene = GetGraphicsScene();
		m_SceneViewId = pGraphicsScene->AllocateSceneView();

		OnResize();
	}
}

void Viewport::UnbindFromWorld()
{
	// Release the old scene view if we have one.
	if ( m_World && IsValid(m_SceneViewId) )
	{
		GraphicsScene* pGraphicsScene = GetGraphicsScene();
		pGraphicsScene->ReleaseSceneView( m_SceneViewId );
	}

	m_World.Release();
	m_SceneViewId = Invalid<uint32_t>();
}

GraphicsScene * Viewport::GetGraphicsScene()
{
#pragma TODO("Have a general way for telling the rendering system that we have a viewport rather than assuming the use of GraphicsManagerComponent")
	HELIUM_ASSERT( m_World );
	return m_World->GetComponents().GetFirst<GraphicsManagerComponent>()->GetGraphicsScene();
}
