#pragma once

#include <cstring>
#include <vector>

#include "Influence.h"
#include "Core/Content/Nodes/SceneNode.h"

namespace Helium
{
    namespace Content
    {
        //
        // Skinning influences
        //

        class CORE_API Skin : public SceneNode
        {
        public:
            // The mesh we are skinning
            Helium::TUID m_Mesh;

            // The influence data for each vertex of the mesh
            V_Influence m_Influences;

            // The influence objects required to deform this mesh, indexed by each influence item above
            Helium::V_TUID m_InfluenceObjectIDs;

            // The influence to use for each vertex (CORRESPONDS TO m_Positions Indices in the referenced mesh)
            std::vector< u32 > m_InfluenceIndices;

            Skin ()
            {

            }

            Skin (const Helium::TUID& id)
                : SceneNode (id)
            {

            }

            REFLECT_DECLARE_CLASS(Skin, SceneNode);

            static void EnumerateClass( Reflect::Compositor<Skin>& comp );
        };

        typedef Helium::SmartPtr<Skin> SkinPtr;
        typedef std::vector<SkinPtr> V_Skin;
    }
}