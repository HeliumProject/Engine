//----------------------------------------------------------------------------------------------------------------------
// StaticMeshEntity.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_STATIC_MESH_ENTITY_H
#define LUNAR_FRAMEWORK_STATIC_MESH_ENTITY_H

#include "Framework/MeshEntity.h"

namespace Lunar
{
    /// In-game entity comprising of a static (non-skinned) mesh, with optional collision data.
    class LUNAR_FRAMEWORK_API StaticMeshEntity : public MeshEntity
    {
        L_DECLARE_OBJECT( StaticMeshEntity, MeshEntity );

    public:
        /// @name Construction/Destruction
        //@{
        StaticMeshEntity();
        virtual ~StaticMeshEntity();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_STATIC_MESH_ENTITY_H
