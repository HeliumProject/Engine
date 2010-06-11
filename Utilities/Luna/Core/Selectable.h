#pragma once

#include "Object.h"

#include "Foundation/Container/OrderedSet.h"

namespace Luna
{
  class ManiuplatorAdapterCollection;
  class Enumerator;
  class EnumerateElementArgs;

  class LUNA_CORE_API Selectable : public Object
  {
  protected:
    bool m_Selected;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Selectable, Object );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    Selectable();
    virtual ~Selectable();

    // Is this object currently selectable?
    //  Sometimes objects can on a per-instance or per-type basis decided to NOT be selectable
    //  This prototype exposes the capability to NOC_OVERRIDE the selection of an object
    virtual bool IsSelectable() const;

    // Get/Set selected state
    virtual bool IsSelected() const;
    virtual void SetSelected(bool);

    // do enumeration of applicable attributes on this object
    virtual void ConnectProperties(EnumerateElementArgs& args);

    // validate a named panel as usable
    virtual bool ValidatePanel(const std::string& name);
  };

  // Smart pointer to an Selectable
  typedef Nocturnal::SmartPtr< Selectable > LSelectablePtr;

  // vector of selectable objects
  typedef Nocturnal::OrderedSet<Selectable*> OS_SelectableDumbPtr;
}