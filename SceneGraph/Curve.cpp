#include "SceneGraphPch.h"
#include "Curve.h"

#include "Foundation/Log.h"
#include "Application/UndoQueue.h"

#include "Math/CalculateCurve.h"
#include "Math/AngleAxis.h"
#include "MathSimd/Quat.h"

#include "Graphics/BufferedDrawer.h"

#include "SceneGraph/Pick.h"
#include "SceneGraph/PrimitiveLocator.h"
#include "SceneGraph/PrimitiveCone.h"
#include "SceneGraph/Color.h"
#include "SceneGraph/Scene.h"
#include "SceneGraph/Statistics.h"
#include "SceneGraph/ReverseChildrenCommand.h"
#include "SceneGraph/PropertiesGenerator.h"
#include "SceneGraph/Orientation.h"

#include <algorithm>

HELIUM_DEFINE_ENUM( Helium::SceneGraph::CurveType );
HELIUM_DEFINE_ENUM( Helium::SceneGraph::ControlPointLabel );
HELIUM_DEFINE_CLASS( Helium::SceneGraph::Curve );

using namespace Helium;
using namespace Helium::SceneGraph;

Helium::Color Curve::s_Material = SceneGraph::Color::FORESTGREEN;
Helium::Color Curve::s_HullMaterial = SceneGraph::Color::GRAY;

void Curve::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Curve::m_Closed,               TXT( "m_Closed" ) );
	comp.AddField( &Curve::m_Type,                 TXT( "m_Type" ) );
	comp.AddField( &Curve::m_ControlPointLabel,    TXT( "m_ControlPointLabel" ) );
	comp.AddField( &Curve::m_Resolution,           TXT( "m_Resolution" ) );
}

Curve::Curve()
	: m_Closed( false )
	, m_Type( CurveType::Linear )
	, m_Resolution( 10 )
	, m_ControlPointLabel( ControlPointLabel::None )
	, m_Locator ( NULL )
	, m_Cone ( NULL )
{
}

Curve::~Curve()
{
	delete m_Locator;
	delete m_Cone;
}

void Curve::Initialize()
{
	Base::Initialize();

	OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
	for ( ; childItr != childEnd; ++childItr )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( point )
		{
			point->AddParentChangingListener( ParentChangingSignature::Delegate( this, &Curve::ChildChangingParents ) );
		}
	}

	m_Locator = new PrimitiveLocator;
	m_Locator->Update();

	m_Cone = new PrimitiveCone;
	m_Cone->m_Radius = 0.2f;
	m_Cone->SetSolid(true);
	m_Cone->Update();

	m_Vertices = new VertexResource;
	m_Vertices->SetElementType( VertexElementTypes::SimpleVertex );
	m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &Curve::Populate ) );
}

int Curve::GetCurveType() const 
{ 
	return (int)m_Type;
}

void Curve::SetCurveType( int value )
{
	m_Type = static_cast< CurveType::Enum >( value );

	Dirty();
}

bool Curve::GetClosed() const
{  
	return m_Closed;
}

void Curve::SetClosed( bool value )
{
	m_Closed = value;

	Dirty();
}

uint32_t Curve::GetResolution() const
{
	return m_Resolution;
}

void Curve::SetResolution( uint32_t value )
{
	m_Resolution = value;

	Dirty();
}

int Curve::GetControlPointLabel() const
{
	return (int)m_ControlPointLabel;
}

void Curve::SetControlPointLabel( int value )
{
	m_ControlPointLabel = static_cast< ControlPointLabel::Enum >( value );

	Dirty();
}

int Curve::ClosestControlPoint( PickVisitor* pick )
{
	int bestIndex = -1;
	float distance = 0.0f;
	float bestDistance = 0.0f;

	pick->SetCurrentObject (this, GetGlobalTransform(), GetInverseGlobalTransform() );

	OS_HierarchyNodeDumbPtr::Iterator itr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator end = GetChildren().End();
	for ( uint32_t i = 0; itr != end; ++itr, ++i )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *itr );
		if ( point )
		{
			if ( pick->PickPoint( point->GetPosition(), FLT_MAX ) )
			{
				distance = pick->GetHits().back()->GetIntersectionDistance();

				if (bestIndex == -1 || distance < bestDistance )
				{
					bestDistance = distance;
					bestIndex = i;
				}
			}
		}
	}

	return bestIndex;
}

bool Curve::ClosestControlPoints( PickVisitor* pick, std::pair<uint32_t, uint32_t>& result )
{
	CurveControlPoint* previous = NULL;
	CurveControlPoint* current = NULL;
	int32_t previousIndex = -1;
	int32_t currentIndex = -1;
	int32_t bestIndex = -1;
	int32_t bestPreviousIndex = -1;
	float32_t distance = 0.0f;
	float32_t bestDistance = 0.0f;

	// need to have at least 2 points!
	if ( GetNumberControlPoints() < 2 )
	{
		return false;
	}

	pick->SetCurrentObject ( this, GetGlobalTransform(), GetInverseGlobalTransform() );

	OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
	for ( uint32_t index = 0; childItr != childEnd; ++childItr )
	{
		current = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( current )
		{
			currentIndex = index;
			if ( previous )
			{
				Vector3 point1 = previous->GetPosition();
				Vector3 point2 = current->GetPosition();

				if ( pick->PickSegment( point1, point2, -1.0f ) )
				{
					distance = pick->GetHits().back()->GetIntersectionDistance();

					if ( bestIndex == -1 || distance < bestDistance )
					{
						bestDistance = distance;
						bestIndex = currentIndex;
						bestPreviousIndex = previousIndex;
					}
				}
			}

			previousIndex = currentIndex;
			previous = current;
			++index;
		}
	}

	if ( bestIndex != -1 ) 
	{
		result.first = bestPreviousIndex;
		result.second = bestIndex;
		return true;
	}

	return false;
}

int32_t Curve::ClosestPoint( PickVisitor* pick )
{
	int32_t bestIndex = -1;
	float32_t distance = FLT_MAX;
	float32_t bestDistance = FLT_MAX;
	Vector3 point;

	pick->SetCurrentObject ( this, GetGlobalTransform(), GetInverseGlobalTransform() );

	uint32_t curvePointCount = (uint32_t)m_Points.size();
	for ( uint32_t i = 0; i < curvePointCount; ++i )
	{
		point = m_Points[i];  

		if ( pick->PickPoint (point, FLT_MAX ) )
		{
			distance = pick->GetHits().back()->GetIntersectionDistance();

			if ( bestIndex == -1 || distance < bestDistance )
			{
				bestDistance = distance;
				bestIndex = i;
			}
		}
	}

	return bestIndex;
}

int32_t Curve::ClosestPoint(Vector3& pos)
{
	int bestIndex = -1;
	float distance = 0.0f;
	float bestDistance = 0.0f;
	Vector3 point;

	const Matrix4& globalTransform = this->GetGlobalTransform();

	V_Vector3::iterator itr = m_Points.begin();
	V_Vector3::iterator end = m_Points.end();
	for ( uint32_t i = 0; itr != end; ++itr, ++i )
	{
		point = *itr;
		globalTransform.TransformVertex( point ); 

		distance = (pos - point).Length();

		if ( bestIndex == -1 || distance < bestDistance )
		{
			bestDistance = distance;
			bestIndex = i;
		}
	}

	return bestIndex;
}

uint32_t Curve::GetNumberControlPoints() const
{
	uint32_t count = 0;
	OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
	for ( ; childItr != childEnd; ++childItr )
	{
		if ( ( *childItr )->IsA( Reflect::GetMetaClass< CurveControlPoint >() ) )
		{
			++count;
		}
	}

	return count;
}

CurveControlPoint* Curve::GetControlPointByIndex( uint32_t index )
{
	CurveControlPoint* controlPoint = NULL;

	OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
	for ( uint32_t i = 0; childItr != childEnd; ++childItr )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( point )
		{
			if ( i == index )
			{
				controlPoint = point;
				break;
			}
			++i;
		}
	}

	return controlPoint;
}

int32_t Curve::GetIndexForControlPoint( CurveControlPoint* pc )
{
	int32_t index = -1;

	OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
	for ( uint32_t i = 0; childItr != childEnd; ++childItr )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( point )
		{
			if ( point == pc )
			{
				index = i;
				break;
			}
			++i;
		}
	}

	return index;
}

UndoCommandPtr Curve::RemoveControlPointAtIndex( uint32_t index )
{
	OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
	for ( uint32_t i = 0; childItr != childEnd; ++childItr )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( point )
		{
			if ( i == index )
			{
				UndoCommandPtr command = new SceneNodeExistenceCommand( ExistenceActions::Remove, m_Owner, point );
				Dirty();
				return command;
			}
			++i;
		}
	}
	return NULL;
}

UndoCommandPtr Curve::InsertControlPointAtIndex( uint32_t index, CurveControlPoint* pc )
{
	HierarchyNode* previous = NULL;
	HierarchyNode* next = NULL;
	OS_HierarchyNodeDumbPtr::Iterator childItr = m_Children.Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Children.End();
	for ( uint32_t i = 0; childItr != childEnd; ++childItr )
	{
		CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
		if ( point )
		{
			if ( i == index )
			{
				next = point;
				break;
			}
			++i;
		}
		previous = *childItr;
	}

	pc->SetPrevious( previous );
	pc->SetNext( next );
	pc->SetParent( this );

	UndoCommandPtr command = new SceneNodeExistenceCommand( ExistenceActions::Add, m_Owner, pc );
	Dirty();
	return command;
}

UndoCommandPtr Curve::ReverseControlPoints()
{
	return new ReverseChildrenCommand( this );
}

void Curve::ProjectPointOnCurve( const Vector3& point, Vector3& projectedPoint ) const
{
	Vector3 closestPoint;

	uint32_t size = (uint32_t)m_Points.size();
	HELIUM_ASSERT( size >= 2 );

	Line segment( m_Points[0], m_Points[1] );
	segment.Transform( m_GlobalTransform );
	segment.ProjectPointOnSegment( point, closestPoint );
	float32_t closestDistSqr = ( point - closestPoint ).LengthSquared();

	for( uint32_t i = 1; i < size-1; ++i )
	{
		segment.m_Origin = m_Points[i];
		segment.m_Point  = m_Points[i+1];
		segment.Transform( m_GlobalTransform );
		segment.ProjectPointOnSegment( point, projectedPoint );

		float32_t distSqr = (point - projectedPoint ).LengthSquared();

		if(  distSqr < closestDistSqr )
		{
			closestDistSqr = distSqr;
			closestPoint = projectedPoint;
		}
	}
	if( m_Closed )
	{
		segment.m_Origin = m_Points[size-1];
		segment.m_Point  = m_Points[0];
		segment.Transform( m_GlobalTransform );
		segment.ProjectPointOnSegment( point, projectedPoint );

		float32_t distSqr = (point - projectedPoint ).LengthSquared();

		if(  distSqr < closestDistSqr )
		{
			closestDistSqr = distSqr;
			closestPoint = projectedPoint;
		}
	}
	projectedPoint = closestPoint;
}

float32_t Curve::DistanceSqrToCurve( const Vector3& point ) const
{
	Vector3 projectedPoint;
	ProjectPointOnCurve( point, projectedPoint );

	return (point - projectedPoint).LengthSquared();
}

float32_t Curve::DistanceToCurve( const Vector3& point ) const
{
	return sqrt( DistanceSqrToCurve(point) );
}

void Curve::Create()
{
	Base::Create();

	m_Vertices->Create();
	m_Locator->Create();
	m_Cone->Create();
}

void Curve::Delete()
{
	Base::Delete();

	// If the curve is added back, it's control points might not be added yet,
	// so we need to zero the element count and it will be recalculated in 
	// Evaluate(), after all the control points are re-added to the scene.
	// If we don't do this, there will be an assert about a buffer underrun in
	// Populate().
	m_Vertices->SetElementCount( 0 );
	m_Vertices->Delete();
	m_Locator->Delete();
	m_Cone->Delete();
}

void Curve::Populate( PopulateArgs* args )
{
	switch( args->m_Type )
	{
	case ResourceTypes::Vertex:
		{
			if ( args->m_Buffer != NULL )
			{
				m_ObjectBounds.Reset();

				const uint32_t vertexCount = m_Vertices->GetElementCount();

				if ( vertexCount == 0 )
					break;

				Helium::SimpleVertex* bufferStart = reinterpret_cast<Helium::SimpleVertex*>( args->m_Buffer + args->m_Offset );

				// go over the control points
				uint32_t countControlPoints = 0;
				CurveControlPoint* firstPoint = NULL;
				OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
				OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
				for ( ; childItr != childEnd; ++childItr )
				{
					CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
					if ( point )
					{
						if ( !firstPoint )
						{
							firstPoint = point;
						}
						Vector3 position = m_ObjectBounds.Test( point->GetPosition() );
						bufferStart[ countControlPoints ].position[ 0 ] = position.x;
						bufferStart[ countControlPoints ].position[ 1 ] = position.y;
						bufferStart[ countControlPoints ].position[ 2 ] = position.z;
						bufferStart[ countControlPoints ].color[ 0 ] = 0xff;
						bufferStart[ countControlPoints ].color[ 1 ] = 0xff;
						bufferStart[ countControlPoints ].color[ 2 ] = 0xff;
						bufferStart[ countControlPoints ].color[ 3 ] = 0xff;
						args->m_Offset += sizeof(Helium::SimpleVertex); 
						++countControlPoints;
					}
				}

				// loop back for closed curves
				if ( countControlPoints > 0 ) 
				{
					Vector3 position = m_ObjectBounds.Test( firstPoint->GetPosition() );
					bufferStart[ countControlPoints ].position[ 0 ] = position.x;
					bufferStart[ countControlPoints ].position[ 1 ] = position.y;
					bufferStart[ countControlPoints ].position[ 2 ] = position.z;
					bufferStart[ countControlPoints ].color[ 0 ] = 0xff;
					bufferStart[ countControlPoints ].color[ 1 ] = 0xff;
					bufferStart[ countControlPoints ].color[ 2 ] = 0xff;
					bufferStart[ countControlPoints ].color[ 3 ] = 0xff;
					args->m_Offset += sizeof(Helium::SimpleVertex); 
				}

				// reset the buffer start to its new location
				bufferStart = reinterpret_cast<Helium::SimpleVertex*>( args->m_Buffer + args->m_Offset );

				// go over the calculated curve points
				uint32_t countCurvePoints = (uint32_t)m_Points.size();
				for ( uint32_t i = 0; i < countCurvePoints; ++i )
				{
					Vector3 position = m_ObjectBounds.Test( m_Points[ i ] );
					bufferStart[ i ].position[ 0 ] = position.x;
					bufferStart[ i ].position[ 1 ] = position.y;
					bufferStart[ i ].position[ 2 ] = position.z;
					bufferStart[ i ].color[ 0 ] = 0xff;
					bufferStart[ i ].color[ 1 ] = 0xff;
					bufferStart[ i ].color[ 2 ] = 0xff;
					bufferStart[ i ].color[ 3 ] = 0xff;
					args->m_Offset += sizeof(Helium::SimpleVertex); 
				}

				// loop back for closed curves
				if ( countCurvePoints > 0 ) 
				{
					Vector3 position = m_ObjectBounds.Test( m_Points[ 0 ] );
					bufferStart[ countCurvePoints ].position[ 0 ] = position.x;
					bufferStart[ countCurvePoints ].position[ 1 ] = position.y;
					bufferStart[ countCurvePoints ].position[ 2 ] = position.z;
					bufferStart[ countCurvePoints ].color[ 0 ] = 0xff;
					bufferStart[ countCurvePoints ].color[ 1 ] = 0xff;
					bufferStart[ countCurvePoints ].color[ 2 ] = 0xff;
					bufferStart[ countCurvePoints ].color[ 3 ] = 0xff;
					args->m_Offset += sizeof(Helium::SimpleVertex); 
				}
			}
			break;
		}

	case ResourceTypes::Index:
		break;
	}
}

UndoCommandPtr Curve::CenterTransform()
{
	BatchUndoCommandPtr batch = new BatchUndoCommand();

	batch->Push( Base::CenterTransform() );

	if ( GetNumberControlPoints() == 0 )
	{
		return batch;
	}


	//
	// We are going to move all control points, so just snap shot
	//

	batch->Push( SnapShot() );


	//
	// Compute the centered position
	//

	Vector3 position = Vector3::Zero;

	{
		uint32_t controlPointCount = 0;
		OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
		OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
		for ( ; childItr != childEnd; ++childItr )
		{
			CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
			if ( point )
			{
				++controlPointCount;
				position += point->GetPosition();
			}
		}
		position /= (float32_t)controlPointCount;
	}

	m_GlobalTransform.TransformVertex( position );


	//
	// Offset the control points
	//

	Matrix4 m = m_GlobalTransform;

	m.t = position;

	m = m_GlobalTransform * m.Inverted();

	{
		OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
		OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
		for ( ; childItr != childEnd; ++childItr )
		{
			CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
			if ( point )
			{
				Vector3 p = point->GetPosition();

				m.TransformVertex( p );

				batch->Push( new PropertyUndoCommand< Vector3 >( new Helium::MemberProperty< CurveControlPoint, Vector3 >( point, &CurveControlPoint::GetPosition, &CurveControlPoint::SetPosition ), p ) );
			}
		}
	}


	//
	// Recompute global transform
	//

	// our new global transform is just translated to the new position
	m_GlobalTransform.t = position;

	// this will recompute the local components
	SetGlobalTransform( m_GlobalTransform );

	// update the transform object
	Evaluate( GraphDirections::Downstream );

	// update each child's local transform to stay in the same global position
	for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
	{
		HierarchyNode* n = *itr;

		Transform* t = Reflect::SafeCast<Transform>( n );

		if ( !t )
		{
			continue;
		}

		batch->Push( t->ComputeObjectComponents() );
	}

	Dirty();

	return batch;
}

float32_t Curve::CalculateCurveLength() const
{
	const Matrix4& globalTransform = this->GetGlobalTransform();
	V_Vector3 points = m_Points;
	float32_t curveLength = 0.f;
	for ( uint32_t i = 1; i < points.size() ; ++i )
	{
		Vector3 endPoint = points[i];
		Vector3 startPoint = points[i-1];
		globalTransform.TransformVertex( endPoint );
		globalTransform.TransformVertex( startPoint );
		curveLength += (endPoint - startPoint).Length();
	}
	return curveLength;
}

void Curve::Evaluate( GraphDirection direction )
{
	uint32_t controlCount = 0;
	V_Vector3 points;
	{
		OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
		OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
		for ( ; childItr != childEnd; ++childItr )
		{
			CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
			if ( point )
			{
				points.push_back( point->GetPosition() );
				++controlCount;
			}
		}
	}

	if ( controlCount < 4  || m_Type == CurveType::Linear ) 
	{     
		m_Points = points;
	}
	else if ( m_Type == CurveType::BSpline )
	{
		CurveGenerator::ComputeCurve( points, m_Resolution, m_Closed, CurveGenerator::kBSpline, m_Points ); 
	}
	else if ( m_Type == CurveType::CatmullRom )
	{
		CurveGenerator::ComputeCurve( points, m_Resolution, m_Closed, CurveGenerator::kCatmullRom, m_Points ); 
	}
	else
	{
		//whoa, did we get a bum curve type?
		HELIUM_BREAK();
	}


	//
	// Update buffer
	//

	if ( controlCount > 0 ) 
	{
		controlCount++;
	}

	uint32_t pointCount = (uint32_t)m_Points.size();
	if ( pointCount > 0 ) 
	{
		pointCount++;
	}

	m_Vertices->SetElementCount( controlCount + pointCount );
	m_Vertices->Update();

	Base::Evaluate(direction);
}

void Curve::Render( RenderVisitor* render )
{
	HELIUM_ASSERT( render );

	DrawArgs* args = render->GetArgs();
	HELIUM_ASSERT( args );

	Helium::BufferedDrawer* drawInterface = render->GetDrawInterface();
	HELIUM_ASSERT( drawInterface );

	const VertexResource* vertices = m_Vertices;

	uint32_t countCurvePoints    = (uint32_t)m_Points.size();
	uint32_t countControlPoints  = GetNumberControlPoints();

	//
	//  Draw start end end locators
	//

	Helium::Color materialColor = GetMaterialColor( s_Material );

	Simd::Matrix44 globalTransform( GetGlobalTransform().GetArray1d() );

	if ( !m_Closed )
	{
		Simd::Matrix44 m;

		if ( countCurvePoints > 0 )
		{
			Simd::Vector3 point( &m_Points[ 0 ].x );
			m.MultiplySet( Simd::Matrix44( Simd::Matrix44::INIT_TRANSLATION, point ), globalTransform );
			m_Locator->Draw( drawInterface, args, materialColor, m );
		}

		if ( countCurvePoints > 1 )
		{
			Simd::Vector3 point( &m_Points[ countCurvePoints - 1 ].x );
			m.MultiplySet( Simd::Matrix44( Simd::Matrix44::INIT_TRANSLATION, point ), globalTransform );
			m_Locator->Draw( drawInterface, args, materialColor, m );

			Simd::Vector3 p1( &m_Points[ countCurvePoints - 2 ].x );
			Simd::Vector3 p2( &m_Points[ countCurvePoints - 1 ].x );
			Simd::Vector3 dir = ( p2 - p1 ).GetNormalized();
			AngleAxis angleAxis = AngleAxis::Rotation(
				OutVector,
				Vector3( dir.GetElement( 0 ), dir.GetElement( 1 ), dir.GetElement( 2 ) ) );
			m.SetRotationTranslation(
				Simd::Quat( Simd::Vector3( &angleAxis.axis.x ), angleAxis.angle ),
				p2 - ( dir * ( m_Cone->m_Length * 0.5f ) ) );
			m *= globalTransform;

			m_Cone->Draw( drawInterface, args, materialColor, m );
		}
	}

	if ( countCurvePoints > 0 ) 
	{
		//
		//  Draw Curve
		//
		uint32_t countCurveLines = m_Closed ? countCurvePoints : countCurvePoints - 1;

		if ( countCurveLines > 0 )
		{
			drawInterface->DrawUntextured(
				Helium::RENDERER_PRIMITIVE_TYPE_LINE_STRIP,
				globalTransform,
				vertices->GetBuffer(),
				NULL,
				vertices->GetBaseIndex() + countControlPoints + 1,
				countCurveLines + 1,
				0,
				countCurveLines,
				materialColor,
				Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
			args->m_LineCount += countCurveLines;
		}

		//
		//  Draw Curve points 
		//
		drawInterface->DrawPoints(
			globalTransform,
			vertices->GetBuffer(),
			vertices->GetBaseIndex() + countControlPoints + 1,
			countCurvePoints,
			materialColor );
	}


	//
	//  Draw Control points
	//

	if ( countControlPoints > 0 )
	{
		//
		// Draw points hull
		//

		if ( m_Type != CurveType::Linear )
		{
			uint32_t countControlLines = m_Closed ? countControlPoints : countControlPoints - 1;

			if ( countControlLines > 0 )
			{
				drawInterface->DrawUntextured(
					Helium::RENDERER_PRIMITIVE_TYPE_LINE_STRIP,
					globalTransform,
					vertices->GetBuffer(),
					NULL,
					vertices->GetBaseIndex(),
					countControlLines + 1,
					0,
					countControlLines,
					s_HullMaterial,
					Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
				args->m_LineCount += countControlLines;
			}
		}


		//
		// Draw all points
		//

		drawInterface->DrawPoints(
			globalTransform,
			vertices->GetBuffer(),
			vertices->GetBaseIndex(),
			countControlPoints,
			Viewport::s_ComponentMaterial );


		//
		//  Overdraw selected points
		//
		{
			Helium::Color textColor( 0xffffffff );

			OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
			OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
			for ( uint32_t i = 0; childItr != childEnd; ++childItr )
			{
				CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
				if ( point )
				{
					if ( point->IsSelected() )
					{
						drawInterface->DrawPoints(
							globalTransform,
							vertices->GetBuffer(),
							vertices->GetBaseIndex() + i,
							1,
							Viewport::s_SelectedComponentMaterial);
					}

					if ( GetControlPointLabel() != ControlPointLabel::None )
					{
						std::stringstream label;
						switch ( GetControlPointLabel() )
						{
						case ControlPointLabel::CurveAndIndex:
							label << GetName() << TXT( "[" ) << i << TXT( "]" );
							break;

						case ControlPointLabel::IndexOnly:
							label << "[" << i << "]";
							break;
						}

						Simd::Vector3 position( &point->GetPosition().x );

						// local to global
						globalTransform.TransformPoint( position, position );

						const int32_t offsetX = 0;
						const int32_t offsetY = 15;

						drawInterface->DrawProjectedText(
							position,
							offsetX,
							offsetY,
							String( label.str().c_str() ),
							textColor,
							Helium::RenderResourceManager::DEBUG_FONT_SIZE_SMALL );
					}
					++i;
				}
			}
		}

		//
		//  Overdraw highlighted points
		// 
		{
			OS_HierarchyNodeDumbPtr::Iterator childItr = GetChildren().Begin();
			OS_HierarchyNodeDumbPtr::Iterator childEnd = GetChildren().End();
			for ( uint32_t i = 0; childItr != childEnd; ++childItr )
			{
				CurveControlPoint* point = Reflect::SafeCast< CurveControlPoint >( *childItr );
				if ( point )
				{
					if ( point->IsHighlighted() )
					{
						drawInterface->DrawPoints(
							globalTransform,
							vertices->GetBuffer(),
							vertices->GetBaseIndex() + i,
							1,
							Viewport::s_HighlightedMaterial);
					}
					++i;
				}
			}
		}
	}

	Base::Render( render );
}

bool Curve::Pick( PickVisitor* pick )
{  
	bool pickHit = false;

	Vector3 normal ( UpVector );
	pick->State().m_Matrix.TransformNormal( normal );

	pick->SetCurrentObject (this, pick->State().m_Matrix);

	if ( !m_Points.empty() )
	{
		//
		// Pick Curve Points
		//

		for ( size_t i=0; i < m_Points.size() && !pickHit; ++i )
		{
			pickHit |= pick->PickPoint (m_Points[ i ]); 
		}


		//
		// Pick Curve Lines
		//

		for ( size_t i=0; i < m_Points.size() - 1 && !pickHit; ++i )
		{
			pickHit |= pick->PickSegment (m_Points[ i ], m_Points[ i + 1 ]); 
		}


		//
		// Pick locators
		//

		if ( !pickHit )
		{
			Vector3 startLocator (m_Points[ 0 ]);
			pick->State().m_Matrix.TransformVertex (startLocator);
			pick->SetCurrentObject (this, Matrix4 (startLocator));
			pickHit = m_Locator->Pick (pick);
		}

		if ( !pickHit )
		{
			Vector3 endLocator (m_Points[ m_Points.size() - 1 ]);
			pick->State().m_Matrix.TransformVertex (endLocator);
			pick->SetCurrentObject (this, Matrix4 (endLocator));
			pickHit = m_Locator->Pick (pick);
		}
	}

	return pickHit;
}

void Curve::ChildChangingParents( const ParentChangingArgs& args )
{
	// Prevent rearranging points (for now)
	args.m_Veto = true;
}
