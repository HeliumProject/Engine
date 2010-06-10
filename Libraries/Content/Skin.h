#pragma once

#include <cstring>
#include <vector>

#include "Influence.h"
#include "SceneNode.h"

namespace Content
{
    //
    // Skinning influences
    //

    class CONTENT_API Skin : public SceneNode
    {
    public:
        // The mesh we are skinning
        Nocturnal::UID::TUID m_Mesh;

        // The influence data for each vertex of the mesh
        V_Influence m_Influences;

        // The influence objects required to deform this mesh, indexed by each influence item above
        Nocturnal::UID::V_TUID m_InfluenceObjectIDs;

        // The influence to use for each vertex (CORRESPONDS TO m_Positions Indices in the referenced mesh)
        V_u32 m_InfluenceIndices;

        Skin ()
        {

        }

        Skin (const Nocturnal::UID::TUID& id)
            : SceneNode (id)
        {

        }

        REFLECT_DECLARE_CLASS(Skin, SceneNode);

        static void EnumerateClass( Reflect::Compositor<Skin>& comp );
    };

    typedef Nocturnal::SmartPtr<Skin> SkinPtr;
    typedef std::vector<SkinPtr> V_Skin;
}