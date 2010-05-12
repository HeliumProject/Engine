#pragma once

#include "ReflectFieldInterpreter.h"
#include "ReflectBitfieldCheckBox.h"

namespace Inspect
{
  class INSPECTREFLECT_API ReflectBitfieldInterpreter : public ReflectFieldInterpreter
  {
  public:
    ReflectBitfieldInterpreter( Container* container );

    virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<ReflectBitfieldInterpreter> ReflectBitfieldInterpreterPtr;
  typedef std::vector< ReflectBitfieldInterpreterPtr > V_ReflectBitfieldInterpreterSmartPtr;
}