#include "LooseAttachEndAttribute.h" 

namespace Content
{
  REFLECT_DEFINE_CLASS(LooseAttachEndAttribute); 

void LooseAttachEndAttribute::EnumerateClass( Reflect::Compositor<LooseAttachEndAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Loose Attach Chain End";

}


  LooseAttachEndAttribute::LooseAttachEndAttribute()
  {

  }

  LooseAttachEndAttribute::~LooseAttachEndAttribute()
  {


  }
}