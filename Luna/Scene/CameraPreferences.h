#pragma once

#include "Foundation/Reflect/Element.h"
#include "Pipeline/Content/Nodes/Camera.h"
#include "Luna/Scene/Camera.h" 
#include "Luna/Scene/Viewport.h"
#include "Luna/Settings.h"

namespace Luna
{
    class CameraPreferences : public Reflect::ConcreteInheritor< CameraPreferences, Settings >
    {
    public: 
        CameraPreferences(); 

        void ApplyToCamera(Luna::Camera* camera); 
        void LoadFromCamera(Luna::Camera* camera); 

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

    typedef Nocturnal::SmartPtr<CameraPreferences> CameraPreferencesPtr; 
    typedef std::vector< CameraPreferencesPtr> V_CameraPreferences; 
}
