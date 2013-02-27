//----------------------------------------------------------------------------------------------------------------------
// GraphicsTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "Graphics/Material.h"
#include "Graphics/Font.h"
#include "Graphics/Shader.h"
#include "Graphics/Animation.h"
#include "Graphics/Texture2d.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"

HELIUM_GRAPHICS_API void RegisterGraphicsTypes()
{
    Helium::SceneObjectTransform::RegisterComponentType(128);
}

HELIUM_GRAPHICS_API void UnregisterGraphicsTypes()
{    

}
