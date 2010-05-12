#pragma once

#include "Serializer.h"

namespace Reflect
{
  // Delimiter to use when representing the data in the container as a 
  // single string.  If your data contains this delimiter naturally, the
  // string representation may not show up properly.
  static const char* s_ContainerItemDelimiter = "\n";

  class REFLECT_API ContainerSerializer : public Serializer
  {
  public:
    REFLECT_DECLARE_ABSTRACT( ContainerSerializer, Serializer )

    ContainerSerializer();

    virtual size_t GetSize() const = 0;
    virtual void Clear() = 0;
   };

  typedef Nocturnal::SmartPtr<ContainerSerializer> ContainerSerializerPtr;

  class REFLECT_API ElementContainerSerializer : public ContainerSerializer
  {
  public:
    REFLECT_DECLARE_ABSTRACT( ElementContainerSerializer, ContainerSerializer );

    i32 m_TypeID;

    ElementContainerSerializer();
  };

  typedef Nocturnal::SmartPtr<ContainerSerializer> ContainerSerializerPtr;
}
