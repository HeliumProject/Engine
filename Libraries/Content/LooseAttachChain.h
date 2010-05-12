#pragma once

#include "Reflect/Element.h"
#include "JointTransform.h" 
#include "API.h"

#include <list>

namespace Content
{
  class Scene; 
  typedef std::list<JointTransformPtr> L_JointTransform; 

  // predeclarations for our type and related types
  // 
  class  CONTENT_API LooseAttachChain; 
  typedef Nocturnal::SmartPtr<LooseAttachChain> LooseAttachChainPtr; 
  typedef std::vector<LooseAttachChainPtr> V_LooseAttachChain; 

  //------------------------------------------------------------
  // LooseAttachChain
  //

  class CONTENT_API LooseAttachChain : public Reflect::Element
  {
  public: 

    // ChainType, very simple
    // 
    enum ChainType
    {
      SingleEnded, 
      DoubleEnded
    }; 

    static void ChainTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(SingleEnded, "SingleEnded");
      info->AddElement(DoubleEnded, "DoubleEnded"); 
    }

    // SingleEnded chains have only a start joint
    //
    // DoubleEnded have a start and end joint, as well 
    // as a middle joint which determines where the 
    // sim changes from forwards to backwards
    // 
    enum JointIndex
    {
      JointStart, 
      JointMiddle, 
      JointEnd, 
      JointCount
    }; 

    static void JointIndexEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(JointStart, "JointStart"); 
      info->AddElement(JointMiddle, "JointMiddle"); 
      info->AddElement(JointEnd, "JointEnd"); 
      info->AddElement(JointCount, "JointCount"); 
    }


    LooseAttachChain(); 
    ~LooseAttachChain(); 

    //! Resolve the stored GUIDs to actual JointTransform pointers
    //!
    //! Should be called after load. Is not in PostSerialize because
    //! we don't know anything about the 
    //!
    bool Resolve(Content::Scene& scene); 

    // i want this to be accessible both to the 
    // construction tool and to the builders
    // 
    bool Validate(Content::Scene& scene, V_LooseAttachChain& others, std::string& message); 


    // getter and setter, inlined
    // 
    void SetJoint(JointIndex which, const JointTransformPtr& joint)
    {
      m_Joints[which]    = joint; 

      switch( which )
      {
      case JointStart:
        m_StartJoint = (joint ? joint->m_ID : UniqueID::TUID::Null);
        break;

      case JointMiddle:
        m_MiddleJoint = (joint ? joint->m_ID : UniqueID::TUID::Null);
        break;

      case JointEnd:
        m_EndJoint = (joint ? joint->m_ID : UniqueID::TUID::Null);
        break;
      }
    }

    const JointTransformPtr& GetJoint(JointIndex which)
    {
      return m_Joints[which]; 
    }

    void SetChainType(ChainType type)
    {
      m_ChainType = type; 
    }

    ChainType GetChainType() const
    {
      return m_ChainType; 
    }

    bool ContainsJoint(const Content::JointTransformPtr& joint); 

    const L_JointTransform& GetJointsInChain() const
    {
      return m_JointsInChain; 
    }

  public: 
    ///---------------------------------------------------
    // serialized data
    // 
    std::string       m_Name; 
    ChainType         m_ChainType; 
    UniqueID::TUID    m_StartJoint; 
    UniqueID::TUID    m_MiddleJoint; 
    UniqueID::TUID    m_EndJoint; 

  private: 
    //----------------------------------------------------
    // unserialized data that is only valid after we call 
    // Resolve with a content scene
    // 
    JointTransformPtr m_Joints[JointCount]; 
    L_JointTransform  m_JointsInChain;
    bool              m_Resolved; 

    // internal validation routines
    // 
    bool ValidateSelf(Content::Scene& scene, std::string& message, L_JointTransform& chain);
    bool ValidateSingleEnded(Content::Scene& scene, std::string& message, L_JointTransform& chain); 
    bool ValidateDoubleEnded(Content::Scene& scene, std::string& message, L_JointTransform& chain); 
    bool ValidateNoForks(Content::Scene& scene, L_JointTransform::iterator start, L_JointTransform::iterator end); 

    bool MakeChain(Content::Scene& scene, JointTransformPtr start, JointTransformPtr end, L_JointTransform& chain); 
    bool MakeChain(Content::Scene& scene, JointTransformPtr start, L_JointTransform& chain); 

    REFLECT_DECLARE_CLASS(LooseAttachChain, Reflect::Element); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachChain>& comp );
    virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName);
  };
}