#pragma once

#include "ExampleGame/Components/Graphics/Camera.h"
#include "Graphics/GraphicsManagerComponent.h"

#define EXAMPLE_GAME_MAX_WORLDS (1)
#define EXAMPLE_GAME_MAX_CAMERAS (4)

namespace ExampleGame
{
	class CameraManagerComponentDefinition;
	
	typedef Helium::StrongPtr<CameraManagerComponentDefinition> CameraManagerComponentDefinitionPtr;
	typedef Helium::StrongPtr<const CameraManagerComponentDefinition> ConstCameraManagerComponentDefinitionPtr;
		
	//////////////////////////////////////////////////////////////////////////
	// CameraManagerComponent
	class EXAMPLE_GAME_API CameraManagerComponent : public Helium::Component
	{
		HELIUM_DECLARE_COMPONENT( ExampleGame::CameraManagerComponent, Helium::Component );
		static void PopulateStructure( Helium::Reflect::Structure& comp );
		
		void Finalize( const CameraManagerComponentDefinition *pDefinition);

		void Tick();

		bool RegisterNamedCamera(Helium::Name cameraName, CameraComponent *pCameraC);
		bool UnregisterNamedCamera(Helium::Name cameraName, CameraComponent *pCameraC);

		void SetCurrentCameraByName(Helium::Name cameraName);
		void SetCurrentCamera(CameraComponent *pCameraC);

		Helium::Map<Helium::Name, CameraComponent *> m_Cameras;
		Helium::Name m_CurrentCameraName;
		CameraComponentPtr m_CurrentCamera;
		Helium::GraphicsManagerComponentPtr m_GraphicsManager;
		bool m_CameraChanged;
	};
	
	class EXAMPLE_GAME_API CameraManagerComponentDefinition : public Helium::ComponentDefinitionHelper<CameraManagerComponent, CameraManagerComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( ExampleGame::CameraManagerComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Helium::Reflect::Structure& comp );

		CameraManagerComponentDefinition();

		Helium::Name m_DefaultCameraName;
	};

	//////////////////////////////////////////////////////////////////////////
	// CameraManagerTick
	struct EXAMPLE_GAME_API CameraManagerTick : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(CameraManagerTick)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
