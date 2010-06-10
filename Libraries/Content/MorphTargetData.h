#pragma once

#include "API.h"

#include "Mesh.h"
#include "Common/Memory/SmartPtr.h"

namespace Content
{  
  static const u32 s_InvalidU32Id = 0xffffffff;

  /////////////////////////////////////////////////////////////////////////////
  // Morph Target Data
  // 
  typedef std::map< std::string, u32 > M_TargetSetNameToId; // map target-set-name to target-set-id
  typedef std::map< u32, V_u32 > M_TargetSetIdToTargetIndex;   // map target-set-id to target-ids

  class CONTENT_API MorphTargetData : public Nocturnal::RefCountBase<MorphTargetData>
  {
  public:
    V_MorphTargetPtr            m_Targets;

    M_TargetSetNameToId         m_TargetSetNameToId;
    M_TargetSetIdToTargetIndex  m_TargetSetIdToTargetIndiciesMap;
    M_u32                       m_TargetIndexToTargetSetId;

    // mapping of each target's TUID to its location in the list of targets
    Nocturnal::UID::HM_TUIDU32        m_TargetIdToTargetIndex;


    MorphTargetData()
    {
    }

    void CollateMorphTargets( const V_Mesh& meshes );
    u32 GetMorphTargetIndex( const Nocturnal::UID::TUID& targetId ) const;
    u32 FindSetIDFromTargetIndex( const u32 targetIndex ) const;
    //void CopyMorphData( const MorphTargetData& source );

  };
  typedef Nocturnal::SmartPtr< MorphTargetData > MorphTargetDataPtr;

}