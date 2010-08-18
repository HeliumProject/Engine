#pragma once

#include "Foundation/Reflect/Element.h"
#include "Core/Content/Nodes/ContentCamera.h"
#include "Core/Scene/Settings.h"

namespace Helium
{
    namespace Core
    {
        namespace CameraModes
        {
            enum CameraMode
            {
                Orbit,
                Front,
                Side,
                Top,

                Count
            };

            static void CameraModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(CameraModes::Orbit, TXT( "CameraModes::Orbit" ), TXT( "Orbit" ) ); 
                info->AddElement(CameraModes::Front, TXT( "CameraModes::Front" ), TXT( "Front" ) ); 
                info->AddElement(CameraModes::Side, TXT( "CameraModes::Side" ), TXT( "Side" ) ); 
                info->AddElement(CameraModes::Top, TXT( "CameraModes::Top" ), TXT( "Top" ) ); 
                info->AddElement(CameraModes::Count, TXT( "CameraModes::Count" ) ); 
            }
        }

        typedef CameraModes::CameraMode CameraMode;

        namespace ShadingModes
        {
            enum ShadingMode
            {
                Wireframe,
                Material,
                Texture,
            };

            static void EnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(ShadingModes::Wireframe, TXT( "Wireframe" ) );
                info->AddElement(ShadingModes::Material, TXT( "Material" ) );
                info->AddElement(ShadingModes::Texture, TXT( "Texture" ) );
            }
        }

        typedef ShadingModes::ShadingMode ShadingMode;

        class CORE_API CameraSettings : public Reflect::ConcreteInheritor< CameraSettings, Settings >
        {
        public: 
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

        typedef Helium::SmartPtr<CameraSettings> CameraSettingsPtr; 
        typedef std::vector< CameraSettingsPtr> V_CameraSettings; 
    }
}