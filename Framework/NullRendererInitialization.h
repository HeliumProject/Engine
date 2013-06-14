#pragma once

#include "Framework/RendererInitialization.h"

namespace Helium
{
	/// Renderer initializer that creates a null renderer.
	class HELIUM_FRAMEWORK_API NullRendererInitialization : public RendererInitialization
	{
	public:
		/// @name Renderer Initialization
		//@{
		bool Initialize();
		//@}

		void Shutdown();
	};
}
