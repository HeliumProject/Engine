#pragma once

#include "Core/Scene/PivotTransform.h"
#include "Core/Scene/CurveControlPoint.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Core/Scene/VertexResource.h"

namespace Helium
{
    namespace Math
    {
        class Vector3;
        class Matrix4;
    }

    namespace Core
    {
        class PrimitiveLocator;
        class PrimitiveCone;

        namespace CurveTypes
        {
            enum CurveType
            {
                Linear,
                BSpline,
                CatmullRom,
            };

            static void CurveTypeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Linear, TXT( "Linear" ) );
                info->AddElement(BSpline, TXT( "BSpline" ) );
                info->AddElement(CatmullRom, TXT( "CatmullRom" ) );
            }
        }
        typedef CurveTypes::CurveType CurveType;

        namespace ControlPointLabels
        {
            enum ControlPointLabel
            {
                None,
                CurveAndIndex,
                IndexOnly
            };

            static void ControlPointLabelEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( None, TXT( "None" ) );
                info->AddElement( CurveAndIndex, TXT( "Curve and Index" ) );
                info->AddElement( IndexOnly, TXT( "Index only" ) );
            }
        }
        typedef ControlPointLabels::ControlPointLabel ControlPointLabel;

        class Curve : public PivotTransform
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Curve, PivotTransform );
            static void EnumerateClass( Reflect::Compositor<Curve>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Curve();
            ~Curve();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Initialize() HELIUM_OVERRIDE;

            // Curve Type
            int GetCurveType() const;
            void SetCurveType( int value );

            // Curve closed
            bool GetClosed() const;
            void SetClosed( bool value );

            // Curve resolution
            u32 GetResolution() const;
            void SetResolution( u32 value );

            // Control point labeling
            int GetControlPointLabel() const;
            void SetControlPointLabel( int value );

            // the single closest control point
            int ClosestControlPoint( PickVisitor* pick );

            // the two closest control points
            bool ClosestControlPoints(PickVisitor* pick, std::pair<u32, u32>& result);

            // the closest point on the computed curve
            i32 ClosestPoint( PickVisitor* pick );

            // the closest point on the computed curve to the provided point
            i32 ClosestPoint(Math::Vector3& pos);

            // the number of control points the determine the computed curve
            u32 GetNumberControlPoints() const;

            // get control point by index
            CurveControlPoint* GetControlPointByIndex( u32 index );

            // linear seach for the index of a particular point
            i32 GetIndexForControlPoint( CurveControlPoint* pc );

            // insert new control point
            Undo::CommandPtr InsertControlPointAtIndex( u32 index, CurveControlPoint* pc );

            // remove existing control point
            Undo::CommandPtr RemoveControlPointAtIndex( u32 index );

            // reverse curve direction
            Undo::CommandPtr ReverseControlPoints();

            void ProjectPointOnCurve( const Math::Vector3& point, Math::Vector3& projectedPoint ) const;
            f32 DistanceSqrToCurve( const Math::Vector3& point ) const;
            f32 DistanceToCurve( const Math::Vector3& point ) const;

            //
            // Resources
            //

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;
            virtual void Populate(PopulateArgs* args);


            //
            // Evaluate/Render/Pick
            //

            virtual Undo::CommandPtr CenterTransform() HELIUM_OVERRIDE;

            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;
            f32 CalculateCurveLength() const;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void Draw( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            //
            // UI
            //

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

        private:
            static void OnReverseControlPoints( const Inspect::ButtonClickedArgs& args );
            void ChildChangingParents( const ParentChangingArgs& args );

        protected:
            // Reflected
            bool                    m_Closed;               // Is the curve closed?
            CurveType               m_Type;                 // The degree of the curve basis function
            u32                     m_Resolution;           // The resolution of the points to compute
            ControlPointLabel       m_ControlPointLabel;    // How to display labels for control points
            Math::V_Vector3         m_Points;               // The 3D locations of the computed curve points

            // Non-reflected
            PrimitiveLocator* m_Locator;
            PrimitiveCone*    m_Cone;
            static D3DMATERIAL9     s_Material;
            static D3DMATERIAL9     s_HullMaterial;
            VertexResourcePtr       m_Vertices;
        };

        typedef Helium::SmartPtr<Curve> CurvePtr;
    }
}