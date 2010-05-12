#include "LooseAttachChain.h" 
#include "LooseAttachStartAttribute.h"
#include "Scene.h"

#include <algorithm>

namespace Content
{
  REFLECT_DEFINE_CLASS( LooseAttachChain );

  void LooseAttachChain::EnumerateClass( Reflect::Compositor<LooseAttachChain>& comp )
  {
    Reflect::Field* fieldName = comp.AddField( &LooseAttachChain::m_Name, "m_Name" );
    Reflect::EnumerationField* enumChainType = comp.AddEnumerationField( &LooseAttachChain::m_ChainType, "m_ChainType" );
    Reflect::Field* fieldJointUIDsJointStart = comp.AddField( &LooseAttachChain::m_StartJoint, "m_StartJoint" );
    Reflect::Field* fieldJointUIDsJointMiddle = comp.AddField( &LooseAttachChain::m_MiddleJoint, "m_MiddleJoint" );
    Reflect::Field* fieldJointUIDsJointEnd = comp.AddField( &LooseAttachChain::m_EndJoint, "m_EndJoint" );
  }

  bool LooseAttachChain::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
  {
    if ( fieldName == "m_JointUIDs[JointStart]" )
    {
      Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>(element), m_StartJoint );
      return true;
    }
    else if ( fieldName == "m_JointUIDs[JointMiddle]" )
    {
      Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>(element), m_MiddleJoint );
      return true;
    }
    else if ( fieldName == "m_JointUIDs[JointEnd]" )
    {
      Reflect::Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>(element), m_EndJoint );
      return true;
    }

    return __super::ProcessComponent(element, fieldName);
  }

  LooseAttachChain::LooseAttachChain() 
    : m_ChainType(SingleEnded)
    , m_Resolved(false)
  {

  }

  LooseAttachChain::~LooseAttachChain()
  {

  }

  bool LooseAttachChain::Resolve(Content::Scene& scene)
  {
    bool ok = true; 
    m_JointsInChain.clear(); 

    //start
    if(m_StartJoint != UniqueID::TUID::Null)
    {
      m_Joints[JointStart] = scene.Get<JointTransform>( m_StartJoint ); 
      if(m_Joints[JointStart] == NULL)
      {
        Console::Warning("Loose Attach Chain %s can't resolve, missing start joint\n", m_Name.c_str()); 

        // keep going, but mark that we have failed...
        ok = false; 
      }
    }

    //middle
    if(m_MiddleJoint != UniqueID::TUID::Null)
    {
      m_Joints[JointMiddle] = scene.Get<JointTransform>( m_MiddleJoint ); 
      if(m_Joints[JointMiddle] == NULL)
      {
        Console::Warning("Loose Attach Chain %s can't resolve, missing middle joint\n", m_Name.c_str()); 

        // keep going, but mark that we have failed...
        ok = false; 
      }
    }

    //end
    if(m_EndJoint != UniqueID::TUID::Null)
    {
      m_Joints[JointEnd] = scene.Get<JointTransform>( m_EndJoint ); 
      if(m_Joints[JointEnd] == NULL)
      {
        Console::Warning("Loose Attach Chain %s can't resolve, missing end joint\n", m_Name.c_str()); 

        // keep going, but mark that we have failed...
        ok = false; 
      }
    }

    if(ok)
    {
      switch(m_ChainType)
      {
      case SingleEnded:
        ok &= MakeChain(scene, m_Joints[JointStart], m_JointsInChain); 
        break; 
      case DoubleEnded: 
        ok &= MakeChain(scene, m_Joints[JointStart], m_Joints[JointEnd], m_JointsInChain); 
        break; 
      }; 

    }

    m_Resolved = ok; 
    
    // we should consider throwing an exception... 
    // and or look at the caller to see how to deal with this. 
    return ok; 
  }

  bool LooseAttachChain::Validate(Content::Scene& scene, V_LooseAttachChain& others, std::string& message)
  {
    L_JointTransform chain; 
    if(!ValidateSelf(scene, message, chain))
    {
      return false; 
    }

   
    for(size_t i = 0; i < others.size(); ++i)
    {
      const LooseAttachChainPtr& otherChain = others[i]; 

      // skip testing against ourselves, which might happen in the builder
      // 
      if(otherChain.Ptr() == this)
        continue; 

      if(otherChain->m_Joints[JointStart] == m_Joints[JointStart])
      {
        message = "Start joint \"" + m_Joints[JointStart]->GetName() + "\" is the same as chain \"" + otherChain->m_Name + "\""; 
        return false; 
      }

      // go through all the joints in our chain
      // 
      bool keepTesting = true; 
      for(L_JointTransform::iterator it = chain.begin(); it != chain.end() && keepTesting; ++it)
      {
        const JointTransformPtr& ourJoint = *it; 

        if(otherChain->ContainsJoint( ourJoint ))
        {
          // if they contain one of our joints, that's POSSIBLY okay. 
          // 
          // it is okay if they are a double ended chain and their end joint 
          // is our start joint

          if(otherChain->m_ChainType == DoubleEnded &&
             ourJoint == m_Joints[JointStart] &&
             ourJoint == otherChain->m_Joints[JointEnd])
          {
            continue; 

          }

          // 
          // it is okay if we are a double ended chain and our end joint
          // is their start joint
          //

          if(m_ChainType == DoubleEnded && 
             ourJoint == m_Joints[JointEnd] && 
             ourJoint == otherChain->m_Joints[JointStart])
          {
            continue; 
          }

          // it is okay if they are a single ended chain, and they have 
          // our start joint somewhere in the middle of their chain
          // 
          if(otherChain->m_ChainType == SingleEnded && 
             ourJoint == m_Joints[JointStart])
          {  
            // stop looking at joints on their chain, we know we overlap
            keepTesting = false; 
            continue; 
          }

          // it is okay if we are a single ended chain, and we have 
          // their start joint at the end of our chain (and it is not
          // our start joint.) 
          // 
          // basically the opposite direction of the test right above. 
          if(m_ChainType == SingleEnded && 
             ourJoint == otherChain->m_Joints[JointStart] &&
             ourJoint != m_Joints[JointStart])
          {
            keepTesting = false; 
            continue; 
          }

          // any other case joint is not okay 
          // 

          message = "Joint \"" + ourJoint->GetName() + "\" overlaps with the chain \"" + otherChain->m_Name + "\""; 
          return false; 
        }
      }
    }

    return true; 
  }

  bool LooseAttachChain::ValidateSelf(Content::Scene& scene, std::string& message, L_JointTransform& chain)
  {
    if(m_Name == "")
    {
      message = "Name field is empty."; 
      return false; 
    }

    switch(m_ChainType)
    {
    case SingleEnded:
      return ValidateSingleEnded(scene, message, chain); 
      break; 
    case DoubleEnded:
      return ValidateDoubleEnded(scene, message, chain); 
      break; 
    }

    NOC_ASSERT(false); 
    return false;  

  }

  bool LooseAttachChain::ValidateSingleEnded(Content::Scene& scene, std::string& message, L_JointTransform& chain)
  {
    if(m_StartJoint == UniqueID::TUID::Null)
    {
      message = "Start Joint is empty"; 
      return false; 
    }

    if(!MakeChain(scene, m_Joints[JointStart], chain))
    {
      message = "One of the joints on this chain has multiple children, which is not supported"; 
      return false; 
    }

    return true; 
  }

  bool LooseAttachChain::ValidateDoubleEnded(Content::Scene& scene, std::string& message, L_JointTransform& chain)
  {
    if(m_StartJoint == UniqueID::TUID::Null)
    {
      message = "Start Joint is empty"; 
      return false; 
    }
    if(m_MiddleJoint == UniqueID::TUID::Null)
    {
      message = "Middle Joint is empty"; 
      return false; 
    }
    if(m_EndJoint == UniqueID::TUID::Null)
    {
      message = "Middle Joint is empty"; 
      return false; 
    }

    if(m_StartJoint == m_EndJoint)
    {
      message = "Start and End joints can't be the same joint"; 
      return false; 
    }

    if(!MakeChain(scene, m_Joints[JointStart], m_Joints[JointEnd], chain))
    {
      message = "Start joint is not a parent of the end joint"; 
      return false; 
    }

    // since we are a double ended chain, that means that we can support 
    // the last joint having multiple children (the last joint is fixed in space, 
    // so we don't get into trouble with the sim. 
    // 
    if(!ValidateNoForks(scene, chain.begin(), --chain.end()))
    {
      message = "One of the joints on this chain has multiple children, which is not supported";  
      return false; 
    }

    // make sure that the middle joint appears between start and end joints. 
    // 
    L_JointTransform::iterator middle = std::find(chain.begin(), chain.end(), m_Joints[JointMiddle]); 
    if(middle == chain.end())
    {
      message = "Middle joint is not on that chain"; 
      return false; 
    }

    if(chain.size() < 3)
    {
      message = "Double ended chain must have at least 3 joints"; 
      return false; 
    }

    return true; 
  }

  bool LooseAttachChain::ValidateNoForks(Content::Scene& scene, L_JointTransform::iterator start, L_JointTransform::iterator end)
  {
    JointTransformPtr jointChild = NULL; 
    L_JointTransform::iterator it; 
    for(it = start; it != end; ++it)
    {
      // this checks that the joint child that we selected in 
      // the previous iteration of this loop is indeed the 
      // same as the next joint in the chain. 
      // 
      NOC_ASSERT( *it && jointChild ? *it == jointChild : 1); 

      V_HierarchyNode children; 
      scene.GetChildren(children, *it); 

      // have to reset jointChild here because of the checks in NOC_ASSERT
      //
      jointChild = NULL;       

      for(size_t i = 0; i < children.size(); ++i)
      {
        JointTransform* child = Reflect::ObjectCast<JointTransform>(children[i]); 

        if(child && jointChild)
        {
          return false; 
        }
        else if(child)
        {
          jointChild = child; 
        }
      }
    }

    return true; 
  }


  //! Create a list of Joints which form a double-ended chain.
  //! 
  //! This function will return false if the chain cannot be created because start is 
  //! not a parent of end. 
  //!
  //! The algorithm used starts from the end joint, and traverses up the hierarchy.
  //! We use a list instead of a vector so that we can push onto the front easily. 
  //! 
  bool LooseAttachChain::MakeChain(Content::Scene& scene, JointTransformPtr start, JointTransformPtr end, L_JointTransform& chain)
  {
    // first, push the end joint onto the chain
    chain.push_front(end); 

    // now test if we're all the way up the chain.
    if(start == end)
    {
      return true; 
    }

    // get the parent... 
    JointTransformPtr parent = scene.GetParent<JointTransform>(end); 

    if(!parent)
    {
      // if we got to a null parent, that means that we didn't see the 
      // start joint, which is an error. 
      return false; 
    }
    else
    {
      return MakeChain(scene, start, parent, chain); 
    }
  }

  //! Create a list of Joints which form a single-ended chain
  //! 
  //! This function will return false if there are more than one joint children in 
  //! for any node in the chain. This is different than the return test for 
  //! the other MakeChain function. I guess that is sad. 
  //! 
  bool LooseAttachChain::MakeChain(Content::Scene& scene, JointTransformPtr start, L_JointTransform& chain)
  {
    if(!start)
    {
      return true; 
    }

    chain.push_back(start); 

    // okay. basically, if we are building a chain, and come upon another
    // "LooseAttachStartAttribute" somewhere along the chain, then we are done
    // 
    if(chain.size() > 1 && start->GetAttribute<Content::LooseAttachStartAttribute>())
    {
      return true; 
    }

    V_HierarchyNode children; 
    scene.GetChildren(children, start); 

    // we may only have one joint child per joint
    // 
    JointTransformPtr jointChild = NULL; 
    for(size_t i = 0; i < children.size(); ++i)
    {
      JointTransformPtr child = Reflect::ObjectCast<JointTransform>(children[i]); 
      if(child && jointChild)
      {
        return false; 
      }
      else if(child)
      {
        jointChild = child; 
      }
    }

    return MakeChain(scene, jointChild, chain); 
  }


  bool LooseAttachChain::ContainsJoint(const Content::JointTransformPtr& joint)
  {
    NOC_ASSERT(m_Resolved); 

    L_JointTransform::iterator found = std::find(m_JointsInChain.begin(), m_JointsInChain.end(), joint); 

    return (found != m_JointsInChain.end()); 

  }
}
