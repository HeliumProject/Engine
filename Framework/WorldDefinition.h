//----------------------------------------------------------------------------------------------------------------------
// WorldDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WORLD_DEFINITION_H
#define HELIUM_FRAMEWORK_WORLD_DEFINITION_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"
#include "Graphics/GraphicsScene.h"

namespace Helium
{
    class Entity;
    class EntityDefinition;

    class GraphicsScene;
    typedef Helium::StrongPtr< GraphicsScene > GraphicsScenePtr;
    typedef Helium::StrongPtr< const GraphicsScene > ConstGraphicsScenePtr;

    class Slice;
    typedef Helium::StrongPtr< Slice > SlicePtr;

    class SceneDefinition;
    
    class HELIUM_FRAMEWORK_API WorldDefinition : public GameObject
    {
        HELIUM_DECLARE_OBJECT( WorldDefinition, GameObject );
    };
}

#include "Framework/WorldDefinition.inl"

#endif  // HELIUM_FRAMEWORK_WORLD_H
