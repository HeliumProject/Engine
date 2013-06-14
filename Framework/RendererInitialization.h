#pragma once

#include "Framework/Framework.h"

namespace Helium
{
	class Renderer;

	/// Interface for Renderer creation during System initialization.
	class HELIUM_FRAMEWORK_API RendererInitialization
	{
	public:
		/// @name Construction/Destruction
		//@{
		virtual ~RendererInitialization();
		//@}

		/// @name Renderer Initialization
		//@{
		virtual bool Initialize() = 0;
		//@}

		virtual void Shutdown() = 0;
	};
}
