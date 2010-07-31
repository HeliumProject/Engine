#pragma once

#include "PivotTransform.h"
#include "Point.h"
#include "VertexResource.h"

#include "Pipeline/Content/Nodes/Curve.h"

namespace Helium
{
    namespace Math
    {
        class Vector3;
        class Matrix4;
    }

    namespace Editor
    {
        class PrimitiveLocator;
        class PrimitiveCone;

        class Curve : public Editor::PivotTransform
        {
            //
            // Members
            //

        protected:

            // start and end locator
            Editor::PrimitiveLocator* m_Locator;

            // directional cone
            Editor::PrimitiveCone* m_Cone;

            // materials to use for rendering
            static D3DMATERIAL9 s_Material;
            static D3DMATERIAL9 s_HullMaterial;

            // resources
            VertexResourcePtr m_Vertices;


            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE( Editor::Curve, Editor::PivotTransform );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            Curve( Editor::Scene* scene, Content::Curve* curve );
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
            Editor::Point* GetControlPointByIndex( u32 index );

            // linear seach for the index of a particular point
            i32 GetIndexForControlPoint( Editor::Point* pc );

            // insert new control point
            Undo::CommandPtr InsertControlPointAtIndex( u32 index, Editor::Point* pc );

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
            static void OnReverseControlPoints( Inspect::Button* button );
            bool ChildChangingParents( const ParentChangingArgs& args );
        };

        typedef Helium::SmartPtr<Editor::Curve> LCurvePtr;
    }
}