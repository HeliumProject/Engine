//----------------------------------------------------------------------------------------------------------------------
// RShader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RShader.h"

using namespace Lunar;

/// Destructor.
RShader::~RShader()
{
}

/// @fn RShader::EType RShader::GetType() const
/// Get the type of this shader.
///
/// @return  Shader type.

/// @fn void* RShader::Lock()
/// Lock this shader for loading.
///
/// On some platforms, this may be a pointer to memory to which the CPU does not have fast read access, so data
/// should only be written to the returned memory buffer.  When loading is complete, Unlock() should be called to
/// commit the data and finalize the initialization process so that the shader can be used.
///
/// Additionally, due to the underlying implementation, some platforms may not allow locking a shader after it has
/// already been loaded (either during resource creation or through a previous Lock()/Unlock() combination).
/// Consecutive calls to lock the shader again may in turn fail.
///
/// @return  Pointer to the base address in which to copy the shader data if the shader could be locked
///          successfully, null if allocation failed.
///
/// @see Unlock()

/// @fn void RShader::Unlock()
/// Unlock this shader to finalize loading.
///
/// This should only be called with a corresponding Lock() call made previously to begin the load process.
///
/// @return  True if the shader was successfully initialized with the loaded data, false if an error occurred.
///
/// @see Lock()
