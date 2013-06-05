//----------------------------------------------------------------------------------------------------------------------
// GameSystem.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_GAME_SYSTEM_H
#define HELIUM_FRAMEWORK_GAME_SYSTEM_H

#include "Framework/System.h"

#define NO_GFX (1)

namespace Helium
{
	class AssetType;

	class CommandLineInitialization;
	class MemoryHeapPreInitialization;
	class AssetLoaderInitialization;
	class ConfigInitialization;
	class WindowManagerInitialization;
	class RendererInitialization;
	class SceneDefinition;
	class Window;

	/// Base interface for game application systems.
	class HELIUM_FRAMEWORK_API GameSystem : public System
	{
	public:
		/// @name Construction/Destruction
		//@{
		GameSystem();
		virtual ~GameSystem();
		//@}

		/// @name Initialization
		//@{
		virtual bool Initialize(
			CommandLineInitialization& rCommandLineInitialization, 
			MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
			AssetLoaderInitialization& rAssetLoaderInitialization, 
			ConfigInitialization& rConfigInitialization,
			WindowManagerInitialization& rWindowManagerInitialization,
			RendererInitialization& rRendererInitialization);
		virtual void Shutdown();
		
		void LoadScene( Helium::SceneDefinition *spSceneDefinition );
		//@}

		/// @name Application Loop
		//@{
		virtual int32_t Run();
		//@}

		/// @name Static Initialization
		//@{
		static GameSystem* CreateStaticInstance();
		//@}

		virtual void StopRunning();

	protected:
		/// AssetLoader initialization interface.
		AssetLoaderInitialization* m_pAssetLoaderInitialization;
		RendererInitialization*    m_pRendererInitialization;
		bool                       m_bStopRunning;
	};
}

#endif  // HELIUM_FRAMEWORK_GAME_SYSTEM_H
