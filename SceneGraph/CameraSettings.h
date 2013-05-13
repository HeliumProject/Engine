#pragma once

#include "SceneGraph/API.h"

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CameraMode
        {
        public:
            enum Enum
            {
                Orbit,
                Front,
                Side,
                Top,

                Count
            };

            REFLECT_DECLARE_ENUMERATION(CameraMode);

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(CameraMode::Orbit,  TXT( "Orbit" ) ); 
                info.AddElement(CameraMode::Front,  TXT( "Front" ) ); 
                info.AddElement(CameraMode::Side,   TXT( "Side" ) ); 
                info.AddElement(CameraMode::Top,    TXT( "Top" ) ); 
            }
        };

        class ShadingMode
        {
        public:
            enum Enum
            {
                Wireframe,
                Material,
                Texture,
            };

            REFLECT_DECLARE_ENUMERATION(ShadingMode);

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(ShadingMode::Wireframe, TXT( "Wireframe" ) );
                info.AddElement(ShadingMode::Material,  TXT( "Material" ) );
                info.AddElement(ShadingMode::Texture,   TXT( "Texture" ) );
            }
        };

        class HELIUM_SCENE_GRAPH_API CameraSettings : public Reflect::Object
        {
        public:
            REFLECT_DECLARE_OBJECT( CameraSettings, Reflect::Object );

            CameraSettings(); 

        public: 
            CameraMode  m_CameraMode; // we save what mode we correspond to
            ShadingMode m_ShadingMode; 

            bool m_WireframeOnMesh; 
            bool m_WireframeOnShaded;

            bool m_ViewFrustumCulling; 
            bool m_BackFaceCulling; 

        public:
            static void PopulateStructure( Reflect::Structure& comp )
            {
                comp.AddField( &CameraSettings::m_CameraMode, TXT( "Camera Mode" ) );
                comp.AddField( &CameraSettings::m_ShadingMode, TXT( "Shading Mode" ) );
                comp.AddField( &CameraSettings::m_WireframeOnMesh, TXT( "Wireframe On Mesh" ) );
                comp.AddField( &CameraSettings::m_WireframeOnShaded, TXT( "Wireframe On Shaded" ) );
                comp.AddField( &CameraSettings::m_ViewFrustumCulling, TXT( "View Frustum Culling" ) );
                comp.AddField( &CameraSettings::m_BackFaceCulling, TXT( "Backface Culling" ) );
            }
        };

        typedef Helium::StrongPtr<CameraSettings> CameraSettingsPtr; 
        typedef std::vector< CameraSettingsPtr> V_CameraSettings; 
    }
}