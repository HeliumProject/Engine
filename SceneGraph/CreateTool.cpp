#include "SceneGraphPch.h"
#include "CreateTool.h"

#include "Math/AngleAxis.h"

#include "SceneGraph/Mesh.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Transform.h"
#include "SceneGraph/Pick.h"
#include "SceneGraph/Orientation.h"
#include "SceneGraph/PrimitiveCircle.h"

HELIUM_DEFINE_ABSTRACT( Helium::SceneGraph::CreateTool );

using namespace Helium;
using namespace Helium::SceneGraph;

IntersectionPlane CreateTool::s_PlaneSnap = IntersectionPlanes::Ground;

bool CreateTool::s_LiveObjectsOnly = false;
bool CreateTool::s_SurfaceSnap = false;
bool CreateTool::s_ObjectSnap = false;
bool CreateTool::s_NormalSnap = false;

bool CreateTool::s_RandomizeAzimuth = false;
float CreateTool::s_AzimuthMin = 0.f;
float CreateTool::s_AzimuthMax = 15.f;

bool CreateTool::s_RandomizeDirection = false;
float CreateTool::s_DirectionMin = 0.f;
float CreateTool::s_DirectionMax = 15.f;

bool CreateTool::s_RandomizeScale = false;
float CreateTool::s_ScaleMin = 0.8f;
float CreateTool::s_ScaleMax = 1.2f;

bool CreateTool::s_PaintMode = false;
bool CreateTool::s_PaintPreventAnyOverlap = false;
PlacementStyle CreateTool::s_PaintPlacementStyle = PlacementStyles::Radial;
DistributionStyle CreateTool::s_PaintDistributionStyle = DistributionStyles::Normal;
float CreateTool::s_PaintRadius = 10.0f;
int CreateTool::s_PaintSpeed = 10;
float CreateTool::s_PaintDensity = 1.0f;
float CreateTool::s_PaintJitter = 1.0f;

CreateTool::CreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator)
: Tool (scene, generator)
, m_Created (false)
, m_InstanceUpdateOffsets (false)
, m_Instance (NULL)
, m_AzimuthMin (NULL)
, m_AzimuthMax (NULL)
, m_DirectionMin (NULL)
, m_DirectionMax (NULL)
, m_ScaleMin (NULL)
, m_ScaleMax (NULL)
, m_PaintPreventAnyOverlap (NULL)
, m_PaintPlacementStyle (NULL)
, m_PaintDistributionStyle (NULL)
, m_PaintRadius (NULL)
, m_PaintSpeed (NULL)
, m_PaintDensity (NULL)
, m_PaintJitter (NULL)
, m_PaintTimer( TXT( "CreateToolPaintTimer" ), 1000 / s_PaintSpeed )
{
    m_Scene->e_NodeAdded.Add( NodeChangeSignature::Delegate ( this, &CreateTool::SceneNodeAdded ) );
    m_Scene->e_NodeRemoved.Add( NodeChangeSignature::Delegate ( this, &CreateTool::SceneNodeRemoved ) );

    m_PaintTimer.AddTickListener( TimerTickSignature::Delegate( this, &CreateTool::TimerCallback ) );
}

CreateTool::~CreateTool()
{
    m_PaintTimer.RemoveTickListener( TimerTickSignature::Delegate( this, &CreateTool::TimerCallback ) );

    m_Scene->e_NodeAdded.Remove( NodeChangeSignature::Delegate ( this, &CreateTool::SceneNodeAdded ) );
    m_Scene->e_NodeRemoved.Remove( NodeChangeSignature::Delegate ( this, &CreateTool::SceneNodeRemoved ) );

    if (m_Instance.ReferencesObject())
    {
        // remove temp reference
        m_Scene->RemoveObject( m_Instance.Ptr() );
    }

    m_Scene->Push( m_Scene->GetSelection().SetItems( m_Selection ) );
}

void CreateTool::Place(const Matrix4& position)
{
    if (m_Instance.ReferencesObject())
    {
        // remove temp reference
        m_Scene->RemoveObject( m_Instance.Ptr() );
    }

    //
    // Create Instance Object
    //

    m_Instance = CreateNode();

    if (m_Instance.ReferencesObject())
    {
        m_Instance->SetSelected( true );

        m_Instance->SetTransient( true );

        m_Instance->SetObjectTransform(position);

        m_Scene->AddObject( m_Instance.Ptr() );

        m_Instance->Evaluate( GraphDirections::Downstream );
    }
}

void CreateTool::DetermineTranslationAndNormal( int x, int y, Vector3& t, Vector3& n )
{
    FrustumLinePickVisitor pick( m_Scene->GetViewport()->GetCamera(), x, y );
    if ( !DetermineTranslationAndNormal( pick, t, n ) )
    {
        // place the object on the selected plane
        m_Scene->GetViewport()->GetCamera()->ViewportToPlaneVertex( (float32_t)x, (float32_t)y, s_PlaneSnap, t );
    }
}

bool CreateTool::DetermineTranslationAndNormal( PickVisitor& pick, Vector3& t, Vector3& n )
{
    // pick in the world
    PickArgs args ( &pick );
    m_PickWorld.Raise( args );

    bool set = false;
    if ( s_SurfaceSnap || s_ObjectSnap )
    {
        // process results
        V_PickHitSmartPtr sorted;
        PickHit::Sort( m_Scene->GetViewport()->GetCamera(), pick.GetHits(), sorted, PickSortTypes::Intersection );

        V_PickHitSmartPtr::const_iterator itr = sorted.begin();
        V_PickHitSmartPtr::const_iterator end = sorted.end();
        for ( ; itr != end; ++itr )
        {
            SceneGraph::HierarchyNode* node = Reflect::SafeCast<SceneGraph::HierarchyNode>( (*itr)->GetHitObject() );

            // don't snap against what we are placing
            if ( node == m_Instance )
            {
                continue;
            }

            // eliminate nodes that are not live when we are live snapping
            if ( s_LiveObjectsOnly && !node->IsLive() )
            {
                continue;
            }

            // don't snap if we are surface snapping with no normal
            if ( s_SurfaceSnap && !(*itr)->HasNormal() )
            {
                continue;
            }

            // don't snap if we don't have an intersection
            if ( !(*itr)->HasIntersection() )
            {
                continue;
            }

            if ( s_ObjectSnap )
            {
                Vector4 v = node->GetTransform()->GetGlobalTransform().t;
                t.x = v.x;
                t.y = v.y;
                t.z = v.z;
                set = true;
            }
            else
            {
                t = (*itr)->GetIntersection();
            }

            if ( s_NormalSnap && (*itr)->HasNormal() )
            {
                n = (*itr)->GetNormal();
            }

            set = true;
            break;
        }
    }

    return set;
}

void CreateTool::GenerateInstanceOffsets( PlacementStyle style, float radius, float instanceRadius, V_Vector3& positions )
{
    switch ( style )
    {
    case PlacementStyles::Grid:
        {
            float radiusSquared = radius * radius;
            int numInstances = MAX( 2, (int) sqrt( radiusSquared / ( instanceRadius * instanceRadius ) ) );
            float delta = radius * 2.0f / numInstances;

            for ( float x = -radius; x <= radius; x += delta )
            {
                for ( float y = -radius; y <= radius; y += delta )
                {
                    if ( x * x + y * y < radiusSquared )
                    {
                        Vector3 v = ( SceneGraph::UpVector * x ) + ( SceneGraph::OutVector * y );
                        positions.push_back( v );
                    }
                }
            }
            break;
        }

    case PlacementStyles::Radial:
    default:
        {
            float currentRadius = 0.0f;
            while ( currentRadius < radius )
            {
                float circumference = static_cast< float32_t >( HELIUM_TWOPI ) * currentRadius;
                int numInstances = MAX( 1, (int) ( circumference / ( 2.0f * instanceRadius ) ) );

                float deltaAngle = static_cast< float32_t >( HELIUM_TWOPI ) / numInstances;
                float currentAngle = static_cast< float32_t >( HELIUM_TWOPI ) * rand() / ( (float) RAND_MAX + 1.0f );

                for ( int i = 0; i < numInstances; ++i )
                {
                    float x = currentRadius * cos( currentAngle );
                    float y = currentRadius * sin( currentAngle );
                    Vector3 v = ( SceneGraph::UpVector * x ) + ( SceneGraph::OutVector * y );
                    positions.push_back( v );

                    currentAngle += deltaAngle;
                    while ( currentAngle > HELIUM_TWOPI )
                    {
                        currentAngle -= static_cast< float32_t >( HELIUM_TWOPI );
                    }
                }

                currentRadius += instanceRadius + instanceRadius;
            }
            break;
        }
    }
}

void CreateTool::SelectInstanceOffsets( DistributionStyle style, float radius, V_Vector3& offsets )
{
    V_Vector3 selectedOffsets;
    selectedOffsets.reserve( offsets.size() );

    V_Vector3::iterator itr = offsets.begin();
    V_Vector3::iterator end = offsets.end();
    for ( ; itr != end; ++itr )
    {
        switch ( style )
        {
        case DistributionStyles::Uniform:
            {
                float randomNumber = rand() / ( (float) RAND_MAX + 1.0f );
                if ( randomNumber <= 0.5f )
                {
                    selectedOffsets.push_back( *itr );
                }
                break;
            }

        case DistributionStyles::Linear:
            {
                float radiusPercent = (*itr).Length() / radius;
                float randomNumber = rand() / ( (float) RAND_MAX + 1.0f );
                float testNumber = 1.0f - radiusPercent;
                if ( randomNumber <= testNumber )
                {
                    selectedOffsets.push_back( *itr );
                }
                break;
            }

        case DistributionStyles::Normal:
            {
                float radiusPercent = (*itr).Length() / radius;
                float randomNumber = rand() / ( (float) RAND_MAX + 1.0f );
                float testNumber = GetNormalProbabilityFromPercent( radiusPercent );
                if ( randomNumber <= testNumber )
                {
                    selectedOffsets.push_back( *itr );
                }
                break;
            }

        case DistributionStyles::Constant:
        default:
            selectedOffsets.push_back( *itr );
            break;
        }
    }

    offsets.clear();
    offsets.reserve( selectedOffsets.size() );
    itr = selectedOffsets.begin();
    end = selectedOffsets.end();
    for ( ; itr != end; ++itr )
    {
        offsets.push_back( *itr );
    }
}

void CreateTool::JitterInstanceOffsets( float instanceRadius, float maxJitter, V_Vector3& offsets )
{
    V_Vector3 jitterVectors;
    jitterVectors.push_back( SceneGraph::UpVector );
    jitterVectors.push_back( SceneGraph::OutVector );

    V_Vector3::iterator itr = offsets.begin();
    V_Vector3::iterator end = offsets.end();
    for ( ; itr != end; ++itr )
    {
        V_Vector3::const_iterator jitterItr = jitterVectors.begin();
        V_Vector3::const_iterator jitterEnd = jitterVectors.end();
        for ( ; jitterItr != jitterEnd; ++jitterItr )
        {
            int searchTries = 10;
            while ( searchTries > 0 )
            {
                --searchTries;
                float jitter = ( rand() / ( (float) RAND_MAX + 1.0f ) ) * 2.0f - 1.0f;
                float randomNumber = rand() / ( (float) RAND_MAX + 1.0f );
                float testNumber = GetNormalProbabilityFromPercent( jitter );
                if ( randomNumber <= testNumber )
                {
                    (*itr) += (*jitterItr) * jitter * maxJitter;
                    searchTries = 0;
                }
            }
        }
    }
}

void CreateTool::RandomizeInstanceOffsets( V_Vector3& offsets )
{
    V_Vector3 newOffsets;
    newOffsets.reserve( offsets.size() );

    while ( offsets.size() )
    {
        V_Vector3::iterator itr = offsets.begin() + ( rand() % offsets.size() );
        newOffsets.push_back( *itr );
        offsets.erase( itr );
    }

    V_Vector3::iterator itr = newOffsets.begin();
    V_Vector3::iterator end = newOffsets.end();
    for ( ; itr != end; ++itr )
    {
        offsets.push_back( *itr );
    }
}

void CreateTool::FinalizeOrientation(Matrix4& position, const Vector3& t, const Vector3& n)
{
    // randomize the scale
    if ( s_RandomizeScale )
    {
        // random float b/t 0.0f and 1.0f
        float32_t scale = rand()/(float(RAND_MAX)+1);

        // multiply by the range and add the base to set the bounds
        scale = (scale * (s_ScaleMax - s_ScaleMin)) + s_ScaleMin;

        // scale
        position *= Matrix4 ( Scale ( scale, scale, scale ) );
    }

    // seed the normal
    if ( n != Vector3::Zero )
    {
        // rotate by the normal (we are currently orthogonal)
        position *= Matrix4 ( AngleAxis::Rotation( SceneGraph::UpVector, n ) );
    }

    // randomize the rotation
    if ( s_RandomizeDirection )
    {
        // random float b/t -1.0f and 1.0f
        float32_t angle = rand()/(float(RAND_MAX)+1) * 2.0f - 1.0f;

        // multiply by the range
        angle = (angle * (s_DirectionMax - s_DirectionMin));

        // add/subract the base depending on sign
        if (angle < 0.f)
        {
            angle -= s_DirectionMin;
        }
        else
        {
            angle += s_DirectionMin;
        }

        // convert to radians
        angle *= static_cast< float32_t >( HELIUM_DEG_TO_RAD );

        // pick a random vector
        Vector3 axis (rand()/(float(RAND_MAX)+1) - 0.5f, rand()/(float(RAND_MAX)+1) - 0.5f, rand()/(float(RAND_MAX)+1) - 0.5f);

        // make sure its 1.0f in length
        axis.Normalize();

        // rotate
        position *= Matrix4 ( AngleAxis ( angle, axis ) );
    }

    // randomize the rotation
    if ( s_RandomizeAzimuth )
    {
        // random float b/t -1.0f and 1.0f
        float32_t angle = rand()/(float(RAND_MAX)+1) * 2.0f - 1.0f;

        // multiply by the range
        angle = (angle * (s_AzimuthMax - s_AzimuthMin));

        // add/subract the base depending on sign
        if (angle < 0.f)
        {
            angle -= s_AzimuthMin;
        }
        else
        {
            angle += s_AzimuthMin;
        }

        // convert to radians
        angle *= static_cast< float32_t >( HELIUM_DEG_TO_RAD );

        // rotate
        position = Matrix4 ( AngleAxis ( angle, UpVector ) ) * position;
    }

    // set the translation
    position.t = t;
}

bool CreateTool::ValidPosition( const AlignedBox& bounds, const Vector3& translation, float minDistance )
{
    SceneGraph::HierarchyNode* node = Reflect::SafeCast<SceneGraph::HierarchyNode>( m_Instance );

    FrustumPickVisitor frustumPick( m_Scene->GetViewport()->GetCamera(), Frustum( bounds ) );
    m_Scene->Pick( &frustumPick );

    V_PickHitSmartPtr::const_iterator resultsItr = frustumPick.GetHits().begin();
    V_PickHitSmartPtr::const_iterator resultsEnd = frustumPick.GetHits().end();
    for ( ; resultsItr != resultsEnd; ++resultsItr )
    {
        SceneGraph::HierarchyNode* currentNode = Reflect::SafeCast<SceneGraph::HierarchyNode>( (*resultsItr)->GetHitObject() );
        if ( !currentNode->IsTransient() && ( s_PaintPreventAnyOverlap || node->IsSimilar( currentNode ) ) )
        {
            const SceneGraph::Transform* transform = currentNode->GetTransform();
            if ( !transform )
            {
                return false;
            }

            Vector3 position( transform->GetGlobalTransform().t.x, transform->GetGlobalTransform().t.y, transform->GetGlobalTransform().t.z );
            Vector3 differenceVector = translation - position;
            if ( differenceVector.Length() < minDistance )
            {
                return false;
            }
        }
    }

    return true;
}

void CreateTool::CalculateInstanceRadiusAndBounds( float32_t& instanceRadius, AlignedBox& bounds )
{
    SceneGraph::HierarchyNode* node = Reflect::SafeCast<SceneGraph::HierarchyNode>( m_Instance );
    bounds = node->GetObjectBounds();

    Vector3 boundVector = bounds.maximum - bounds.minimum;
    Vector3 out = boundVector * SceneGraph::OutVector;
    Vector3 side = boundVector * SceneGraph::SideVector;
    instanceRadius = MAX( out.Length(), side.Length() ) / 2.0f;
}

void CreateTool::RefreshInstance( void )
{
    Matrix4 orientation;
    FinalizeOrientation( orientation, m_InstanceTranslation, m_InstanceNormal );
    Place( orientation );
}

float CreateTool::GetNormalProbabilityFromPercent( float value )
{
    static float scale = 2.2f;
    float probability = exp( ( scale * scale * value * value ) / -2.0f );
    return probability;
}

void CreateTool::AddToScene()
{
    if (!m_Instance.ReferencesObject())
    {
        return;
    }

    if (!m_Scene->IsEditable())
    {
        return;
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Remove Transient Instance") );

        m_Scene->RemoveObject( m_Instance.Ptr() );
    }

    BatchUndoCommandPtr batch = new BatchUndoCommand ();

    if (!m_Created)
    {
        batch->Push( m_Scene->GetSelection().Clear() );
        m_Created = true;
    }

    m_Instance->SetTransient( false );

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Push Undo Command Adding Instance Into Batch") );

        batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Add, m_Scene, m_Instance ) );
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Initialize Instance") );

        if ( !m_Instance->IsInitialized() )
        {
            m_Instance->SetOwner( m_Scene );
            m_Instance->Initialize();
        }

        HELIUM_ASSERT( m_Instance->GetOwner() == m_Scene );
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Append To Selection") );

        m_Selection.Append( m_Instance );
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Push Undo Batch Into Scene") );

        m_Scene->Push( batch );
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ("Place New Instance At Origin") );

        m_Instance = NULL;
        Place( Matrix4::Identity );
    }
}

void CreateTool::SceneNodeAdded( const NodeChangeArgs& args )
{
    m_Selection.Append( args.m_Node );
}

void CreateTool::SceneNodeRemoved( const NodeChangeArgs& args )
{
    m_Selection.Remove( args.m_Node );
}

void CreateTool::Draw( DrawArgs* args )
{
    if ( !s_PaintMode )
    {
        return;
    }

    Matrix4 ringTransform;

    if ( m_InstanceNormal != Vector3::Zero )
    {
        ringTransform *= Matrix4( AngleAxis::Rotation( SceneGraph::SideVector, m_InstanceNormal ) );
    }
    else
    {
        ringTransform *= Matrix4( AngleAxis::Rotation( SceneGraph::SideVector, SceneGraph::UpVector ) );
    }

    ringTransform.t.x = m_InstanceTranslation.x;
    ringTransform.t.y = m_InstanceTranslation.y;
    ringTransform.t.z = m_InstanceTranslation.z;

#ifdef VIEWPORT_REFACTOR
    m_View->GetDevice()->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(&ringTransform));

    SceneGraph::PrimitiveCircle ring ();
    ring.m_RadiusSteps = 360;
    ring.m_Radius = s_PaintRadius;
    ring.Update();
    ring.Draw(args);
#endif
}

bool CreateTool::AllowSelection()
{
    return false;
}

bool CreateTool::MouseDown( const MouseButtonInput& e )
{
    if ( e.MiddleDown() )
    {
        Vector3 translation;
        Vector3 normal;

        {
            SCENE_GRAPH_SCOPE_TIMER( ( "Pick Location For Instance" ) );
            DetermineTranslationAndNormal( e.GetPosition().x, e.GetPosition().y, translation, normal );
        }

        CreateSingleObject( translation, normal );

        if ( s_PaintMode )
        {
            m_InstanceUpdateOffsets = true;
            CalculateInstanceRadiusAndBounds( m_InstanceRadius, m_InstanceBounds );
            if ( e.ShiftIsDown() )
            {
                CreateMultipleObjects( true );
            }
            if ( !m_PaintTimer.IsAlive() )
            {
                m_PaintTimer.Start();
            }
        }

        {
            SCENE_GRAPH_SCOPE_TIMER( ( "Execute Scene" ) );

            m_Scene->Execute(true);
        }
    }

    return Base::MouseDown( e );
}

void CreateTool::MouseMove( const MouseMoveInput& e )
{
    if ( !m_Instance.ReferencesObject() )
    {
        return;
    }

    if ( m_PaintTimer.IsAlive() )
    {
        m_InstanceUpdateOffsets = true;
    }

    // get position
    Vector3 translation;
    Vector3 normal;
    DetermineTranslationAndNormal( e.GetPosition().x, e.GetPosition().y, translation, normal );

    Matrix4 position;
    FinalizeOrientation( position, translation, normal );

    m_InstanceTranslation = translation;
    m_InstanceNormal = ( normal == Vector3::Zero ) ? SceneGraph::UpVector : normal;

    // hide the temporary object when painting and moving
    if ( m_PaintTimer.IsAlive() )
    {
        position *= Matrix4( Scale( 0.0f, 0.0f, 0.0f ) );
    }

    // set position
    m_Instance->SetObjectTransform( position );

    // evaluate
    m_Instance->Evaluate( GraphDirections::Downstream );

    // render
    m_Scene->Execute(true);

    Base::MouseMove(e);
}

void CreateTool::MouseUp( const MouseButtonInput& e )
{
    if ( !e.MiddleDown() && m_PaintTimer.IsAlive() )
    {
        m_PaintTimer.Stop();
    }

    Base::MouseUp(e);
}

void CreateTool::KeyPress( const KeyboardInput& e )
{
    if (e.GetKeyCode() == KeyCodes::Return)
    {
        AddToScene();

        // create new at position
        Place(Matrix4::Identity);

        // render
        m_Scene->Execute(true);
    }
    else
    {
        e.Skip();
    }
}

void CreateTool::CreateProperties()
{
    Base::CreateProperties();

    Place(Matrix4::Identity);

    Inspect::CheckBox* checkBox;

    m_Generator->PushContainer( TXT( "Common" ) );
    {
        m_Generator->PushContainer( TXT( "Snapping" ) )->SetUIHints( Inspect::UIHint::Popup );
        {
            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Controls the snapping for placed objects.\n\nViewport - Snap to the camera's point of interest\n\nGround - Snap to the X-Z plane" );
                m_Generator->AddLabel( TXT( "Plane" ) )->a_HelpText.Set( helpText );
                Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::CreateTool, int> (this, &CreateTool::GetPlaneSnap, &CreateTool::SetPlaneSnap) );
                choice->a_IsDropDown.Set( true );
                choice->a_HelpText.Set( helpText );

                std::vector< Inspect::ChoiceItem > items;

                {
                    std::ostringstream str;
                    str << IntersectionPlanes::Ground;
                    items.push_back( Inspect::ChoiceItem( TXT( "Ground" ), str.str() ) );
                }

                {
                    std::ostringstream str;
                    str << IntersectionPlanes::Viewport;
                    items.push_back( Inspect::ChoiceItem( TXT( "Viewport" ), str.str() ) );
                }

                choice->a_Items.Set( items );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "If set, objects will only snap to other objects which have been marked with the 'Live' flag." );
                m_Generator->AddLabel( TXT( "Live Objects" ) )->a_HelpText.Set( helpText );
                checkBox = m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetLiveObjectsOnly, &CreateTool::SetLiveObjectsOnly) );
                checkBox->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Toggles surface snapping for placed objects." );
                m_Generator->AddLabel( TXT( "Surfaces" ) )->a_HelpText.Set( helpText );
                checkBox = m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetSurfaceSnap, &CreateTool::SetSurfaceSnap) );
                checkBox->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "If enabled, created objects will snap to already existing objects." );
                m_Generator->AddLabel( TXT( "Objects" ) )->a_HelpText.Set( helpText );
                checkBox = m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetObjectSnap, &CreateTool::SetObjectSnap) );
                checkBox->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Snaps objects to the face normal of the surface they are placed upon." );
                m_Generator->AddLabel( TXT( "Normals" ) )->a_HelpText.Set( helpText );
                checkBox = m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetNormalSnap, &CreateTool::SetNormalSnap) );
                checkBox->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();
        }
        m_Generator->Pop();

        m_Generator->PushContainer( TXT( "Randomization" ) )->SetUIHints( Inspect::UIHint::Popup );
        {
            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "If set, this will apply a random offset to the created object's azimuth" );
                m_Generator->AddLabel( TXT( "Azimuth" ) )->a_HelpText.Set( helpText );
                checkBox = m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetRandomizeAzimuth, &CreateTool::SetRandomizeAzimuth) );
                checkBox->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_AzimuthMin = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the lower bound for azimuth variation." );
                m_Generator->AddLabel( TXT( "Lower Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetAzimuthMin, &CreateTool::SetAzimuthMin) );
                slider->a_Min.Set( 0.f );
                slider->a_Max.Set( 180.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_AzimuthMax = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the upper bound for azimuth variation." );
                m_Generator->AddLabel( TXT( "Upper Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetAzimuthMax, &CreateTool::SetAzimuthMax) );
                slider->a_Min.Set( 0.f );
                slider->a_Max.Set( 180.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "If set, the direction the created objects are facing will be randomized." );
                m_Generator->AddLabel( TXT( "Direction" ) )->a_HelpText.Set( helpText );
                m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetRandomizeDirection, &CreateTool::SetRandomizeDirection) )->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_DirectionMin = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the lower bound for direction variation." );
                m_Generator->AddLabel( TXT( "Lower Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetDirectionMin, &CreateTool::SetDirectionMin) );
                slider->a_Min.Set( 0.f );
                slider->a_Max.Set( 180.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_DirectionMax = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the upper bound for direction variation." );
                m_Generator->AddLabel( TXT( "Upper Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetDirectionMax, &CreateTool::SetDirectionMax) );
                slider->a_Min.Set( 0.f );
                slider->a_Max.Set( 180.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "When enabled, this will cause the scale of the created objects to be randomized." );
                m_Generator->AddLabel( TXT( "Scale" ) )->a_HelpText.Set( helpText );
                m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetRandomizeScale, &CreateTool::SetRandomizeScale) )->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_ScaleMin = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the lower bound for random scaling of created objects." );
                m_Generator->AddLabel( TXT( "Lower Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetScaleMin, &CreateTool::SetScaleMin) );
                slider->a_Min.Set( 0.05f );
                slider->a_Max.Set( 5.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_ScaleMax = m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "Sets the upper bound for random scaling of created objects." );
                m_Generator->AddLabel( TXT( "Upper Bound" ) )->a_HelpText.Set( helpText );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetScaleMax, &CreateTool::SetScaleMax) );
                slider->a_Min.Set( 0.05f );
                slider->a_Max.Set( 5.f );
                slider->a_HelpText.Set( helpText );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                textBox->a_HelpText.Set( helpText );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();
        }
        m_Generator->Pop();

        m_Generator->PushContainer( TXT( "Painting" ) )->SetUIHints( Inspect::UIHint::Popup );
        {
            m_Generator->PushContainer();
            {
                const std::string helpText = TXT( "If enabled, object instances will be 'painted' down, following some rules.  So, for instance, if you wished to add a number of shrubs to a scene, you could turn on painting (and some other options) and click and drag to 'paint' the instances into the scene." );
                m_Generator->AddLabel( TXT( "Enable" ) )->a_HelpText.Set( helpText );
                m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetPaintMode, &CreateTool::SetPaintMode) )->a_HelpText.Set( helpText );
            }
            m_Generator->Pop();

            m_PaintPreventAnyOverlap = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Prevent Overlap" ) );
                m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::CreateTool, bool> (this, &CreateTool::GetPaintPreventAnyOverlap, &CreateTool::SetPaintPreventAnyOverlap) );
            }
            m_Generator->Pop();

            m_PaintPlacementStyle = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Placement" ) );

                Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::CreateTool, int> (this, &CreateTool::GetPaintPlacementStyle, &CreateTool::SetPaintPlacementStyle ) );
                choice->a_IsDropDown.Set( true );
                std::vector< Inspect::ChoiceItem > items;

                {
                    std::ostringstream str;
                    str << PlacementStyles::Grid;
                    items.push_back( Inspect::ChoiceItem( TXT( "Grid" ), str.str() ) );
                }

                {
                    std::ostringstream str;
                    str << PlacementStyles::Radial;
                    items.push_back( Inspect::ChoiceItem( TXT( "Radial" ), str.str() ) );
                }

                choice->a_Items.Set( items );
            }
            m_Generator->Pop();

            m_PaintDistributionStyle = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Distribution" ) );

                Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::CreateTool, int> (this, &CreateTool::GetPaintDistributionStyle, &CreateTool::SetPaintDistributionStyle ) );
                choice->a_IsDropDown.Set( true );
                std::vector< Inspect::ChoiceItem > items;

                {
                    std::ostringstream str;
                    str << DistributionStyles::Constant;
                    items.push_back( Inspect::ChoiceItem( TXT( "Constant" ), str.str() ) );
                }

                {
                    std::ostringstream str;
                    str << DistributionStyles::Uniform;
                    items.push_back( Inspect::ChoiceItem( TXT( "Uniform" ), str.str() ) );
                }

                {
                    std::ostringstream str;
                    str << DistributionStyles::Linear;
                    items.push_back( Inspect::ChoiceItem( TXT( "Linear" ), str.str() ) );
                }

                {
                    std::ostringstream str;
                    str << DistributionStyles::Normal;
                    items.push_back( Inspect::ChoiceItem( TXT( "Normal" ), str.str() ) );
                }

                choice->a_Items.Set( items );
            }
            m_Generator->Pop();

            m_PaintRadius = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Radius" ) );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetPaintRadius, &CreateTool::SetPaintRadius) );
                slider->a_Min.Set( 0.1f );
                slider->a_Max.Set( 30.0f );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_PaintSpeed = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Speed" ) );
                Inspect::Slider* slider = m_Generator->AddSlider<int>( new Helium::MemberProperty<SceneGraph::CreateTool, int> (this, &CreateTool::GetPaintSpeed, &CreateTool::SetPaintSpeed) );
                slider->a_Min.Set( 1 );
                slider->a_Max.Set( 10 );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_PaintDensity = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Density" ) );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetPaintDensity, &CreateTool::SetPaintDensity) );
                slider->a_Min.Set( 0.0f );
                slider->a_Max.Set( 2.0f );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();

            m_PaintJitter = m_Generator->PushContainer();
            {
                m_Generator->AddLabel( TXT( "Jitter" ) );
                Inspect::Slider* slider = m_Generator->AddSlider<float>( new Helium::MemberProperty<SceneGraph::CreateTool, float> (this, &CreateTool::GetPaintJitter, &CreateTool::SetPaintJitter) );
                slider->a_Min.Set( 0.0f );
                slider->a_Max.Set( 1.0f );

                Inspect::ValuePtr textBox = m_Generator->CreateControl<Inspect::Value>();
                textBox->Bind( slider->GetBinding() );
                m_Generator->Add( textBox );
            }
            m_Generator->Pop();
        }
        m_Generator->Pop();
    }
    m_Generator->Pop();

    m_AzimuthMin->a_IsEnabled.Set(s_RandomizeAzimuth);
    m_AzimuthMax->a_IsEnabled.Set(s_RandomizeAzimuth);

    m_DirectionMin->a_IsEnabled.Set(s_RandomizeDirection);
    m_DirectionMax->a_IsEnabled.Set(s_RandomizeDirection);

    m_ScaleMin->a_IsEnabled.Set(s_RandomizeScale);
    m_ScaleMax->a_IsEnabled.Set(s_RandomizeScale);

    m_PaintPreventAnyOverlap->a_IsEnabled.Set(s_PaintMode);
    m_PaintPlacementStyle->a_IsEnabled.Set(s_PaintMode);
    m_PaintDistributionStyle->a_IsEnabled.Set(s_PaintMode);
    m_PaintRadius->a_IsEnabled.Set(s_PaintMode);
    m_PaintSpeed->a_IsEnabled.Set(s_PaintMode);
    m_PaintDensity->a_IsEnabled.Set(s_PaintMode);
    m_PaintJitter->a_IsEnabled.Set(s_PaintMode);
}

void CreateTool::SetupInstanceOffsets( float instanceRadius, V_Vector3& instanceOffsets )
{
    instanceOffsets.clear();
    instanceOffsets.reserve( 256 );

    float adjustedInstanceRadius = instanceRadius / s_PaintDensity;

    GenerateInstanceOffsets( s_PaintPlacementStyle, s_PaintRadius, adjustedInstanceRadius, instanceOffsets );
    SelectInstanceOffsets( s_PaintDistributionStyle, s_PaintRadius, instanceOffsets );
    JitterInstanceOffsets( instanceRadius, s_PaintJitter, instanceOffsets );
    RandomizeInstanceOffsets( instanceOffsets );
}

void CreateTool::CreateSingleObject( const Vector3& translation, const Vector3& normal, bool checkValid )
{
    Matrix4 orientation;

    {
        SCENE_GRAPH_SCOPE_TIMER( ( "Finalize Instance Orientation" ) );
        FinalizeOrientation( orientation, translation, normal );
    }

    if ( checkValid )
    {
        float adjustedInstanceRadius = m_InstanceRadius / s_PaintDensity;
        float minDistance = 2.0f * adjustedInstanceRadius;
        float scale = minDistance / m_InstanceRadius;
        Matrix4 scaledOrientation( Scale( minDistance, minDistance, minDistance ) );
        scaledOrientation *= orientation;

        AlignedBox instanceBounds = m_InstanceBounds;
        instanceBounds.Transform( scaledOrientation );

        if ( !ValidPosition( instanceBounds, translation, minDistance ) )
        {
            return;
        }
    }

    if ( m_Instance.ReferencesObject() )
    {
        SCENE_GRAPH_SCOPE_TIMER( ( "Update Temporary Instance At Location" ) );
        m_Instance->SetObjectTransform( orientation );
        m_Instance->Evaluate( GraphDirections::Downstream );
    }
    else
    {
        SCENE_GRAPH_SCOPE_TIMER( ( "Place Temporary Instance At Location" ) );
        Place( orientation );
    }

    {
        SCENE_GRAPH_SCOPE_TIMER( ( "Add Instance To Scene" ) );
        AddToScene();
    }

    if ( m_Instance.ReferencesObject() )
    {
        SCENE_GRAPH_SCOPE_TIMER( ( "Update Temporary Instance At Location" ) );
        if ( m_PaintTimer.IsAlive() )
        {
            orientation *= Matrix4( Scale( 0.0f, 0.0f, 0.0f ) );
        }

        m_Instance->SetObjectTransform( orientation );
        m_Instance->Evaluate( GraphDirections::Downstream );
    }
}

void CreateTool::CreateMultipleObjects( bool stamp )
{
    SCENE_GRAPH_SCOPE_TIMER( ("Place Multiple Instances At Location") );

    if ( m_InstanceRadius <= 0.0f )
    {
        return;
    }

    if ( m_InstanceOffsets.empty() || m_InstanceUpdateOffsets )
    {
        m_InstanceOffsets.clear();
        SetupInstanceOffsets( m_InstanceRadius, m_InstanceOffsets );
        m_InstanceUpdateOffsets = false;
    }

    float32_t maxTime = 100.0f;
    SimpleTimer instanceTimer;
    Vector3 instanceNormalOffset = m_InstanceNormal.Normalize() * 2.0f * s_PaintRadius;

    while ( m_InstanceOffsets.size() && ( stamp || ( instanceTimer.Elapsed() < maxTime ) ) )
    {
        V_Vector3::iterator itr = m_InstanceOffsets.begin();

        Matrix4 instanceTransform;
        instanceTransform.t = Vector4( *itr );
        instanceTransform *= Matrix4( AngleAxis::Rotation( SceneGraph::SideVector, m_InstanceNormal ) );
        instanceTransform.t += Vector4( m_InstanceTranslation );

        Vector3 point = Vector3( instanceTransform.t.x, instanceTransform.t.y, instanceTransform.t.z );
        LinePickVisitor pick( m_Scene->GetViewport()->GetCamera(), Line( point + instanceNormalOffset, point - instanceNormalOffset ) );

        Vector3 instanceTranslation;
        Vector3 instanceNormal;
        if ( DetermineTranslationAndNormal( pick, instanceTranslation, instanceNormal ) )
        {
            point = Vector3( instanceTranslation.x - m_InstanceTranslation.x, instanceTranslation.y - m_InstanceTranslation.y, instanceTranslation.z - m_InstanceTranslation.z );
            if ( point.Length() <= s_PaintRadius )
            {
                CreateSingleObject( instanceTranslation, instanceNormal, true );
            }
        }

        m_InstanceOffsets.erase( itr );
    }
}

void CreateTool::TimerCallback( const TimerTickArgs& args )
{
    CreateMultipleObjects();
    m_Scene->Execute(true);
}

int CreateTool::GetPlaneSnap() const
{
    return s_PlaneSnap;
}

void CreateTool::SetPlaneSnap(int snap)
{
    s_PlaneSnap = (IntersectionPlane)snap;

    m_Scene->Execute(false);
}

bool CreateTool::GetSurfaceSnap() const
{
    return s_SurfaceSnap;
}

void CreateTool::SetSurfaceSnap(bool snap)
{
    s_SurfaceSnap = snap;

    if (s_SurfaceSnap)
    {
        s_ObjectSnap = false;
        m_Generator->GetContainer()->Read();
    }

    m_Scene->Execute(false);
}

bool CreateTool::GetLiveObjectsOnly() const
{
    return s_LiveObjectsOnly;
}

void CreateTool::SetLiveObjectsOnly(bool snap)
{
    s_LiveObjectsOnly = snap;
}

bool CreateTool::GetObjectSnap() const
{
    return s_ObjectSnap;
}

void CreateTool::SetObjectSnap(bool snap)
{
    s_ObjectSnap = snap;

    if (s_ObjectSnap)
    {
        s_SurfaceSnap = false;
        m_Generator->GetContainer()->Read();
    }

    m_Scene->Execute(false);
}

bool CreateTool::GetNormalSnap() const
{
    return s_NormalSnap;
}

void CreateTool::SetNormalSnap(bool snap)
{
    s_NormalSnap = snap;

    m_Scene->Execute(false);
}

bool CreateTool::GetRandomizeAzimuth() const
{
    return s_RandomizeAzimuth;
}

void CreateTool::SetRandomizeAzimuth(bool value)
{
    s_RandomizeAzimuth = value;
    m_AzimuthMin->a_IsEnabled.Set(value);
    m_AzimuthMax->a_IsEnabled.Set(value);
}

float CreateTool::GetAzimuthMin() const
{
    return s_AzimuthMin;
}

void CreateTool::SetAzimuthMin(float value)
{
    s_AzimuthMin = value;
}

float CreateTool::GetAzimuthMax() const
{
    return s_AzimuthMax;
}

void CreateTool::SetAzimuthMax(float value)
{
    s_AzimuthMax = value;
}

bool CreateTool::GetRandomizeDirection() const
{
    return s_RandomizeDirection;
}

void CreateTool::SetRandomizeDirection(bool value)
{
    s_RandomizeDirection = value;
    m_DirectionMin->a_IsEnabled.Set(value);
    m_DirectionMax->a_IsEnabled.Set(value);
}

float CreateTool::GetDirectionMin() const
{
    return s_DirectionMin;
}

void CreateTool::SetDirectionMin(float value)
{
    s_DirectionMin = value;
}

float CreateTool::GetDirectionMax() const
{
    return s_DirectionMax;
}

void CreateTool::SetDirectionMax(float value)
{
    s_DirectionMax = value;
}

bool CreateTool::GetRandomizeScale() const
{
    return s_RandomizeScale;
}

void CreateTool::SetRandomizeScale(bool value)
{
    s_RandomizeScale = value;
    m_ScaleMin->a_IsEnabled.Set(value);
    m_ScaleMax->a_IsEnabled.Set(value);
}

float CreateTool::GetScaleMin() const
{
    return s_ScaleMin;
}

void CreateTool::SetScaleMin(float value)
{
    s_ScaleMin = value;
}

float CreateTool::GetScaleMax() const
{
    return s_ScaleMax;
}

void CreateTool::SetScaleMax(float value)
{
    s_ScaleMax = value;
}

bool CreateTool::GetPaintMode() const
{
    return s_PaintMode;
}

void CreateTool::SetPaintMode(bool value)
{
    s_PaintMode = value;
    m_PaintPreventAnyOverlap->a_IsEnabled.Set(s_PaintMode);
    m_PaintPlacementStyle->a_IsEnabled.Set(s_PaintMode);
    m_PaintDistributionStyle->a_IsEnabled.Set(s_PaintMode);
    m_PaintRadius->a_IsEnabled.Set(s_PaintMode);
    m_PaintSpeed->a_IsEnabled.Set(s_PaintMode);
    m_PaintDensity->a_IsEnabled.Set(s_PaintMode);
    m_PaintJitter->a_IsEnabled.Set(s_PaintMode);
}

bool CreateTool::GetPaintPreventAnyOverlap() const
{
    return s_PaintPreventAnyOverlap;
}

void CreateTool::SetPaintPreventAnyOverlap(bool value)
{
    s_PaintPreventAnyOverlap = value;
}

float CreateTool::GetPaintRadius() const
{
    return s_PaintRadius;
}

void CreateTool::SetPaintRadius(float value)
{
    s_PaintRadius = value;
}

int CreateTool::GetPaintSpeed() const
{
    return s_PaintSpeed;
}

void CreateTool::SetPaintSpeed(int value)
{
    s_PaintSpeed = value;
}

float CreateTool::GetPaintDensity() const
{
    return s_PaintDensity;
}

void CreateTool::SetPaintDensity(float value)
{
    s_PaintDensity = value;
}

float CreateTool::GetPaintJitter() const
{
    return s_PaintJitter;
}

void CreateTool::SetPaintJitter(float value)
{
    s_PaintJitter = value;
}

int CreateTool::GetPaintPlacementStyle() const
{
    return s_PaintPlacementStyle;
}

void CreateTool::SetPaintPlacementStyle( int style )
{
    s_PaintPlacementStyle = (PlacementStyle) style;
}

int CreateTool::GetPaintDistributionStyle() const
{
    return s_PaintDistributionStyle;
}

void CreateTool::SetPaintDistributionStyle( int style )
{
    s_PaintDistributionStyle = (DistributionStyle) style;
}

