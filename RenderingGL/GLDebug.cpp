#include "RenderingGLPch.h"
#include "RenderingGL/GLDebug.h"

#include "Platform/Trace.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#if !HELIUM_RELEASE && !HELIUM_PROFILE

using namespace Helium;

const char * GetTypeString(GLenum type);
const char * GetSourceString(GLenum source);
TraceLevel GetSeverityTraceLevel(GLenum severity);

void APIENTRY HeliumGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const TraceLevel traceLevel = GetSeverityTraceLevel( severity );
	const char *sourceString = GetSourceString( source );
	const char *typeString = GetTypeString( type );

	HELIUM_TRACE(
		traceLevel, "OpenGL Debug - Type: %s, Source: %s, ID: %u, %s\n",
		typeString, sourceString, id, message );
}

void Helium::GLDebugRegister(GLFWwindow* context, GLSeverity minimumSeverity)
{
	if( context == NULL )
	{
		HELIUM_TRACE( TraceLevel::Warning, "GLDebug: Invalid context provided to GLDebugRegister" );
		return;
	}

	// Store current context; switch to specified.
	GLFWwindow *prevContext = glfwGetCurrentContext();
	if( prevContext != context )
	{
		glfwMakeContextCurrent( context );
	}

	// Register callback function
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	glDebugMessageCallback( HeliumGLDebug, NULL );

	// Configure severity
	glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_FALSE );
	switch( minimumSeverity )
	{
	default:
		HELIUM_TRACE( TraceLevels::Warning, "GLDebug: Invalid minimumSeverity provided to Register(); enabling all GL debug levels.\n" );
	case GLSeverities::Notification:
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_TRUE );
	case GLSeverities::Low:
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, 0, GL_TRUE );
	case GLSeverities::Medium:
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, 0, GL_TRUE );
	case GLSeverities::High:
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, 0, GL_TRUE );
	case GLSeverities::None:
		break;
	}

	// Restore previous context.
	if( prevContext != context )
	{
		glfwMakeContextCurrent( prevContext );
	}
}

const char * GetTypeString(GLenum type)
{
	switch( type )
	{
	case GL_DEBUG_TYPE_ERROR:
		return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behavior";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability Problem";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance Problem";
	case GL_DEBUG_TYPE_OTHER:
		return "Other";
	default:
		return "";
	}
}

const char * GetSourceString(GLenum source)
{
	switch( source )
	{
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		return "";
	}
}

TraceLevel GetSeverityTraceLevel(GLenum severity)
{
	switch( severity )
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return TraceLevels::Debug;
	case GL_DEBUG_SEVERITY_LOW:
		return TraceLevels::Info;
	case GL_DEBUG_SEVERITY_MEDIUM:
		return TraceLevels::Warning;
	case GL_DEBUG_SEVERITY_HIGH:
	default:
		return TraceLevels::Error;
	}
}

#endif // !HELIUM_RELEASE && !HELIUM_PROFILE
