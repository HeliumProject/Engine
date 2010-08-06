#pragma once

#include "Foundation/Reflect/Element.h"
#include "Pipeline/Content/Nodes/Camera.h"
#include "Editor/Settings.h"

namespace Helium
{
    namespace Editor
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

            static void CameraModeEnumerateEnumeration( Reflect::Enumeration* info )
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

            static void EnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement(ShadingModes::Wireframe, TXT( "Wireframe" ) );
                info->AddElement(ShadingModes::Material, TXT( "Material" ) );
                info->AddElement(ShadingModes::Texture, TXT( "Texture" ) );
            }
        }

        typedef ShadingModes::ShadingMode ShadingMode;

        class CameraPreferences : public Reflect::ConcreteInheritor< CameraPreferences, Settings >
        {
        public: 
            CameraPreferences(); 

        public: 
            CameraMode  m_CameraMode; // we save what mode we correspond to
            ShadingMode m_ShadingMode; 

            bool m_WireframeOnMesh; 
            bool m_WireframeOnShaded;

            bool m_ViewFrustumCulling; 
            bool m_BackFaceCulling; 

        public:
            static void EnumerateClass( Reflect::Compositor<CameraPreferences>& comp )
            {
                Reflect::EnumerationField* enumCameraMode = comp.AddEnumerationField( &CameraPreferences::m_CameraMode, "m_CameraMode" );
                Reflect::EnumerationField* enumShadingMode = comp.AddEnumerationField( &CameraPreferences::m_ShadingMode, "m_ShadingMode" );
                Reflect::Field* fieldWireframeOnMesh = comp.AddField( &CameraPreferences::m_WireframeOnMesh, "m_WireframeOnMesh" );
                Reflect::Field* fieldWireframeOnShaded = comp.AddField( &CameraPreferences::m_WireframeOnShaded, "m_WireframeOnShaded" );
                Reflect::Field* fieldViewFrustumCulling = comp.AddField( &CameraPreferences::m_ViewFrustumCulling, "m_ViewFrustumCulling" );
                Reflect::Field* fieldBackFaceCulling = comp.AddField( &CameraPreferences::m_BackFaceCulling, "m_BackFaceCulling" );
            }
        };

        typedef Helium::SmartPtr<CameraPreferences> CameraPreferencesPtr; 
        typedef std::vector< CameraPreferencesPtr> V_CameraPreferences; 
    }
}