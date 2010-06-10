#pragma once

#include "API.h"
#include "SceneNode.h"
#include "UID/TUID.h"

namespace Content
{
    class CONTENT_API Region : public SceneNode
    {
    public: 
        Region(); 

        bool HasZone( Nocturnal::UID::TUID zoneId ) const;
        void AddZone( Nocturnal::UID::TUID zoneId );
        void RemoveZone( Nocturnal::UID::TUID zoneId );

        const Reflect::Field* ZonesField() const
        {
            return GetClass()->FindField( &Region::m_ZoneIds ); 
        }

        const Nocturnal::UID::V_TUID& GetZones()
        {
            return m_ZoneIds; 
        }

    private: 
        Nocturnal::UID::V_TUID m_ZoneIds; 

    public: 
        float       m_SpatialGridSize;
        bool        m_OcclTestDownwardColl;
        bool        m_OcclTestDownwardVis;
        float       m_OcclVisDistAdjust;
        bool        m_OverrideOcclusionSettings; 


        REFLECT_DECLARE_CLASS(Region, SceneNode);
        static void EnumerateClass( Reflect::Compositor<Region>& comp );

    }; 

    typedef Nocturnal::SmartPtr<Region> RegionPtr; 
    typedef std::vector< RegionPtr > V_Region; 
}