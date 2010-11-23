#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/SerializerDeduction.h"

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
                info.AddElement(CameraMode::Orbit,  TXT( "CameraMode::Orbit" ), TXT( "Orbit" ) ); 
                info.AddElement(CameraMode::Front,  TXT( "CameraMode::Front" ), TXT( "Front" ) ); 
                info.AddElement(CameraMode::Side,   TXT( "CameraMode::Side" ), TXT( "Side" ) ); 
                info.AddElement(CameraMode::Top,    TXT( "CameraMode::Top" ), TXT( "Top" ) ); 
                info.AddElement(CameraMode::Count,  TXT( "CameraMode::Count" ) ); 
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

        class PIPELINE_API CameraSettings : public Reflect::Element
        {
        public:
            REFLECT_DECLARE_CLASS( CameraSettings, Reflect::Element );

            CameraSettings(); 

        public: 
            CameraMode  m_CameraMode; // we save what mode we correspond to
            ShadingMode m_ShadingMode; 

            bool m_WireframeOnMesh; 
            bool m_WireframeOnShaded;

            bool m_ViewFrustumCulling; 
            bool m_BackFaceCulling; 

        public:
            static void EnumerateClass( Reflect::Compositor<CameraSettings>& comp )
            {
                Reflect::EnumerationField* enumCameraMode = comp.AddEnumerationField( &CameraSettings::m_CameraMode, "m_CameraMode" );
                Reflect::EnumerationField* enumShadingMode = comp.AddEnumerationField( &CameraSettings::m_ShadingMode, "m_ShadingMode" );
                Reflect::Field* fieldWireframeOnMesh = comp.AddField( &CameraSettings::m_WireframeOnMesh, "m_WireframeOnMesh" );
                Reflect::Field* fieldWireframeOnShaded = comp.AddField( &CameraSettings::m_WireframeOnShaded, "m_WireframeOnShaded" );
                Reflect::Field* fieldViewFrustumCulling = comp.AddField( &CameraSettings::m_ViewFrustumCulling, "m_ViewFrustumCulling" );
                Reflect::Field* fieldBackFaceCulling = comp.AddField( &CameraSettings::m_BackFaceCulling, "m_BackFaceCulling" );
            }
        };

        typedef Helium::StrongPtr<CameraSettings> CameraSettingsPtr; 
        typedef std::vector< CameraSettingsPtr> V_CameraSettings; 
    }
}