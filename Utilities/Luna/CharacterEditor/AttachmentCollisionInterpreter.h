#pragma once

#include "Reflect/Serializer.h"
#include "Inspect/StringData.h"
#include "InspectReflect/ReflectArrayInterpreter.h"
#include "Content/LooseAttachCollisionAttribute.h"

namespace Inspect
{
  class Button;
  class container;
}

namespace Reflect
{
  class Element;
  class Field;
}

namespace Luna
{
  class LAttachmentCollisionInterpreter : public Inspect::ReflectArrayInterpreter
  {
  public:
    LAttachmentCollisionInterpreter (Inspect::Container* container);

    virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Inspect::Container* parent) NOC_OVERRIDE;

    void TranslateInputTUIDContainer( Reflect::TranslateInputEventArgs& args );
    void TranslateOutputTUIDContainer( Reflect::TranslateOutputEventArgs& args );

  protected:

    // callbacks
    virtual Inspect::ActionPtr AddAddButton( Inspect::List* list ) NOC_OVERRIDE;
    
    void MyOnAdd( Inspect::Button* button );
    
    virtual void OnCreateFieldSerializer( Reflect::SerializerPtr & s ) NOC_OVERRIDE;

  private:

    const Reflect::Field*                       m_Field;
    Content::LooseAttachCollisionAttributePtr   m_LooseAttachAttributePtr;
    Inspect::Container*                         m_Parent;

    // tuid / display lookups
    std::map<tuid, std::string>                 m_TuidDisplay;
    std::map<std::string, tuid>                 m_DisplayToTuid;
  };

  typedef Nocturnal::SmartPtr<LAttachmentCollisionInterpreter> LAttachmentCollisionInterpreterPtr;
  typedef std::vector< LAttachmentCollisionInterpreterPtr > V_LAttachmentCollisionInterpreterPtrSmartPtr;
}