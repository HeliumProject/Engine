#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Core/API.h"
#include "Core/Scene/Pick.h"
#include "Core/Scene/SceneNodeType.h"

namespace Helium
{
    namespace Core
    {
        class PickHit;
        typedef Helium::SmartPtr< PickHit > PickHitPtr;
        typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

        class CORE_API HierarchyNodeType : public Core::SceneNodeType
        {
            // 
            // Member variables
            //

        protected:
            // members
            bool m_Visible;
            bool m_Selectable;

        public:
            // materials
            D3DMATERIAL9 m_WireMaterial;
            D3DMATERIAL9 m_SolidMaterial;


            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( HierarchyNodeType, Core::SceneNodeType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            HierarchyNodeType( Core::Scene* scene, i32 instanceType );

            virtual void Create();
            virtual void Delete();

            bool IsVisible() const;
            void SetVisible(bool value);

            bool IsSelectable() const;
            void SetSelectable( bool value );

            virtual bool IsTransparent();
        };

        typedef std::map< tstring, HierarchyNodeType* > M_HierarchyNodeTypeDumbPtr;
    }
}