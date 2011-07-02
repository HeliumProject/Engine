//----------------------------------------------------------------------------------------------------------------------
// StaticMeshEntity.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_STATIC_MESH_ENTITY_H
#define HELIUM_FRAMEWORK_STATIC_MESH_ENTITY_H

#include "Framework/MeshEntity.h"

namespace Helium
{
    /// In-game entity comprising of a static (non-skinned) mesh, with optional collision data.
    class HELIUM_FRAMEWORK_API StaticMeshEntity : public MeshEntity
    {
        HELIUM_DECLARE_OBJECT( StaticMeshEntity, MeshEntity );

    public:
        /// @name Construction/Destruction
        //@{
        StaticMeshEntity();
        virtual ~StaticMeshEntity();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_STATIC_MESH_ENTITY_H
