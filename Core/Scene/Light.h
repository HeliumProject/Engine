#pragma once

#include "Instance.h"
#include "Core/Content/Nodes/ContentLight.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Foundation/Container/BitArray.h"
#include "Primitive.h"

namespace Helium
{
    namespace Core
    {

        class SelectionDataObject : public Reflect::Object
        {
        public:

            OS_SelectableDumbPtr m_Selection;

            SelectionDataObject( const OS_SelectableDumbPtr& selection )
                : m_Selection( selection)
            {
            }

        };

        class PrimitiveLocator;
        class Light;
        class PrimitiveSphere;

        namespace LightSelectionChoices
        {
            enum LightSelectionChoice
            {
                RenderType = 0,
                Color      = 1,
                Intensity  = 2,
                Scale      = 3,
                LensFlare  = 4,
                PhysicalLight = 5,
                LightType    = 6,
                NumLightSelectionChoices,
            };
        };
        typedef LightSelectionChoices::LightSelectionChoice LightSelectionChoice;

        struct LightChangeArgs
        {
            Core::Light* m_Light;

            LightChangeArgs( Core::Light* light )
                : m_Light( light )
            {}
        };
        typedef Helium::Signature< void, const LightChangeArgs& > LightChangeSignature;

        struct RealtimeLightExistenceArgs
        {
            Core::Light* m_Light;
            bool    m_Added;

            RealtimeLightExistenceArgs( Core::Light* light, bool added )
                : m_Light( light )
                , m_Added( added )
            {}
        };
        typedef Helium::Signature< void, const RealtimeLightExistenceArgs& > RealtimeLightExistenceSignature;

        class CORE_API Light HELIUM_ABSTRACT : public Core::Instance
        {
            //
            // Members
            //

        protected:
            BitArray m_SelectionHelper;

        public:
            static D3DMATERIAL9 s_Material;

            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::Light, Core::Instance );
            static void InitializeType();
            static void CleanupType();

            //
            // Member functions
            //

            Light(Core::Scene* scene, Content::Light* light);

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            Math::Color3 GetColor() const;
            void SetColor( Math::Color3 color );

            float GetIntensity() const;
            void SetIntensity( float intensity );

            i32 GetRenderType() const;
            void SetRenderType( i32 renderType );

            i32 GetLensFlareType() const;
            void SetLensFlareType( i32 type );

            float GetDrawDist() const;
            void SetDrawDist( float distance );

            bool GetKillIfInactive() const;
            void SetKillIfInactive( bool b );

            bool GetVisibilityRayTest() const;
            void SetVisibilityRayTest( bool b );

            bool GetCastsShadows() const;
            void SetCastsShadows( bool b );

            bool GetAttenuate() const;
            void SetAttenuate( bool b );

            f32 GetShadowFactor() const;
            void SetShadowFactor( f32 radius );

            bool GetAllowOversized() const;
            void SetAllowOversized( bool b );

            bool GetEmitPhotons() const;
            void SetEmitPhotons( bool b );

            u32 GetNumPhotons() const;
            void SetNumPhotons( u32 numPhotons );

            Math::Color3 GetPhotonColor() const;
            void SetPhotonColor( Math::Color3 color );

            float GetPhotonIntensity() const;
            void SetPhotonIntensity( float intensity );

            f32 GetAnimationDuration() const;
            void SetAnimationDuration( f32 duration );

            bool GetRandomAnimOffset() const;
            void SetRandomAnimOffset( bool b );

            bool GetPhysicalLight() const;
            void SetPhysicalLight( bool b );

            bool GetSelectionHelperRenderType() const;
            void SetSelectionHelperRenderType( bool b );

            bool GetSelectionHelperColor() const;
            void SetSelectionHelperColor( bool b );

            bool GetSelectionHelperIntensity() const;
            void SetSelectionHelperIntensity( bool b );

            bool GetSelectionHelperScale() const;
            void SetSelectionHelperScale( bool b );

            bool GetSelectionHelperLensFlare() const;
            void SetSelectionHelperLensFlare( bool b );

            bool GetSelectionHelperPhysicalLight() const;
            void SetSelectionHelperPhysicalLight( bool b );

            ///////////////////////////////////////////////////////////////////////////
            // Returns true if the attribute specified by the template parameter is in
            // this collection.
            // 
            template < class T >
            bool GetOverride() const
            {
                const Content::Light* pkg = GetPackage< Content::Light >();
                return ( pkg->GetAttribute( Reflect::GetType< T >() ).ReferencesObject() );
            }

            ///////////////////////////////////////////////////////////////////////////
            // Adds or removes the specified attribute from this collection.
            // 
            template < class T >
            void SetOverride( bool enable )
            {
                Content::Light* pkg = GetPackage< Content::Light >();
                if ( enable )
                {
                    // This will create a new attribute or enable an existing one.
                    Component::ComponentEditor< T > editor( pkg );
                    editor.Commit();
                }
                else
                {
                    pkg->RemoveAttribute( Reflect::GetType< T >() );
                }
            }

            //
            // Events
            //
        protected:
            LightChangeSignature::Event m_Changed;
        public:
            void AddChangedListener( const LightChangeSignature::Delegate& listener )
            {
                m_Changed.Add( listener );
            }

            void RemoveChangedListener( const LightChangeSignature::Delegate& listener )
            {
                m_Changed.Remove( listener );
            }

        protected:
            RealtimeLightExistenceSignature::Event m_RealtimeExistence;
        public:
            void AddRealtimeLightExistenceListener( const RealtimeLightExistenceSignature::Delegate& listener )
            {
                m_RealtimeExistence.Add( listener );
            }

            void RemoveRealtimeLightExistenceListener( const RealtimeLightExistenceSignature::Delegate& listener )
            {
                m_RealtimeExistence.Remove( listener );
            }
        };
        typedef Helium::SmartPtr<Core::Light> LightPtr;
        typedef std::vector<Core::Light*> V_LightDumbPtr;
    }
}