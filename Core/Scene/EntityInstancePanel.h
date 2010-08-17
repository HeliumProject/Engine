#pragma once

#include "Core/API.h"
#include "InstancePanel.h"

#include "Application/RCS/RCS.h"
#include "Application/UI/FileDialog.h"
#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

#ifdef INSPECT_REFACTOR
#include "Application/Inspect/DragDrop/FilteredDropTarget.h"
#endif

#include "EntityInstance.h"

namespace Helium
{
    namespace Core
    {
        struct EntitySetChangeArgs;

        template <class T>
        struct Enabler
        {
            std::vector<T*> m_Items;

            void SetEnabled(bool enabled)
            {
                std::vector<T*>::const_iterator itr = m_Items.begin();
                std::vector<T*>::const_iterator end = m_Items.end();
                for ( ; itr != end; ++itr )
                {
                    (*itr)->SetEnabled(enabled);
                }
            }
        };

        typedef Enabler<Inspect::Control> ControlEnabler;

        class EntityPanel : public InstancePanel
        {
        protected:
            Inspect::Value*     m_TextBox;

            Inspect::Panel*     m_LightingPanel;
            Inspect::Value*     m_DrawDistance;
            Inspect::Value*     m_UpdateDistance;
            Inspect::Choice*    m_ShaderGroup;
            Inspect::Value*     m_Attenuation;

            Inspect::CheckBox*  m_FarShadowFadeout;
            Inspect::CheckBox*  m_CastsBakedShadows;
            Inspect::CheckBox*  m_DoBakedShadowCollisions;
            Inspect::Value*     m_BakedShadowAABBExt;
            Inspect::Value*     m_BakedShadowMergeGroups;
            Inspect::CheckBox*  m_HighResShadowMap;

            Inspect::CheckBox*  m_SpecializeCheckbox;

            ControlEnabler*     m_SegmentEnabler;
            ControlEnabler*     m_ShaderGroupEnabler;

            ControlEnabler*     m_FarShadowFadeoutEnabler;
            ControlEnabler*     m_CastsBakedShadowsEnabler;
            ControlEnabler*     m_DoBakedShadowCollisionsEnabler;
            ControlEnabler*     m_BakedShadowAABBExtEnabler;
            ControlEnabler*     m_BakedShadowMergeGroupsEnabler;
            ControlEnabler*     m_HighResShadowMapEnabler;

            typedef std::vector< EntityPtr > V_EntitySmartPtr;
            V_EntitySmartPtr                m_Entities;
            Inspect::ReflectInterpreterPtr  m_CollisionReflectInterpreter;

        public:
            EntityPanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection);
            virtual ~EntityPanel();
            virtual void Create() HELIUM_OVERRIDE;

        protected:
            void CreateClassPath();
            void CreateClassActions();
            void CreateShowFlags();
            void CreateLighting();
            void CreateCubemap();
            void CreateGameplay();
            void CreateShaderGroups();
            void CreateVisual();
            void CreateCollision();
            void CreateAttenuation();

        protected:
            // UI callbacks

            bool OnEntityAssetChanging( const Inspect::ControlChangingArgs& args );
            void OnEntityAssetChanged( const Inspect::ControlChangedArgs& args );
            void OnEntityAssetRefresh( Inspect::Button* button );
            void OnEntityAssetEditAsset( Inspect::Button* button );
            void OnEntityAssetEditArt( Inspect::Button* button );
            void OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args );
            void OnCubeMapEditAsset( Inspect::Button* button );
            void OnViewUVs( Inspect::Button* button );

            // Internal Callbacks
            void EntityAssetReloaded( const EntitySetChangeArgs& args );

            // Getters/Setters

            template <class ComponentType>
            bool GetOverride() const
            {
                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *itr );
                    HELIUM_ASSERT(entity);

                    if ( !entity->GetPackage<Asset::EntityInstance>()->ContainsComponent( Reflect::GetType<ComponentType>() ) )
                    {
                        return false;
                    }
                }

                return true;
            }

            template <class ComponentType, class Control, Control EntityPanel::* MemberPtr>
            void SetOverride(bool o)
            {
                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *itr );
                    HELIUM_ASSERT(entity);

                    if ( o )
                    {
                        if ( !entity->GetPackage<Asset::EntityInstance>()->ContainsComponent( Reflect::GetType<ComponentType>() ) )
                        {
                            entity->GetPackage<Asset::EntityInstance>()->SetComponent( new ComponentType() );
                        }
                    }
                    else
                    {
                        if ( entity->GetPackage<Asset::EntityInstance>()->ContainsComponent( Reflect::GetType<ComponentType>() ) )
                        {
                            entity->GetPackage<Asset::EntityInstance>()->RemoveComponent( Reflect::GetType<ComponentType>() );
                        }
                    }
                }

                (this->*MemberPtr)->SetEnabled( o );
            }

            //
            template< class ComponentType, bool ComponentType::*PtrToBool >
            bool GetBoolOverride() const
            {
                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::AssertCast< Core::Entity >( *itr );
                    Asset::EntityInstance* pkg = entity->GetPackage< Asset::EntityInstance >();
                    Helium::SmartPtr< ComponentType > attrib = Reflect::ObjectCast< ComponentType >( pkg->GetAttribute( Reflect::GetType< ComponentType >() ) );
                    if ( !attrib.ReferencesObject() || !( attrib->*PtrToBool ) )
                    {
                        return false;
                    }
                }
                return true;
            }

            //
            template< class ComponentType, class Control, Control EntityPanel::*MemberPtr, bool ComponentType::*PtrToBool >
            void SetBoolOverride( bool o )
            {
                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::AssertCast< Core::Entity >( *itr );

                    Asset::EntityInstance* pkg = entity->GetPackage<Asset::EntityInstance>();
                    if ( !pkg->ContainsAttribute( Reflect::GetType<ComponentType>() ) )
                    {
                        pkg->SetAttribute( new ComponentType () );
                    }

                    Component::ComponentEditor< ComponentType > attr( pkg );
                    (attr.operator->())->*PtrToBool = o;
                    attr.Commit();
                }

                (this->*MemberPtr)->SetEnabled( o );
            }

            template <class ValueType, class ComponentType, ValueType ComponentType::* MemberPtr>
            tstring GetValue() const
            {
                ValueType result;
                tostringstream str;

                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *itr );
                    HELIUM_ASSERT(entity);

                    Component::ComponentViewer< ComponentType > attr (entity->GetPackage<Asset::EntityInstance>());
                    if (!attr.Valid())
                    {
                        return "";
                    }

                    if ( itr == m_Selection.Begin() )
                    {
                        result = (attr.operator->())->*MemberPtr;
                        str << (attr.operator->())->*MemberPtr;
                    }
                    else if ( (attr.operator->())->*MemberPtr != result )
                    {
                        return Inspect::MULTI_VALUE_STRING;
                    }
                }

                return str.str();
            }

            template <class ValueType, class ComponentType, ValueType ComponentType::* MemberPtr>
            void SetValue(const tstring& dist)
            {
                std::istringstream str (dist);

                ValueType value;
                str >> value;

                OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
                OS_SelectableDumbPtr::Iterator end = m_Selection.End();
                for ( ; itr != end; ++itr )
                {
                    Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *itr );
                    HELIUM_ASSERT(entity);

                    Component::ComponentEditor< ComponentType > attr (entity->GetPackage<Asset::EntityInstance>());
                    HELIUM_ASSERT(attr.Valid());

                    (attr.operator->())->*MemberPtr = value;

                    attr.Commit();
                }
            }
        };
    }
}