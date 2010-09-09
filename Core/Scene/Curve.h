#pragma once

#include "PivotTransform.h"
#include "Core/Scene/Point.h"
#include "VertexResource.h"

#include "Core/Content/Nodes/ContentCurve.h"

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

        class Curve : public Core::PivotTransform
        {
            //
            // Members
            //

        protected:

            // start and end locator
            Core::PrimitiveLocator* m_Locator;

            // directional cone
            Core::PrimitiveCone* m_Cone;

            // materials to use for rendering
            static D3DMATERIAL9 s_Material;
            static D3DMATERIAL9 s_HullMaterial;

            // resources
            VertexResourcePtr m_Vertices;


            //
            // RTTI
            //

            SCENE_DECLARE_TYPE( Core::Curve, Core::PivotTransform );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            Curve( Core::Scene* scene, Content::Curve* curve );
            virtual ~Curve();

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
            Core::Point* GetControlPointByIndex( u32 index );

            // linear seach for the index of a particular point
            i32 GetIndexForControlPoint( Core::Point* pc );

            // insert new control point
            Undo::CommandPtr InsertControlPointAtIndex( u32 index, Core::Point* pc );

            // remove existing control point
            Undo::CommandPtr RemoveControlPointAtIndex( u32 index );

            // reverse curve direction
            Undo::CommandPtr ReverseControlPoints();


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
        };

        typedef Helium::SmartPtr<Core::Curve> LCurvePtr;
    }
}