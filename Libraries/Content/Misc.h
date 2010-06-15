#pragma once

#include "Foundation/TUID.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/CalculateBounds.h"
#include "Content/ContentTypes.h"

namespace Content
{  
    typedef std::pair<Nocturnal::TUID, u64> t_UidId;                                                     // uid, mesh-id pair
    typedef std::pair<Math::BoundingVolumeGenerator::BSphere, u64> t_BsphereId;                       // bsphere, mesh-id pair

    typedef std::map<t_UidId, Math::V_Vector3> t_JointVertsMap;                                       // map (uid, id) to verts
    typedef std::map<Nocturnal::TUID, std::vector<t_BsphereId> > M_UIDBSphere;                           // map uid to (bsphere, id) list


    struct UidBsphere_t
    {
        Nocturnal::TUID                    m_joint_id;
        u64                                     m_mesh_id;
        Math::BoundingVolumeGenerator::BSphere  m_bsphere;
    };

    typedef std::vector<UidBsphere_t> V_UidBsphere_t;



    class UidBsphereList
    {
    public:
        void Add(const UidBsphere_t& t_)
        {
            //std::string str;
            //t_.m_id.ToString(str);

            //printf("UidBsphereList::Add - '%s' - %f %f %f %f", str.c_str(), t_.m_bsphere.m_Center.x, t_.m_bsphere.m_Center.y, t_.m_bsphere.m_Center.z, t_.m_bsphere.m_Radius);

            size_t i;
            for(i = 0; i < m_list.size(); i++)
            {
                // joint-ids dont match - go to next one
                if (m_list[i].m_joint_id != t_.m_joint_id)
                {
                    continue;
                }

                // mesh-ids dont match - go to next one
                if (m_list[i].m_mesh_id != t_.m_mesh_id)
                {
                    continue;
                }

                const Math::BoundingVolumeGenerator::BSphere& ba = m_list[i].m_bsphere;
                const Math::BoundingVolumeGenerator::BSphere& bb = t_.m_bsphere;

                Math::Vector4 sa(ba.m_Center.x, ba.m_Center.y, ba.m_Center.z, ba.m_Radius);
                Math::Vector4 sb(bb.m_Center.x, bb.m_Center.y, bb.m_Center.z, bb.m_Radius);
                f32 dsq = (sa - sb).LengthSquared();

                // ids matched and these bspheres are very close - we have this one - skip
                if (dsq < SQR(0.01f))
                {
                    break;
                }
            }

            if (i == m_list.size())
            {
                //printf(" - new - adding\n");
                m_list.push_back(t_);
            }
            //else
            //{
            //  printf(" - have it - skipping\n");
            //}
        }

        V_UidBsphere_t  m_list;
    };
};
