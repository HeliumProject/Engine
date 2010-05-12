#include "Scene.h"

#include "Animation.h"
#include "AnimationClip.h"

namespace Content
{
  u32 Scene::GetNumAnimationClips() const
  {
    return (u32)(m_AnimationClips.size());
  }

  u32 Scene::GetNumValidJointAnimations( u32 clipIndex ) const
  {
    return GetNumValidJointAnimations( m_JointIds, clipIndex );
  }

  u32 Scene::GetNumValidJointAnimations( const UniqueID::S_TUID& jointList, u32 clipIndex ) const
  {
    u32 validJointAnimations = 0;

    for each ( UniqueID::TUID jointId in jointList )
    {
      if ( m_AnimationClips[ clipIndex ]->m_JointAnimationMap.find( jointId ) != m_AnimationClips[ clipIndex ]->m_JointAnimationMap.end() )
        ++validJointAnimations;
    }

    return validJointAnimations;
  }

  void Scene::GetJointMismatchReport( V_string &mismatchMessages, u32 clipIndex ) const
  {
    std::set< UniqueID::TUID > animatedJoints;

    M_Animation::iterator itr = m_AnimationClips[ clipIndex ]->m_JointAnimationMap.begin();
    M_Animation::iterator end = m_AnimationClips[ clipIndex ]->m_JointAnimationMap.end();
    
    std::string jointIdAsString;
    for( ; itr != end; ++itr )
    {
      if ( m_JointIds.find( (*itr).first ) == m_JointIds.end() )
      {
        (*itr).first.ToString( jointIdAsString );
        mismatchMessages.push_back( std::string( "joint id [" ) + jointIdAsString + "] not found in scene!" );
        continue;
      }

      if ( (*itr).second->WindowSamples() == 0 )
      {
       (*itr).first.ToString( jointIdAsString );
        mismatchMessages.push_back( std::string( "joint id [" ) + jointIdAsString + "] has no samples!" );
      }

      animatedJoints.insert( (*itr).first );
    }

    for each( UniqueID::TUID jointId in m_JointIds )
    {
      if ( animatedJoints.find( jointId ) == animatedJoints.end() )
      {
        jointId.ToString( jointIdAsString );
        mismatchMessages.push_back( std::string( "joint id [" ) + jointIdAsString + "] was not animated!" );
      }
    }
  }
}