#include "MorphTargetData.h"

namespace Content
{
  ///////////////////////////////////////////////////////////////////////////////
  // Determines the target set name based on the target name; group targets by sets
  // by the target name prefix '_' delimited
  // 
  //  0 leye_open   -> leye
  //  1 leye_blink
  //  2 reye_open   -> reye
  //  3 reye_blink
  static inline void DetermineTargetSetName( const std::string& targetName, std::string& targetSetName )
  {
    static const char* s_DefaultMorphTargetSetName = "@default_set";

#pragma TODO("We need a better way to group morph target sets, either to group targets under a set node in maya, or specify it in an attribute")
    //j    targetSetName = targetName;
    //j    toLower( targetSetName );
    //j
    //j    std::string::size_type delimiterPos = targetSetName.rfind( '_' );
    //j    if ( delimiterPos != std::string::npos )
    //j    {
    //j      targetSetName.erase( delimiterPos );
    //j    }
    //j    else
    {
      targetSetName = s_DefaultMorphTargetSetName;
    }
  }

  void MorphTargetData::CollateMorphTargets( const V_Mesh& meshes )
  {
    V_Mesh::const_iterator meshItr = meshes.begin();
    V_Mesh::const_iterator meshEnd = meshes.end();
    for ( ; meshItr != meshEnd; ++meshItr )
    {
      const Mesh* mesh = (*meshItr);      

      u32 numMorphTargets = (u32) mesh->m_MorphTargets.size();

      for ( u32 targetIndex = 0; targetIndex < numMorphTargets ; ++targetIndex )
      {
        const MorphTarget* oldMorphTarget = mesh->m_MorphTargets[targetIndex];
        const Nocturnal::UID::TUID& targetId = oldMorphTarget->m_Id;

        if ( m_TargetIdToTargetIndex.find( targetId ) != m_TargetIdToTargetIndex.end() )
        {
          continue;
        }

        //
        // Copy Target
        // 
        u32 newTargetIndex = (u32) m_Targets.size();
        m_Targets.push_back( new MorphTarget( oldMorphTarget->m_Name, targetId ) );

        MorphTarget* newMorphTarget = m_Targets.at(newTargetIndex);
        newMorphTarget->m_Deltas = oldMorphTarget->m_Deltas;

        // insert into look up table
        m_TargetIdToTargetIndex[targetId] = newTargetIndex;


        //
        // Add to Target Set
        //
        std::string targetSetName;
        DetermineTargetSetName( newMorphTarget->m_Name, targetSetName );

        // insert the target set name and create an id (or get the existing one)
        m_TargetSetNameToId.insert( M_TargetSetNameToId::value_type( targetSetName, (u32)m_TargetSetNameToId.size() ) );
        u32 targetSetId = m_TargetSetNameToId[targetSetName];

        // insert the target id into the target set list
        m_TargetSetIdToTargetIndiciesMap.insert( M_TargetSetIdToTargetIndex::value_type( targetSetId, V_u32() ) );
        m_TargetSetIdToTargetIndiciesMap[targetSetId].push_back( targetIndex );

        m_TargetIndexToTargetSetId.insert( M_u32::value_type( targetIndex, targetSetId ) ); 
      }
    }

    NOC_ASSERT( m_TargetIndexToTargetSetId.size() == m_Targets.size() );
  }

  u32 MorphTargetData::GetMorphTargetIndex( const Nocturnal::UID::TUID& targetId ) const 
  {
    Nocturnal::UID::HM_TUIDU32::const_iterator findTargetId = m_TargetIdToTargetIndex.find( targetId ) ;
    if ( findTargetId != m_TargetIdToTargetIndex.end() )
    {
      return findTargetId->second;  
    }

    return s_InvalidU32Id;
  }

  u32 MorphTargetData::FindSetIDFromTargetIndex( const u32 targetIndex ) const
  {
    return m_TargetIndexToTargetSetId.find( targetIndex )->second;
  }

  //void MorphTargetData::CopyMorphData( const MorphTargetData& source )
  //{
  //  //morph targets
  //  u32 newTargetIndexOffset = (u32) m_Targets.size();
  //  m_Targets.insert( m_Targets.end(), 
  //    source.m_Targets.begin(), 
  //    source.m_Targets.end() );

  //  // map target-set-name to target-set-id
  //  M_u32 newTargetSetIds;

  //  M_TargetSetNameToId::const_iterator targetSetIdsItr = source.m_TargetSetNameToId.begin();
  //  M_TargetSetNameToId::const_iterator targetSetIdsEnd = source.m_TargetSetNameToId.end();
  //  for ( ; targetSetIdsItr != targetSetIdsEnd; ++targetSetIdsItr )
  //  {
  //    // insert the target set name and create an id (or get the existing one)
  //    const std::string& sourceTargetSetName = targetSetIdsItr->first;
  //    const u32 sourceTargetSetId = targetSetIdsItr->second;
  //    m_TargetSetNameToId.insert( M_TargetSetNameToId::value_type( sourceTargetSetName, (u32)m_TargetSetNameToId.size() ) );
  //    newTargetSetIds.insert( M_u32::value_type( sourceTargetSetId, m_TargetSetNameToId[sourceTargetSetName] ) );
  //  }

  //  // map target-set-id to target-ids
  //  M_TargetSetIdToTargetIndex::const_iterator targetSetItr = source.m_TargetSetIdToTargetIndiciesMap.begin();
  //  M_TargetSetIdToTargetIndex::const_iterator targetSetEnd = source.m_TargetSetIdToTargetIndiciesMap.end();
  //  for ( ; targetSetItr != targetSetEnd; ++targetSetItr )
  //  {
  //    const u32 sourceTargetSetId = targetSetItr->first;
  //    const V_u32& sourceTargetIndicies = targetSetItr->second;
  //    
  //    const u32 numSourceTargetIndicies = (u32) sourceTargetIndicies.size();
  //    
  //    const u32 newTargetSetId = newTargetSetIds[sourceTargetSetId];
  //    Nocturnal::Insert<M_TargetSetIdToTargetIndex>::Result insert = 
  //      m_TargetSetIdToTargetIndiciesMap.insert( M_TargetSetIdToTargetIndex::value_type( newTargetSetId, V_u32() ) );

  //    V_u32& newTargetIndicies = insert.first->second;
  //    newTargetIndicies.reserve( newTargetIndicies.size() + numSourceTargetIndicies );
  //    
  //    for ( u32 sourceTargetIndex = 0; sourceTargetIndex < numSourceTargetIndicies; ++sourceTargetIndex )
  //    {
  //      u32 newTargetIndex = sourceTargetIndicies[sourceTargetIndex] + newTargetIndexOffset;
  //      newTargetIndicies.push_back( newTargetIndex );

  //      // reverse lookup list
  //      m_TargetIndexToTargetSetId.insert( M_u32::value_type( newTargetIndex, newTargetSetId ) );
  //    }
  //  }

  //  // fix-up the mapping of each target's TUID to its location in the new list of targets
  //  Nocturnal::UID::HM_TUIDU32::const_iterator sourceTargetIdItr = source.m_TargetIdToTargetIndex.begin();
  //  Nocturnal::UID::HM_TUIDU32::const_iterator sourceTargetIdEnd = source.m_TargetIdToTargetIndex.end();
  //  for ( ; sourceTargetIdItr != sourceTargetIdEnd; ++sourceTargetIdItr )
  //  {
  //    const Nocturnal::UID::TUID& sourceTargetId = sourceTargetIdItr->first;
  //    const u32 sourceTargetIndex = sourceTargetIdItr->second;
  //    const u32 newTargetIndex = sourceTargetIndex + newTargetIndexOffset;

  //    NOC_ASSERT( m_TargetIdToTargetIndex.find( sourceTargetId ) == m_TargetIdToTargetIndex.end() );
  //    m_TargetIdToTargetIndex[sourceTargetId] = newTargetIndex;
  //  }
  //}
}
