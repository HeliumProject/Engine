#pragma once

#if !HELIUM_RELEASE && !HELIUM_PROFILE

#include "GLFW/glfw3.h"

namespace Helium
{
	namespace GLSeverities
	{
		enum Type
		{
			Notification = 0,
			Low,
			Medium,
			High,
			None
		};
	}
	typedef GLSeverities::Type GLSeverity;

	/// @name OpenGL Callback Interface
	//@{
	void GLDebugRegister( GLFWwindow* context, GLSeverity minimumSeverity );
	//@}
}

#endif // !HELIUM_RELEASE && !HELIUM_PROFILE
