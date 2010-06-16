#pragma once

#include "API.h"
#include "Application/Inspect/Widgets/Control.h"

namespace Inspect
{
  class ClientDataControl : public Reflect::AbstractInheritor<ClientDataControl, Reflect::Object>
  {
  public:
    Control* m_Control;

    ClientDataControl( Control* control )
      : m_Control ( control )
    {

    }

    virtual ~ClientDataControl()
    {

    }
  };
 
  class ClientDataFilter  : public Reflect::AbstractInheritor<ClientDataFilter, Reflect::Object>
  {
  public:
    List*       m_List;
    i32         m_ClassType;
    std::string m_FinderSpec;

    ClientDataFilter( List* list, i32 classType, const std::string& specName )
      : m_List( list )
      , m_ClassType( classType )
      , m_FinderSpec( specName )
    {

    }

    virtual ~ClientDataFilter()
    {

    }
  };
}

namespace InspectReflect
{
  INSPECTREFLECT_API void Initialize();
  INSPECTREFLECT_API void Cleanup();
}